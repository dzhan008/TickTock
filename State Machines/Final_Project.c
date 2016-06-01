/*
This code was automatically generated using the Riverside-Irvine State machine Builder tool
Version 2.7 --- 6/1/2016 0:3:20 PST
*/

#include "rims.h"

unsigned char gameOn = 0;
unsigned char gameEnd = 0;
unsigned char override = 0;
unsigned char win = 0;
unsigned char timerOn = 0;
unsigned char puzzleComplete = 0;
unsigned char puzzlesFinished = 0;
unsigned char totalPuzzles = 5;
unsigned char sequence[5];
typedef struct task {
   int state;
   unsigned long period;
   unsigned long elapsedTime;
   int (*TickFct)(int);
} task;

task tasks[7];

const unsigned char tasksNum = 7;
const unsigned long periodMenu = 100;
const unsigned long periodTimer = 1000;
const unsigned long periodLight_Catch = 1000;
const unsigned long periodKey_Gen = 1000;
const unsigned long periodKey_Input = 1000;
const unsigned long periodHackGame = 1000;
const unsigned long periodSpeakers = 1000;

const unsigned long tasksPeriodGCD = 100;

int TickFct_Menu(int state);
int TickFct_Timer(int state);
int TickFct_Light_Catch(int state);
int TickFct_Key_Gen(int state);
int TickFct_Key_Input(int state);
int TickFct_HackGame(int state);
int TickFct_Speakers(int state);

unsigned char processingRdyTasks = 0;
void TimerISR() {
   unsigned char i;
   if (processingRdyTasks) {
      printf("Period too short to complete tasks\n");
   }
   processingRdyTasks = 1;
   for (i = 0; i < tasksNum; ++i) { // Heart of scheduler code
      if ( tasks[i].elapsedTime >= tasks[i].period ) { // Ready
         tasks[i].state = tasks[i].TickFct(tasks[i].state);
         tasks[i].elapsedTime = 0;
      }
      tasks[i].elapsedTime += tasksPeriodGCD;
   }
   processingRdyTasks = 0;
}
int main() {
   // Priority assigned to lower position tasks in array
   unsigned char i=0;
   tasks[i].state = -1;
   tasks[i].period = periodMenu;
   tasks[i].elapsedTime = tasks[i].period;
   tasks[i].TickFct = &TickFct_Menu;

   ++i;
   tasks[i].state = -1;
   tasks[i].period = periodTimer;
   tasks[i].elapsedTime = tasks[i].period;
   tasks[i].TickFct = &TickFct_Timer;

   ++i;
   tasks[i].state = -1;
   tasks[i].period = periodLight_Catch;
   tasks[i].elapsedTime = tasks[i].period;
   tasks[i].TickFct = &TickFct_Light_Catch;

   ++i;
   tasks[i].state = -1;
   tasks[i].period = periodKey_Gen;
   tasks[i].elapsedTime = tasks[i].period;
   tasks[i].TickFct = &TickFct_Key_Gen;

   ++i;
   tasks[i].state = -1;
   tasks[i].period = periodKey_Input;
   tasks[i].elapsedTime = tasks[i].period;
   tasks[i].TickFct = &TickFct_Key_Input;

   ++i;
   tasks[i].state = -1;
   tasks[i].period = periodHackGame;
   tasks[i].elapsedTime = tasks[i].period;
   tasks[i].TickFct = &TickFct_HackGame;

   ++i;
   tasks[i].state = -1;
   tasks[i].period = periodSpeakers;
   tasks[i].elapsedTime = tasks[i].period;
   tasks[i].TickFct = &TickFct_Speakers;

   ++i;
   TimerSet(tasksPeriodGCD);
   TimerOn();
   
   while(1) { Sleep(); }

   return 0;
}

enum SM1_States { SM1_Menu, SM1_Start, SM1_Win } SM1_State;
int TickFct_Menu(int state) {
   /*VARIABLES MUST BE DECLARED STATIC*/
/*e.g., static int x = 0;*/
/*Define user variables for this state machine here. No functions; make them global.*/

   switch(state) { // Transitions
      case -1:
         state = SM1_Menu;
         break;
      case SM1_Menu:
         if (override == 1) {
            state = SM1_Start;
            LCD_ClearScreen();
         }
         break;
      case SM1_Start:
         if (win == 0) {
            state = SM1_Start;
         }
         else if (win == 1) {
            state = SM1_Win;
         }
         break;
      case SM1_Win:
         break;
      default:
         state = -1;
      } // Transitions

   switch(state) { // State actions
      case SM1_Menu:
         LCD_DisplayString(1, "A. Play");
         LCD_DisplayString(17, "B. Difficulty");
         break;
      case SM1_Start:
         LCD_DisplayString(1, "Game Start!");
         break;
      case SM1_Win:
         LCD_DisplayString(1, "You win!");
         break;
      default: // ADD default behaviour below
         break;
   } // State actions
   SM1_State = state;
   return state;
}


