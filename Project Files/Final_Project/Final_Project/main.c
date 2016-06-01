/*
 * Final_Project.c
 *
 * Created: 5/25/2016 3:27:04 PM
 * Author : skill
 */ 

#include <avr/io.h>
#include <keypad.h>
#include <queue.h>
#include <avr/interrupt.h>
#include <scheduler.h>
#include "io.h"
#include "usart_atmega1284.h"

//Timer Variables
volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

//Globals
unsigned char seed = 1;
unsigned char gameOn = 0;
unsigned char gameEnd = 0;
unsigned char win = 0;
unsigned char timerOn = 0;
unsigned char lives = 3;
unsigned char puzzleComplete = 0;
unsigned char puzzlesFinished = 0;
unsigned char totalPuzzles = 3;
unsigned char difficulty = 0;
unsigned char ticks = 30;
unsigned char time_till_shift = 5;
unsigned char sequence[5] = {0, 0, 0, 0, 0};
unsigned char models[5] = {'A', 'B', 'X', 'M', 'X'};
unsigned char des_model;
unsigned char tempD;
unsigned char keypad_in_use = 0;

//For Hacking Bomb
unsigned char hacked = 0;
unsigned char hackGame = 0;
unsigned char totalMazes = 3;


//Task Setup

static task task1, task2, task3, task4, task5, task6;
task *tasks[] = {&task1, &task2, &task3, &task4, &task5, &task6};
const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

//Shift reg transmitter
void t_data(unsigned char data){	int i;	for(i = 7; i >= 0; --i) {		//for (i = 7; i >= 0; ­­--i) {
		// Sets SRCLR to 1 allowing data to be set
		// Also clears SRCLK in preparation of sending data
		PORTC = 0x08;
		// set SER = next bit of data to be sent.
		PORTC |= ((data >> i) & 0x01);
		// set SRCLK = 1. Rising edge shifts next bit of data into the shift register
		PORTC |= 0x04;
	}
	// set RCLK = 1. Rising edge copies data from the “Shift” register to the “Storage” register
	PORTC |= 0x02;
	// clears all lines in preparation of a new transmission
	PORTC = 0x00;}	

/*********************************************MENU**************************************/

enum SM1_States { SM1_Menu, SM1_Start, SM1_Set_Difficulty, SM1_End } SM1_State;
int TickFct_Menu(int state) {
	/*VARIABLES MUST BE DECLARED STATIC*/
	/*e.g., static int x = 0;*/
	/*Define user variables for this state machine here. No functions; make them global.*/
    static unsigned char model_num;
	static unsigned char start = 0;
	static unsigned char key_input = 0;
	static unsigned char set_difficulty = 0;

	switch(state) { // Transitions
		case -1:
		state = SM1_Menu;
		LCD_DisplayString(1, "A. Play         B. Difficulty");
		break;
		case SM1_Menu:
		if (start == 1) {
			state = SM1_Start;
			//Initialization
			srand(seed);
			model_num = rand() % 5; //Weirdly enough I can't do rand() % 4 otherwise Model M gets picked all the time...
			des_model = models[model_num];
			if(difficulty == 0)
			{
				ticks = 90;
				time_till_shift = 15;
				puzzlesFinished = 3;
				lives = 5;
			}
			else if(difficulty == 1)
			{
				ticks = 60;
				time_till_shift = 10;
				puzzleComplete = 4;
				lives = 3;
			}
			else if(difficulty == 2)
			{
				ticks = 30;
				time_till_shift = 5;
				puzzleComplete = 4;
				lives = 1;
			}
			if(USART_IsSendReady(0))
			{
				USART_Send(des_model, 0); //Code for GameStart
			}
			LCD_ClearScreen();
		}
		else if(set_difficulty == 1)
		{
			state = SM1_Set_Difficulty;
			LCD_DisplayString(1, "A. Normal       B. Hard");
		}
		break;
		case SM1_Start:
		if (gameEnd == 1) {
			state = SM1_End;
			if(win == 1)
			{
				LCD_DisplayString(1, "You win!");
			}
			else
			{
				LCD_DisplayString(1, "Game Over!");
			}
		}
		else
		{
			state = SM1_Start;
		}
		break;
		case SM1_Set_Difficulty:
		if(set_difficulty == 0)
		{
			state = SM1_Menu;
			LCD_DisplayString(1, "A. Play         B. Difficulty");
		}
		else
		{
			state = SM1_Set_Difficulty;
		}
		break;
		case SM1_End:
		break;
		default:
		state = -1;
	} // Transitions

	switch(state) { // State actions
		case SM1_Menu:
		key_input = GetKeypadKey();

		switch (key_input) {
			case '\0': break; // All 5 LEDs on
			case 'A': start = 1; break;
			case 'B': set_difficulty = 1; break;
			default: break;
			}
		break;
		case SM1_Start:
		//LCD_DisplayString(1, "Game Start!");
		gameOn = 1;
		break;
		case SM1_Set_Difficulty:
		key_input = GetKeypadKey();
		switch (key_input) {
			case '\0': break; // All 5 LEDs on
			case 'A': difficulty = 0; set_difficulty = 0; break;
			case 'B': difficulty = 1; set_difficulty = 0; break;
			case 'C': difficulty = 2; set_difficulty = 0; break;
			default: break;
		}
		break;
		case SM1_End:

		break;
		default: // ADD default behaviour below
		break;
	} // State actions
	SM1_State = state;
	return state;
}

