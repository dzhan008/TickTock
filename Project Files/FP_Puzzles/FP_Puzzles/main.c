/*
 * FP_Puzzles.c
 *
 * Created: 5/29/2016 5:48:20 PM
 * Author : skill
 */ 

#include <avr/io.h>
#include <keypad.h>
#include <avr/interrupt.h>
#include "usart_atmega1284.h"

unsigned char gameOn = 1;
unsigned char seed = 1;

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;


//Globals
unsigned char tempA;

void TimerOn()
{
	TCCR1B = 0x0B;

	//AVR output compare register
	//When TCNT1 = 125, 1 ms has passed. So we compare with 125.
	OCR1A = 125;

	//AVR timer interrupt mask register
	TIMSK1 = 0x02;

	//Initialize AVR Counter
	TCNT1 = 0;

	_avr_timer_cntcurr = _avr_timer_M;
	//TimerISR will be called every _avr_timer_cntcurr milliseconds

	//Enable Global Interrupts
	SREG |= 0X80;

}

void TimerOff()
{
	TCCR1B = 0x00;
}

void TimerISR()
{
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect)
{
	_avr_timer_cntcurr--;
	if (_avr_timer_cntcurr == 0)
	{
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet(unsigned long M)
{
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

	unsigned char choices[3] = {0x04, 0x08, 0x10};
	unsigned char i;

enum SM1_States { SM1_Init, SM1_Pick, SM1_Wrong, SM1_Input,  } SM1_State;

void TickFct_State_machine_1() {

	static unsigned char numCorrect = 0;
	static unsigned char t = 0;

	switch(SM1_State) { // Transitions
		case -1:
		SM1_State = SM1_Init;
		break;
		case SM1_Init:
		if (1) {
			SM1_State = SM1_Pick;
		}
		break;
		case SM1_Pick:
		if (1) {
			SM1_State = SM1_Input;
		}
		break;
		case SM1_Input:
		if(tempA == choices[i])
		{
			SM1_State = SM1_Pick;
			numCorrect++;
		}
		else if(tempA == 0x00)
		{
			SM1_State = SM1_Input;
		}
		else if (tempA != choices[i]) {
			SM1_State = SM1_Wrong;
			numCorrect = 0;
		}
		break;
		case SM1_Wrong:
		if (t == 8) {
			SM1_State = SM1_Input;
			t = 0;
			PORTC = choices[i];
		}
		else if (t < 8) {
			SM1_State = SM1_Wrong;
			t++;
		}
		break;
		default:
		SM1_State = SM1_Init;
	} // Transitions

	switch(SM1_State) { // State actions
		case SM1_Init:
		numCorrect = 0;
		break;
		case SM1_Pick:
		i = rand() % 3;

		if(numCorrect == 5)
		{
			PORTC = 0xFF;
			if(USART_IsSendReady(1))
			{
				USART_Send(0x01, 1);
			}
		}
		else
		{
				PORTC = choices[i];
		}
		break;
		case SM1_Input:
		break;
		case SM1_Wrong:
		if(t % 2 == 0)
		PORTC = 0xFF;
		else
		PORTC = 0x00;
		break;
		default: // ADD default behaviour below
		break;
	} // State actions

}

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0xFF;
	unsigned char temp;
	initUSART(0);
	initUSART(1);
	USART_Flush(0);
	/* Replace with your application code */
	TimerSet(100);
	TimerOn();
	srand(seed);
	SM1_State = SM1_Init;
	while (1)
	{
		tempA = ~PINA;
		TickFct_State_machine_1();
		if(USART_HasReceived(0))
		{
			temp = USART_Receive(0);
			PORTA = temp;
			USART_Flush(0);
		}
		while(!TimerFlag);
		TimerFlag = 0;
		seed = seed + 10;
	}
}