enum SM2_States { SM2_Init, SM2_Tick } SM2_State;
int TickFct_Timer(int state) {
   /*VARIABLES MUST BE DECLARED STATIC*/
/*e.g., static int x = 0;*/
static unsigned char t = 0;
static unsigned char ticks = 30;
static unsigned char shift_val = 0x7F;
   switch(state) { // Transitions
      case -1:
         state = SM2_Init;
         break;
      case SM2_Init:
         if (1) {
            state = SM2_Tick;
         }
         break;
      case SM2_Tick:
         if (t < ticks) {
            state = SM2_Tick;
            t++;
         }
         break;
      default:
         state = -1;
      } // Transitions

   switch(state) { // State actions
      case SM2_Init:
         t_data(shift_val);
         t = 0;
         break;
      case SM2_Tick:
         if(t % 5 == 0)
         {
         shift_val = shift_val >> 1;
         t_data(shift_val);
         }
         if(t == 30)
         gameEnd = 1;
         break;
      default: // ADD default behaviour below
         break;
   } // State actions
   SM2_State = state;
   return state;
}


enum SM3_States { SM3_Init, SM3_Pick, SM3_Input } SM3_State;
int TickFct_Light_Catch(int state) {
   /*VARIABLES MUST BE DECLARED STATIC*/
/*e.g., static int x = 0;*/
static unsigned char choices[3] = {0x04, 0x08, 0x10};
static unsigned char inputs[3] = {7,6,5};
static unsigned char t = 0;
static unsigned char i = 0;
   switch(state) { // Transitions
      case -1:
         state = SM3_Init;
         break;
      case SM3_Init:
         if (gameOn == 1) {
            state = SM3_Pick;
         }
         else if (gameOn == 0) {
            state = SM3_Init;
         }
         break;
      case SM3_Pick:
         if (1) {
            state = SM3_Input;
         }
         break;
      case SM3_Input:
         if (GetBit(tempD, 7) == 1 && choices[i] == 0x04 || GetBit(tempD, 6) == 1 && choices[i] == 0x08 || GetBit(tempD, 5) == 1 && choices[i] == 0x10 ) {
            state = SM3_Pick;
         }
         else if (GetBit(tempD, 7) != 1 || GetBit(tempD, 6) != 1 || GetBit(tempD, 5) != 1) {
            state = SM3_Input;
         }
         break;
      default:
         state = -1;
      } // Transitions

   switch(state) { // State actions
      case SM3_Init:
         t = 0;
         i = 0;
         break;
      case SM3_Pick:
         i = rand() % 3;
         PORTD = choice[i];
         
         break;
      case SM3_Input:
         break;
      default: // ADD default behaviour below
         break;
   } // State actions
   SM3_State = state;
   return state;
}


enum SM4_States { SM4_Wait, SM4_M_Key, SM4_Init } SM4_State;
int TickFct_Key_Gen(int state) {
   /*VARIABLES MUST BE DECLARED STATIC*/
/*e.g., static int x = 0;*/
/*Define user variables for this state machine here. No functions; make them global.*/
static unsigned char key;
static unsigned char keys[16] = {'1','2','3','4','5','6','7','8','9','0','A','B','C','D','*','#'};

static unsigned char i;
static unsigned char sequence[5];
   switch(state) { // Transitions
      case -1:
         state = SM4_Init;
         break;
      case SM4_Wait:
         if (puzzleComplete == 1) {
            state = SM4_M_Key;
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
         			unsigned char val = USART_Receive(1);
         				puzzleComplete = 1;
         				USART_Flush(1);
         			
         		}
         break;
      case SM4_M_Key:
         key = rand() % 16;
         sequence[i] = keys[key];
         i++;
         puzzleComplete = 0;
         LCD_DisplayString(1, "Puzzle Done!");
         LCD_DisplayString(17, "Code:" + sequence[i]); 
         break;
      case SM4_Init:
         i = 0;
         break;
      default: // ADD default behaviour below
         break;
   } // State actions
   SM4_State = state;
   return state;
}