/*********************************************TIMER**************************************/

enum SM2_States { SM2_Init, SM2_Tick };
int TickFct_Timer(int state) {
	/*VARIABLES MUST BE DECLARED STATIC*/
	/*e.g., static int x = 0;*/
	static unsigned char t = 0;
	static unsigned char shift_val = 0x7F;
	switch(state) { // Transitions
		case SM2_Init:
		if(gameOn == 1)
		{
			state = SM2_Tick;
		}
		else
		{
			state = SM2_Init;
		}

		break;
		case SM2_Tick:
		if (t < 150) {
			state = SM2_Tick;
			t++;
		}
		break;
		default:
		state = SM2_Init;

	} // Transitions

	switch(state) { // State actions
		case SM2_Init:
		t_data(shift_val);
		t = 0;
		break;
		case SM2_Tick:
		if(t % time_till_shift == 0)
		{
			shift_val = shift_val >> 1;
			t_data(shift_val);
							//LCD_DisplayString(1, "Shifting!");
		}
		if(t == ticks){
				gameEnd = 1;
		}

		break;
		default: // ADD default behaviour below
		break;
	} // State actions
	return state;
}

/*********************************************RGB PUZZLE**************************************/

enum SM3_States { SM3_Init, SM3_Pick, SM3_Input, SM3_Complete } SM3_State;
int TickFct_Light_Catch(int state) {
	//VARIABLES MUST BE DECLARED STATIC
	//e.g., static int x = 0;
	static unsigned char choices[3] = {0x40, 0x20, 0x10}; //0x10 = Red 0x20 = Green 0x40 = Blue
	static unsigned char inputs[3] = {7,6,5};
	static unsigned char t = 0;
	static unsigned char i = 0;
	static unsigned char finished = 0;
	static unsigned char flicker = 0;
	static unsigned char lit = 0;
	static unsigned char key_input;
	switch(state) { // Transitions
		case -1:
		state = SM3_Init;
		break;
		case SM3_Init:
		if (gameOn == 1) {
			state = SM3_Pick;
		}
		else
		{
			state = SM3_Init;
		}
		break;
		case SM3_Pick:
		if(finished == 1)
		{
			state = SM3_Complete;
			puzzleComplete = 1;
			PORTD = PORTD & 0x87;
		}
		else{
			state = SM3_Input;
			t++;
		}
		break;
		case SM3_Input:
		if(0)
		{
			state = SM3_Complete;
		}
		else
		{
			state = SM3_Pick;
		}
		break;
		case SM3_Complete: break;
		default:
		state = -1;
	} // Transitions

	switch(state) { // State actions
		case SM3_Init:
		t = 0;
		i = 0;
		srand(seed);
		i = rand() % 3;
		if(i == 0)
		{
			flicker = 1;
		}
		else if(i == 1)
		{
			lit = 1;
		}
		break;
		case SM3_Pick:
		key_input = GetKeypadKey();
		if(keypad_in_use == 0)
		{
			switch (key_input) {
				case '\0': break; // All 5 LEDs on
				case 'C':
				if(choices[i] == 0x10 && lit == 1) //Red
				{
					finished = 1;
				}
				else if (choices[i] == 0x20 && flicker == 1) //Green
				{
					finished = 1;
				}
				else if (choices[i] == 0x40 && lit == 0) //Blue
				{
					finished = 1;
				}
				else
				{
					puzzleComplete = 2;
				}
				break;

				default: break;
			}
		}
		i = rand() % 3;
		PORTD = choices[i];
		if(flicker == 1)
		{
			if (t % 2 == 0)
			{
				PORTD = PORTD | 0x80;
			}
		}
		else if (lit == 1)
		{
			PORTD = PORTD | 0x80;
		}


		
		break;
		case SM3_Input:
		break;
		case SM3_Complete:
		default: // ADD default behaviour below
		break;
	} // State actions
	SM3_State = state;
	return state;
}

