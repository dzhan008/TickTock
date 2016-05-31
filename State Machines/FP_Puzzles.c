/*
This code was automatically generated using the Riverside-Irvine State machine Builder tool
Version 2.7 --- 5/31/2016 2:0:38 PST
*/

#include "rims.h"

/*This code will be shared between state machines.*/
unsigned char TimerFlag = 0;
void TimerISR() {
   TimerFlag = 1;
}


enum SM1_States { SM1_Init, SM1_Randomize, SM1_Flash, SM1_Wrong, SM1_Select } SM1_State;

TickFct_Simon_Says() {
   /*VARIABLES MUST BE DECLARED STATIC*/
/*e.g., static int x = 0;*/
/*Define user variables for this state machine here. No functions; make them global.*/
unsigned char ss_sequences[4] = {0x01, 0x02, 0x20, 0x40};
unsigned char buffer[4];
unsigned char i_index = 0;
unsigned char timer = 10;
unsigned char b_index = 0;
unsigned char req_correct = 4;
unsigned char curr_length = 0;
unsigned char length = 1;
   switch(SM1_State) { // Transitions
      case -1:
         SM1_State = SM1_Init;
         break;
      case SM1_Init:
         if (1) {
            SM1_State = SM1_Randomize;
         }
         break;
      case SM1_Randomize:
         if (1) {
            SM1_State = SM1_Flash;
         }
         break;
      case SM1_Flash:
         if (curr_length < length) {
            SM1_State = SM1_Flash;
         }
         else if (curr_length > length) {
            SM1_State = SM1_Select;
         }
         break;
      case SM1_Wrong:
         break;
      case SM1_Select:
         break;
      default:
         SM1_State = SM1_Init;
      } // Transitions

   switch(SM1_State) { // State actions
      case SM1_Init:
         curr_length = 0;
         length = 1;
         break;
      case SM1_Randomize:
         for(int i = 0; i < 4; i++)
         {
            int in = rand % 3;
            buffer[i] = ss_sequences[in];
         }
         break;
      case SM1_Flash:
         PORTC = buffer[curr_length];
         break;
      case SM1_Wrong:
         break;
      case SM1_Select:
         break;
      default: // ADD default behaviour below
         break;
   } // State actions
}

enum SM2_States { SM2_s1 } SM2_State;

TickFct_Dummy() {
   /*VARIABLES MUST BE DECLARED STATIC*/
/*e.g., static int x = 0;*/
/*Define user variables for this state machine here. No functions; make them global.*/
   switch(SM2_State) { // Transitions
      case -1:
         SM2_State = SM2_s1;
         break;
      case SM2_s1:
         break;
      default:
         SM2_State = SM2_s1;
      } // Transitions

   switch(SM2_State) { // State actions
      case SM2_s1:
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
         if (wirePulled = 0;) {
            SM4_State = SM4_Check;
         }
         else if (wirePulled == 1) {
            SM4_State = SM4_Complete;
            puzzleCompleted = 1;
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
      case SM4_Check:
         break;
      case SM4_Complete:
         break;
      default: // ADD default behaviour below
         break;
   } // State actions
}
int main() {
   B = 0; //Init outputs
   TimerSet(1000);
   TimerOn();
   SM1_State = -1;
   SM2_State = -1;
   SM3_State = -1;
   SM4_State = -1;
   while(1) {
      TickFct_Simon_Says();
      TickFct_Dummy();
      TickFct_GameController();
      TickFct_Wire_Pull();
      while (!TimerFlag);
      TimerFlag = 0;
   }
}