/*
This code was automatically generated using the Riverside-Irvine State machine Builder tool
Version 2.7 --- 5/18/2016 1:38:41 PST
*/

#include "rims.h"

/*This code will be shared between state machines.*
unsigned char t = 0;
unsigned char i = 0;
unsigned char end = 1;
unsigned char seq = 1; //Max number of lights to light
unsigned char s = 0; //Used to flash numbers = to seq
unsigned char choices[4] = {0x01, 0x02, 0x04, 0x08};/
typedef struct task {
   int state;
   unsigned long period;
   unsigned long elapsedTime;
   int (*TickFct)(int);
} task;

task tasks[2];

const unsigned char tasksNum = 2;
const unsigned long periodSimon_Says = 200;
const unsigned long periodTimer = 1000;

const unsigned long tasksPeriodGCD = 200;

int TickFct_Simon_Says(int state);
int TickFct_Timer(int state);

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
   tasks[i].period = periodSimon_Says;
   tasks[i].elapsedTime = tasks[i].period;
   tasks[i].TickFct = &TickFct_Simon_Says;

   ++i;
   tasks[i].state = -1;
   tasks[i].period = periodTimer;
   tasks[i].elapsedTime = tasks[i].period;
   tasks[i].TickFct = &TickFct_Timer;

   ++i;
   TimerSet(tasksPeriodGCD);
   TimerOn();
   
   while(1) { Sleep(); }

   return 0;
}

enum SS1_States { SS1_Init, SS1_Pick, SS1_Input, SS1_Wrong } SS1_State;
int TickFct_Simon_Says(int state) {
   /*VARIABLES MUST BE DECLARED STATIC*/
/*e.g., static int x = 0;*/
/*Define user variables for this state machine here. No functions; make them global.*/
   switch(state) { // Transitions
      case -1:
         state = SS1_Init;
         break;
      case SS1_Init:
         if (1) {
            state = SS1_Pick;
         }
         break;
      case SS1_Pick:
         if (s == seq) {
            state = SS1_Input;
         }
         else if (s != seq) {
            state = SS1_Pick;
            s += 1;
         }
         break;
      case SS1_Input:
         if (tempB != choices[i]) {
            state = SS1_Wrong;
         }
         else if (tempB = 0x00) {
            state = SS1_Input;
         }
         else if (tempB = choices[i]) {
            state = SS1_Pick;
            s = 0;
         }
         break;
      case SS1_Wrong:
         if (t < 8) {
            state = SS1_Wrong;
            t++;
         }
         else if (t == 8) {
            state = SS1_Input;
            t = 0;
PORTB = temp[i];
         }
         break;
      default:
         state = -1;
      } // Transitions

   switch(state) { // State actions
      case SS1_Init:
         t = 0;
         i = 0;
         
         break;
      case SS1_Pick:
         i = rand() % 3;
         PORTB = choices[i];
         break;
      case SS1_Input:
         break;
      case SS1_Wrong:
         if(t % 2 == 0)
         PORTB = 0xFF;
         else
         PORTB = 0x00;
         break;
      default: // ADD default behaviour below
         break;
   } // State actions
   SS1_State = state;
   return state;
}


enum SS2_States { SS2_Tick } SS2_State;
int TickFct_Timer(int state) {
   /*VARIABLES MUST BE DECLARED STATIC*/
/*e.g., static int x = 0;*/
/*Define user variables for this state machine here. No functions; make them global.*/
static unsigned char t = 150; //30 Seconds for 200 ms period
static unsigned chat shift_val = 0x3F;
   switch(state) { // Transitions
      case -1:
         t_data(shift_val);
         state = SS2_Tick;
         break;
      case SS2_Tick:
         if (t < 150) {
            state = SS2_Tick;
            t++;
         }
         break;
      default:
         state = -1;
      } // Transitions

   switch(state) { // State actions
      case SS2_Tick:
         if(t % 25 == 0)
         {
             shift_val = shift_val >> 1;
             t_data(shift_val);
         }
         if(t == 150)
         end = 1;
         break;
      default: // ADD default behaviour below
         break;
   } // State actions
   SS2_State = state;
   return state;
}