enum SM5_States { SM5_Init, SM5_Wait, SM5_Input, SM5_Inspect, SM5_Win } SM5_State;
int TickFct_Key_Input(int state) {
   /*VARIABLES MUST BE DECLARED STATIC*/
/*e.g., static int x = 0;*/
static unsigned char input[5];
static unsigned char input = 0;
static unsigned char inspect = 0;
static unsigned char numInputs;

   switch(state) { // Transitions
      case -1:
         state = SM5_Init;
         break;
      case SM5_Init:
         if (gameOn == 1) {
            state = SM5_Wait;
         }
         else if (gameOn == 0) {
            state = SM5_Init;
         }
         break;
      case SM5_Wait:
         if (input == 1 && puzzlesFinished == totalPuzzles) {
            state = SM5_Input;
            LCD_DisplayString(1, "Input Code: ");
         }
         else if (inspect == 1) {
            state = SM5_Inspect;
         }
         break;
      case SM5_Input:
         if (1) {
            state = SM5_Input;
         }
         else if (user_input == sequence && puzzlesFinished == totalPuzzles) {
            state = SM5_Win;
         }
         break;
      case SM5_Inspect:
         if (1) {
            state = SM5_Wait;
            inspect = 0;
         }
         break;
      case SM5_Win:
         break;
      default:
         state = -1;
      } // Transitions

   switch(state) { // State actions
      case SM5_Init:
         break;
      case SM5_Wait:
         x = GetKeypadKey();
         		switch (x) {
         			case '\0': break;
         			case '#': input = 1;  break;
         			case '*': inspect = 1; break;
         			default: break;
         		}
         break;
      case SM5_Input:
         //Keypad code here
         break;
      case SM5_Inspect:
         LCD_DisplayString(1, "Details Here.");
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


enum SM7_States { SM7_Init, SM7_Create_Field, SM7_Movement, SM7_Fail } SM7_State;
int TickFct_HackGame(int state) {
   /*VARIABLES MUST BE DECLARED STATIC*/
/*e.g., static int x = 0;*/
/*Define user variables for this state machine here. No functions; make them global.*/

static unsigned char playerPos = 1;
static unsigned char goal = 16;
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
         break;
      case SM7_Fail:
         if (1) {
            state = SM7_Create_Field;
         }
         break;
      default:
         state = -1;
      } // Transitions

   switch(state) { // State actions
      case SM7_Init:
         break;
      case SM7_Create_Field:
         i = rand() % 5;
         			LCD_Cursor(position_1[i]);
         			LCD_WriteData('#');
         			LCD_Cursor(position_2[i]);
         			LCD_WriteData('#');
         
         LCD_Cursor(position_3[i]);
         			LCD_WriteData('#');
         
         LCD_Cursor(goal);
         			LCD_WriteData('1');
         
         playerPos = 0;
         
         LCD_Cursor(playerPos);
         
         
         break;
      case SM7_Movement:
         //Movement Code Here
         break;
      case SM7_Fail:
         LCD_DisplayString(1, "Failed!");
         break;
      default: // ADD default behaviour below
         break;
   } // State actions
   SM7_State = state;
   return state;
}


enum SM8_States { SM8_Init, SM8_Tick } SM8_State;
int TickFct_Speakers(int state) {
   /*VARIABLES MUST BE DECLARED STATIC*/
/*e.g., static int x = 0;*/
/*Define user variables for this state machine here. No functions; make them global.*/
static unsigned short  t = 0;
   switch(state) { // Transitions
      case -1:
         state = SM8_Init;
         break;
      case SM8_Init:
         if (1) {
            state = SM8_Tick;
         }
         break;
      case SM8_Tick:
         if (1) {
            state = SM8_Tick;
            t++;
         }
         break;
      default:
         state = -1;
      } // Transitions

   switch(state) { // State actions
      case SM8_Init:
         break;
      case SM8_Tick:
         if(t % 5 == 0)
         {
              PORTC = PORTC | 0x01
         }
         else
         {
            PORTC = PORTC & FE
         }
         break;
      default: // ADD default behaviour below
         break;
   } // State actions
   SM8_State = state;
   return state;
}