/*********************************************KEY GENERATION**************************************/

enum SM4_States { SM4_Wait, SM4_M_Key, SM4_Fail, SM4_Init } SM4_State;
int TickFct_Key_Gen(int state) {
	/*VARIABLES MUST BE DECLARED STATIC*/
	/*e.g., static int x = 0;*/
	/*Define user variables for this state machine here. No functions; make them global.*/
	static unsigned char key;
	static unsigned char keys[16] = {'1','2','3','4','5','6','7','8','9','0','A','B','C','D','*','#'};

	static unsigned char i;
	switch(state) { // Transitions
		case -1:
		state = SM4_Init;
		break;
		case SM4_Wait:
		//Conditions: 0 = Nothing Happened | 1 = Success | 2 = Fail
		if (puzzleComplete == 1) {
			state = SM4_M_Key;
		}
		else if(puzzleComplete == 2) 
		{
			state = SM4_Fail;
		}
		else if (puzzleComplete == 0) {
			state = SM4_Wait;
		}
		break;
		case SM4_M_Key:
		if (1) {
			state = SM4_Wait;
		}
		break;
		case SM4_Fail:
		if(1){
			state = SM4_Wait;
		}
		case SM4_Init:
		if (gameOn == 0) {
			state = SM4_Init;
		}
		else if (gameOn == 1) {
			state = SM4_Wait;
		}
		break;
		default:
		state = -1;
	} // Transitions

	switch(state) { // State actions
		case SM4_Wait:
		if(USART_HasReceived(1))
		{
			unsigned char val = USART_Receive(1); //TO DO: Account for failed attempts?
			if(val == 0x01)
			{
				puzzleComplete = 1;
			}
			else if (val == 0x02)
			{
				puzzleComplete = 2;
			}

			USART_Flush(1);
		}
		break;
		case SM4_M_Key:
		key = rand() % 16;
		sequence[i] = keys[key];
		puzzleComplete = 0;
		LCD_DisplayString(1, "Puzzle Done!    Code:");
		LCD_WriteData(sequence[i]);
		i++;
		break;
		case SM4_Fail:
		lives--;
		LCD_DisplayString(1, "Puzzle Failed!   Lives Left:");
		LCD_WriteData(lives + '0');
		puzzleComplete = 0;
		if(lives == 0)
		{
			gameEnd = 1;
		}
		case SM4_Init:
		i = 0;
		break;
		default: // ADD default behaviour below
		break;
	} // State actions
	SM4_State = state;
	return state;
}

/*********************************************KEY INPUT**************************************/

