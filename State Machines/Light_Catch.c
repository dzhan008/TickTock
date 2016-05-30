/*
This code was automatically generated using the Riverside-Irvine State machine Builder tool
Version 2.7 --- 5/29/2016 18:35:24 PST
*/

#include "rims.h"

/*Define user variables and functions for this state machine here.*/
	unsigned char choices[3] = {0x01, 0x02, 0x04};
unsigned char SM1_Clk;
void TimerISR() {
   SM1_Clk = 1;
}

enum SM1_States { SM1_Init, SM1_Pick, SM1_Input } SM1_State;

TickFct_State_machine_1() {
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
         break;
      default:
         SM1_State = SM1_Init;
   } // Transitions

   switch(SM1_State) { // State actions
      case SM1_Init:
         break;
      case SM1_Pick:
         i = rand() % 3;
         PORTC = choices[i];
         break;
      case SM1_Input:
         break;
      default: // ADD default behaviour below
      break;
   } // State actions

}

int main() {

   const unsigned int periodState_machine_1 = 1000; // 1000 ms default
   TimerSet(periodState_machine_1);
   TimerOn();
   
   SM1_State = -1; // Initial state
   B = 0; // Init outputs

   while(1) {
      TickFct_State_machine_1();
      while(!SM1_Clk);
      SM1_Clk = 0;
   } // while (1)
} // Main