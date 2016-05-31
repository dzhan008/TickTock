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

unsigned char gameOn = 0;
unsigned char seed = 1;

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;


//Globals
unsigned char tempA;
unsigned char tempB;
unsigned char model_num;

void adc_init()
{
	// AREF = AVcc
	ADMUX = (1<<REFS0);
	
	// ADC Enable and prescaler of 128
	// 16000000/128 = 125000
	ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
}

uint16_t ReadADC(uint16_t ch){
	//Select ADC Channel. ch must be 0-7
	ADMUX = (ADMUX & 0xF8) | (ch & 0x1F);
	ADCSRB = (ADCSRB & 0xDF) | (ch & 0x20);
	//Start single conversion
	ADCSRA |= (1<<ADSC);
	while (ADCSRA & (1<<ADSC));
	return (ADC);
}

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

/****************************PUZZLE 1 LIGHT CATCH ********************************/

enum SM1_States { SM1_Init, SM1_Pick, SM1_Wrong, SM1_Input,  } SM1_State;

void TickFct_State_machine_1() {

	static unsigned char numCorrect = 0;
	static unsigned char t = 0;

	switch(SM1_State) { // Transitions
		case -1:
		SM1_State = SM1_Init;
		break;
		case SM1_Init:
		if (gameOn == 1) {
			SM1_State = SM1_Pick;
		}
		else
		{
			SM1_State = SM1_Init;
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
		else if(tempA == 0x00 || tempA == 0X01 || tempA == 0x02 || tempA == 0x20)
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
			PORTC = PORTC & 0XE3 | choices[i];
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
		srand(seed);
		i = rand() % 3;

		if(numCorrect == 5)
		{
			PORTC = PORTC | 0x1C;
			if(USART_IsSendReady(1))
			{
				USART_Send(0x01, 1);
			}
		}
		else
		{
				PORTC = PORTC & 0XE3 | choices[i];
		}
		break;
		case SM1_Input:
		break;
		case SM1_Wrong:
		if(t % 2 == 0)
		PORTC = PORTC | 0x1C; //masking done to account for simon says
		else
		PORTC = PORTC & 0xE3;
		break;
		default: // ADD default behaviour below
		break;
	} // State actions

}

/************************************PUZZLE 2 SIMON SAYS ********************************/
unsigned char ss_sequences[4] = {0x02, 0x01, 0x20, 0x40};
unsigned char buffer[4];
unsigned char i_index = 0;
unsigned char b_index = 0;
unsigned char c_index = 0;
unsigned char t_1 = 0;
unsigned char timer = 0;
unsigned char num_correct = 0;
unsigned char req_correct = 4;
unsigned char curr_length = 0;
unsigned char length = 4;

//ADC Configuration
unsigned short adc_result_0 = 0;
unsigned short adc_result_1 = 0;
unsigned short UR = 1023;
unsigned short L = 31;
unsigned short SLR = 3;
unsigned short D = 244;
unsigned short SUD = 546;

enum SM2_States { SM2_Init, SM2_Randomize, SM2_Flash, SM2_Wrong, SM2_Select, SM2_Confirm, SM2_Win } SM2_State;

TickFct_Simon_Says() {
	switch(SM2_State) { // Transitions
		case -1:
		SM2_State = SM2_Init;
		break;
		case SM2_Init:
		if (tempA == 0x20 && gameOn == 1) {
			SM2_State = SM2_Randomize;
		}
		else
		{
			SM2_State = SM2_Init;
		}
		break;
		case SM2_Randomize:
		if (1) {
			SM2_State = SM2_Flash;
		}
		break;
		case SM2_Flash:
		if (curr_length < length) {
			SM2_State = SM2_Flash;
		}
		else if (curr_length > length) {
			SM2_State = SM2_Select;
		}
		break;
		case SM2_Wrong:
		if (t_1 == 8) {
			SM2_State = SM2_Init;
			t_1 = 0;
			PORTC = PORTC & 0X1C;
		}
		else if (t_1 < 8) {
			SM2_State = SM2_Wrong;
			t_1++;
		}
		break;
		case SM2_Select:
		if(tempA == 0x20)
		{
			SM2_State = SM2_Confirm;
		}
		else if(num_correct == 4)
		{
			SM2_State = SM2_Win;
			if(USART_IsSendReady(1))
			{
				USART_Send(0x01, 1);
	 		}
		}
		else
		{
			SM2_State = SM2_Select;
		}
		break;
		case SM2_Confirm:
		break;
		case SM2_Win:
		break;
		default:
		SM2_State = SM2_Init;
	} // Transitions

	switch(SM2_State) { // State actions
		case SM2_Init:
		curr_length = 0;
		num_correct = 0;
		i_index = 0;
		b_index = 0;
		c_index = 0;
		timer = 0;
		t_1 = 0;
		break;
		case SM2_Randomize:
		srand(seed);
		for(int i = 0; i < 4; i++)
		{
			int in = rand() % 4;
			buffer[i] = ss_sequences[in];
		}
		break;
		case SM2_Flash:
		if(timer == 20)
		{
			PORTC = PORTC & 0x1C | buffer[curr_length];
			timer = 0;
			curr_length++;
			if(curr_length > length)
			{
				PORTC = PORTC & 0x1C;
			}
		}
		else if(timer == 10)
		{
			PORTC = PORTC & 0x1C;
			timer++;
		}
		else
		{
			timer++;
		}

		break;
		case SM2_Wrong:
		if(t_1 % 2 == 0)
		PORTC = PORTC | 0xE3; //masking done to account for light catch game
		else
		PORTC = PORTC & 0x1C;
		break;
		case SM2_Select:
		adc_result_0 = ReadADC(0);
		adc_result_1 = ReadADC(1);
		
		if(adc_result_1 == UR || adc_result_1 > 991)
		{
			if(c_index == 3)
			{
				c_index = 0;
			}
			else
			{
				c_index++;
			}
			PORTC = PORTC & 0x1C | ss_sequences[c_index]; //Right

		}
		else
		{
			
		}
		break;
		case SM2_Confirm:
		if(ss_sequences[c_index] == buffer[num_correct])
		{
			num_correct++;
			SM2_State = SM2_Select;
		}
		else
		{
			SM2_State = SM2_Wrong;
		}
		break;
		case SM2_Win:
		PORTC = PORTC | 0xE3;
		break;
		default: // ADD default behaviour below
		break;
	} // State actions

}

enum SM3_States { SM3_Game } SM3_State;

TickFct_GameController() {
	/*VARIABLES MUST BE DECLARED STATIC*/
	/*e.g., static int x = 0;*/
	/*Define user variables for this state machine here. No functions; make them global.*/
	switch(SM3_State) { // Transitions
		case -1:
		SM3_State = SM3_Game;
		break;
		case SM3_Game:
		if (1) {
			SM3_State = SM3_Game;
		}
		break;
		default:
		SM3_State = SM3_Game;
	} // Transitions

	switch(SM3_State) { // State actions
		case SM3_Game:
		if(USART_HasReceived(0))
		{
			unsigned char result = USART_Receive(0);
			if(result != 0x02)
			{
				gameOn = 1;
				model_num = result;

			}
			else if(result == 0x02)
			{
				gameOn = 0;
			}
			USART_Flush(0);
		}
		break;
		default: // ADD default behaviour below
		break;
	} // State actions
}

enum SM4_States { SM4_Init, SM4_Check, SM4_Complete } SM4_State;

TickFct_Wire_Pull() {
	/*VARIABLES MUST BE DECLARED STATIC*/
	/*e.g., static int x = 0;*/
	/*Define user variables for this state machine here. No functions; make them global.*/
	static unsigned char array[5] = {0x01, 0x02, 0x04, 0x08, 0x10};
	static unsigned char wirePulled = 0;
	switch(SM4_State) { // Transitions
		case -1:
		SM4_State = SM4_Init;
		break;
		case SM4_Init:
		if (gameOn == 0) {
			SM4_State = SM4_Init;
		}
		else if (gameOn == 1) {
			SM4_State = SM4_Check;
		}
		break;
		case SM4_Check:
		if (wirePulled == 0) {
			SM4_State = SM4_Check;
		}
		else if (wirePulled == 1) {
			SM4_State = SM4_Complete;
			if(USART_IsSendReady(1))
			{
				USART_Send(0x01, 1);
			}
			
		}
		break;
		case SM4_Complete:
		break;
		default:
		SM4_State = SM4_Init;
	} // Transitions

	switch(SM4_State) { // State actions
		case SM4_Init:
		break;
		case SM4_Check: //TO DO IMPLEMENT FUNCTIONALITY
		if((tempB & 0x01) == 0x01)
		{
			PORTC = 0xFF;
		}
		else
		{
			PORTC = 0x00;
		}
		break;
		case SM4_Complete:
		break;
		default: // ADD default behaviour below
		break;
	} // State actions
}

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0x00; PORTB = 0xFF;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0xFF;
	initUSART(0);
	initUSART(1);
	adc_init();
	USART_Flush(0);
	/* Replace with your application code */
	TimerSet(100);
	TimerOn();
	srand(seed);
	SM1_State = SM1_Init;
	SM2_State = SM2_Init;
	SM3_State = SM3_Game;
	while (1)
	{
		tempA = ~PINA;
		tempB = ~PINB;
		TickFct_State_machine_1();
		TickFct_Simon_Says();
		TickFct_GameController();
		TickFct_Wire_Pull();
		while(!TimerFlag);
		TimerFlag = 0;
		seed = seed + 10;
	}
}