enum SM5_States { SM5_Init, SM5_Wait, SM5_Input, SM5_Inspect, SM5_Lives, SM5_Win } SM5_State;
int TickFct_Key_Input(int state) {
	/*VARIABLES MUST BE DECLARED STATIC*/
	/*e.g., static int x = 0;*/
	static unsigned char user_input[5] = {'0', '0', '0', '0', '0'};
	static unsigned char key_input;
	static unsigned char correct = 1;
	static unsigned char index = 0;
	static unsigned char input = 0;
	static unsigned char inspect = 0;
	static unsigned char check_lives = 0;
	static unsigned char numInputs;

	switch(state) { // Transitions
		case -1:
		state = SM5_Init;
		break;
		case SM5_Init:
		if (gameOn == 1) {
			state = SM5_Wait;
			check_lives = 1;
		}
		else if (gameOn == 0) {
			state = SM5_Init;
		}
		break;
		case SM5_Wait:
		if (input == 1) {
			state = SM5_Input;
			LCD_DisplayString(1, "Input Code:");
			keypad_in_use = 1;
		}
		else if (inspect == 1) {
			state = SM5_Inspect;
		}
		else if (check_lives == 1){
			state = SM5_Lives;
		}
		else if(hackGame == 1)
		{
			state = SM5_Wait;
			keypad_in_use = 1;
		}
		else
		{
			state = SM5_Wait;
		}
		break;
		case SM5_Input:
		//	615C0
		if(puzzlesFinished == totalPuzzles && index == totalPuzzles)
		{
			for(int i = 0; i < totalPuzzles; i++)
			{
				if(user_input[i] != sequence[i])
				{
					correct = 0;
					break;
				}
			}
			if(correct == 1)
			{
				state = SM5_Win;
			}
			else
			{
				LCD_DisplayString(1, "Wrong Input!");
				input = 0;
				index = 0;
				correct = 1;
				keypad_in_use = 0;
				state = SM5_Wait;
			}
		}

		break;
		case SM5_Inspect:
		if (1) {
			state = SM5_Wait;
			inspect = 0;
		}
		break;
		case SM5_Lives:
		if (1){
			state = SM5_Wait;
			check_lives = 0;
		}
		break;
		case SM5_Win:
		gameEnd = 1;
		win = 1;
		break;
		default:
		state = -1;
	} // Transitions

	switch(state) { // State actions
		case SM5_Init:
		index = 0;
		break;
		case SM5_Wait:
		key_input = GetKeypadKey();
		switch (key_input) {
			case '\0': break;
			case '#': input = 1;  break;
			case '*': inspect = 1; break;
			case '0': check_lives = 1; break;
			case 'D': hackGame = 1; break;
			default: break;
		}
		break;
		case SM5_Input:
		//Keypad code here
		key_input = GetKeypadKey();
		switch (key_input) {
			case '\0': break; // All 5 LEDs on
			case '1': LCD_WriteData('1'); user_input[index] = '1'; index++; break; // hex equivalent
			case '2': LCD_WriteData('2'); user_input[index] = '2'; index++; break;
			case '3': LCD_WriteData('3'); user_input[index] = '3'; index++; break;
			case '4': LCD_WriteData('4'); user_input[index] = '4'; index++; break;
			case '5': LCD_WriteData('5'); user_input[index] = '5'; index++; break;
			case '6': LCD_WriteData('6'); user_input[index] = '6'; index++; break;
			case '7': LCD_WriteData('7'); user_input[index] = '7'; index++; break;
			case '8': LCD_WriteData('8'); user_input[index] = '8'; index++; break;
			case '9': LCD_WriteData('9'); user_input[index] = '9'; index++; break;
			case 'A': LCD_WriteData('A'); user_input[index] = 'A'; index++; break;
			case 'B': LCD_WriteData('B'); user_input[index] = 'B'; index++; break;
			case 'C': LCD_WriteData('C'); user_input[index] = 'C'; index++; break;
			case 'D': LCD_WriteData('D'); user_input[index] = 'D'; index++; break;
			case '*': LCD_WriteData('*'); user_input[index] = '*'; index++; break;
			case '0': LCD_WriteData('0'); user_input[index] = '0'; index++; break;
			case '#': LCD_WriteData('#'); user_input[index] = '#'; index++; break;
			default: break;
		}
		break;
		case SM5_Inspect:
		if(hacked == 1)
		{
			LCD_DisplayString(1, "Model:");
			LCD_WriteData(des_model);
		}
		else
		{
			LCD_DisplayString(1, "Access Denied.");		
		}

		break;
		case SM5_Lives:
		LCD_DisplayString(1, "Lives:");
		LCD_WriteData(lives + '0');
		break;
		case SM5_Win:
		win = 1;
		break;
		default: // ADD default behaviour below
		break;
	} // State actions
	SM5_State = state;
	return state;
}

/*********************************************HACKING PUZZLE **************************************/

enum SM7_States { SM7_Init, SM7_Create_Field, SM7_Movement, SM7_Fail, SM7_Success } SM7_State;
int TickFct_HackGame(int state) {
	/*VARIABLES MUST BE DECLARED STATIC*/
	/*e.g., static int x = 0;*/
	/*Define user variables for this state machine here. No functions; make them global.*/
	static unsigned char i = 0;
	static unsigned char playerPos = 1;
	static unsigned char up = 1;
	static unsigned char down = 0;
	static unsigned char goal = 16;
	static unsigned char currMaze = 0;
	static unsigned char position_1[5] = {2, 5, 6, 10, 12};
	static unsigned char position_2[5] = {25, 17, 28, 21, 18};
	static unsigned char position_3[5] = {13, 26, 30, 3, 4};
	static unsigned char key_input = 0;

	switch(state) { // Transitions
		case -1:
		state = SM7_Init;
		break;
		case SM7_Init:
		if (hackGame == 0) {
			state = SM7_Init;
		}
		else if (hackGame == 1) {
			state = SM7_Create_Field;
			LCD_DisplayString(1, "Hacking Module...");
		}
		break;
		case SM7_Create_Field:
		if (1) {
			state = SM7_Movement;
		}
		break;
		case SM7_Movement:
		if (playerPos == position_1[i] || playerPos == position_2[i] || playerPos == position_3[i]) {
			state = SM7_Fail;
		}
		else if (playerPos == goal)
		{
			currMaze++;
			if(currMaze == 3)
			{
				hacked = 1;
				hackGame = 0;
				keypad_in_use = 0;
				state = SM7_Success;
				LCD_DisplayString(1, "Module Hacked!");
			}
			else
			{
				state = SM7_Create_Field;
			}

		}
		else
		{
			state = SM7_Movement;
			LCD_Cursor(playerPos);
		}
		break;
		case SM7_Fail:
		if (1) {
			state = SM7_Create_Field;
		}
		break;
		case SM7_Success:
		if(1){
			state = SM7_Success;
		}
		break;
		default:
		state = -1;
	} // Transitions

	switch(state) { // State actions
		case SM7_Init:
		break;
		case SM7_Create_Field:
		srand(seed);
		i = rand() % 5;
		LCD_ClearScreen();

		LCD_Cursor(position_1[i]); //Don't ask why...I honestly don't know.
		LCD_Cursor(position_1[i]);
		LCD_WriteData('#');

		LCD_Cursor(position_2[i]);
		LCD_WriteData('#');
		
		LCD_Cursor(position_3[i]);
		LCD_WriteData('#');
		
		LCD_Cursor(goal);
		LCD_WriteData('1');
		
		playerPos = 1;
		up = 1;
		down = 0;
		
		LCD_Cursor(playerPos);
		
		
		break;
		case SM7_Movement:
		key_input = GetKeypadKey();
		switch (key_input) {
			case '\0': break;
			case '2':
			if(up == 0)
			{
				playerPos -= 16;
				up = 1;
				down = 0;
			}  
			break;
			case '5': 
			if(down == 0)
			{
				playerPos += 16;
				down = 1;
				up = 0;
			} 
			break;
			case '4': 
			if(playerPos == 1 || playerPos == 17)
			{
				//Do nothing! For some reason != doesn't work...
			}
			else
			{
				playerPos--;
			}
			break;
			case '6': 
			if(playerPos == 16 || playerPos == 32)
			{
			
			} 
			else
			{
				playerPos++;
			}
			break;
			default: break;
		}
		break;
		case SM7_Fail:
		LCD_DisplayString(1, "Hack Failed!");
		currMaze = 0;
		break;
		case SM7_Success:
		break;
		default: // ADD default behaviour below
		break;
	} // State actions
	SM7_State = state;
	return state;
}

/*******************************************************************************************PUZZLES END HERE***********************************************/

/*********************************Timer Functions ***********************************/
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
	// Scheduler code
	/*
	for ( int i = 0; i < numTasks; i++ ) {
		// Task is ready to tick
		if ( tasks[i]->elapsedTime == tasks[i]->period ) {
			// Setting next state for task
			tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
			// Reset the elapsed time for next tick.
			tasks[i]->elapsedTime = 0;
		}
		tasks[i]->elapsedTime += 1;
	}
	*/
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
/*********************************Timer Functions END ***********************************/


int main(void)
{
	DDRA = 0xFF; PORTA = 0x00;
	DDRB = 0xF0; PORTB = 0x0F; // PB7..4 outputs init 0s, PB3..0 inputs init 1s
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	//Period for the tasks
	unsigned long int SMTick1_calc = 200;

	//Calculating GCD
	unsigned long int tmpGCD = 1;

	//Greatest common divisor for all tasks or smallest time unit for tasks.
	unsigned long int GCD = tmpGCD;

	//Recalculate GCD periods for scheduler
	unsigned long int SMTick1_period = SMTick1_calc/GCD;

	// Task 1
	task1.state = -1;//Task initial state.
	task1.period = 10;//Task Period.
	task1.elapsedTime = 0;//Task current elapsed time.
	task1.TickFct = &TickFct_Menu;//Function pointer for the tick.

	// Task 2
	task2.state = -1;//Task initial state.
	task2.period = 100;//Task Period.
	task2.elapsedTime = 0;//Task current elapsed time.
	task2.TickFct = &TickFct_Timer;//Function pointer for the tick.

	// Task 3
	task3.state = -1;//Task initial state.
	task3.period = 30;//Task Period.
	task3.elapsedTime = 0;//Task current elapsed time.
	task3.TickFct = &TickFct_Light_Catch;//Function pointer for the tick.

	// Task 4
	task4.state = -1;//Task initial state.
	task4.period = 30;//Task Period.
	task4.elapsedTime = 0;//Task current elapsed time.
	task4.TickFct = &TickFct_Key_Gen;//Function pointer for the tick.

	// Task 5
	task5.state = -1;//Task initial state.
	task5.period = 20;//Task Period.
	task5.elapsedTime = 0;//Task current elapsed time.
	task5.TickFct = &TickFct_Key_Input;//Function pointer for the tick.

	// Task 6
	task6.state = -1;//Task initial state.
	task6.period = 10;//Task Period.
	task6.elapsedTime = 0;//Task current elapsed time.
	task6.TickFct = &TickFct_HackGame;//Function pointer for the tick.
	
	initUSART(0);
	initUSART(1);
	TimerSet(10);
	TimerOn();
	LCD_init();
	srand(seed);

    while (1) 
    {
		tempD = ~PINC;
		for ( int i = 0; i < numTasks; i++ ) {
			// Task is ready to tick
			if ( tasks[i]->elapsedTime == tasks[i]->period ) {
				// Setting next state for task

				tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
				// Reset the elapsed time for next tick.
				tasks[i]->elapsedTime = 0;
			}
			tasks[i]->elapsedTime += 1;
		}
		while(!TimerFlag);
		TimerFlag = 0;
		seed = seed + 10;

    }
}

