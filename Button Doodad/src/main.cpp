#include "Arduino.h"
#include "Wire.h"
#include "LiquidCrystal_I2C.h"

#define PANEL_IN_SHIFT 3
#define PANEL_IN_CLOCK 2
#define PANEL_IN_DATA 4

#define PANEL_OUT_LATCH 6
#define PANEL_OUT_CLOCK 5
#define PANEL_OUT_DATA 7

#define LG_IN_SHIFT 12
#define LG_IN_CLOCK 11
#define LG_IN_DATA 13

#define LG_OUT_LATCH 9
#define LG_OUT_CLOCK 8
#define LG_OUT_DATA 10

#define PIND_BTN_B_L 0
#define PIND_BTN_B_C 1
#define PIND_BTN_B_R 2
#define PIND_BTN_T_B 3
#define PIND_BTN_T_R 4
#define PIND_BTN_T_Y 5
#define PIND_BTN_T_G 6
#define PIND_BTN_T_W 7

#define LIND_BTN_MAIN 2
#define LIND_BTN_WHITE 3
#define LIND_BTN_GREEN 4
#define LIND_BTN_YELLOW 5
#define LIND_BTN_RED 6
#define LIND_BTN_BLUE 7



#define PANEL_DELAY 0b10000000
#define PANEL_BLUE 0b01000000
#define PANEL_RED 0b00100000
#define PANEL_YELLOW 0b00010000
#define PANEL_GREEN 0b00001000
#define PANEL_WHITE 0b00000100

#define LG_BTN_MAIN   0b00100000
#define LG_BTN_WHITE  0b00010000
#define LG_BTN_GREEN  0b00001000
#define LG_BTN_YELLOW 0b00000100
#define LG_BTN_RED    0b00000010
#define LG_BTN_BLUE   0b00000001

#define LG_LIGHT_MAIN   0b00000100
#define LG_LIGHT_WHITE  0b00001000
#define LG_LIGHT_GREEN  0b00010000
#define LG_LIGHT_YELLOW 0b00100000
#define LG_LIGHT_RED    0b01000000
#define LG_LIGHT_BLUE   0b10000000

#define LIGHT_ALL 0b11111111
#define LIGHT_NONE 0b00000000

#define SAMPLE_ONE 0x90
#define SAMPLE_TWO 0x91
#define SAMPLE_THREE 0x92
#define SAMPLE_FOUR 0x93
#define SAMPLE_FIVE 0x94
#define SAMPLE_SIX 0x95
#define SAMPLE_SEVEN 0x96
#define SAMPLE_EIGHT 0x97
#define SAMPLE_NINE 0x98
#define SAMPLE_TEN 0x99

boolean PANEL_BTN_ARRAY[8];
boolean PANEL_BTN_LAST[8];

boolean LG_BTN_ARRAY[8];
boolean LG_BTN_LAST[8];

int GAME_MODE;
int Q;
int C;
int W;

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup()
{
  Wire.begin();
  Serial.begin(31250);
  PANEL_BTN_ARRAY[0] = 0;
  PANEL_BTN_ARRAY[1] = 0;
  PANEL_BTN_ARRAY[2] = 0;
  PANEL_BTN_ARRAY[3] = 0;
  PANEL_BTN_ARRAY[4] = 0;
  PANEL_BTN_ARRAY[5] = 0;
  PANEL_BTN_ARRAY[6] = 0;
  PANEL_BTN_ARRAY[7] = 0;

  PANEL_BTN_LAST[0] = 0;
  PANEL_BTN_LAST[1] = 0;
  PANEL_BTN_LAST[2] = 0;
  PANEL_BTN_LAST[3] = 0;
  PANEL_BTN_LAST[4] = 0;
  PANEL_BTN_LAST[5] = 0;
  PANEL_BTN_LAST[6] = 0;
  PANEL_BTN_LAST[7] = 0;

  LG_BTN_ARRAY[0] = 0;
  LG_BTN_ARRAY[1] = 0;
  LG_BTN_ARRAY[2] = 0;
  LG_BTN_ARRAY[3] = 0;
  LG_BTN_ARRAY[4] = 0;
  LG_BTN_ARRAY[5] = 0;
  LG_BTN_ARRAY[6] = 0;
  LG_BTN_ARRAY[7] = 0;

  LG_BTN_LAST[0] = 0;
  LG_BTN_LAST[1] = 0;
  LG_BTN_LAST[2] = 0;
  LG_BTN_LAST[3] = 0;
  LG_BTN_LAST[4] = 0;
  LG_BTN_LAST[5] = 0;
  LG_BTN_LAST[6] = 0;
  LG_BTN_LAST[7] = 0;

  pinMode(PANEL_IN_SHIFT, OUTPUT);
  pinMode(PANEL_IN_CLOCK, OUTPUT);
  pinMode(PANEL_IN_DATA, INPUT);

  pinMode(PANEL_OUT_LATCH, OUTPUT);
  pinMode(PANEL_OUT_CLOCK, OUTPUT);
  pinMode(PANEL_OUT_DATA, OUTPUT);

  pinMode(LG_IN_SHIFT, OUTPUT);
  pinMode(LG_IN_CLOCK, OUTPUT);
  pinMode(LG_IN_DATA, INPUT);

  pinMode(LG_OUT_LATCH, OUTPUT);
  pinMode(LG_OUT_CLOCK, OUTPUT);
  pinMode(LG_OUT_DATA, OUTPUT);
  lcd.init();  
  lcd.clear();                    
  lcd.backlight();   

  lcd.setCursor(0, 0);
  lcd.print("SAMPLE MODE");
  
  GAME_MODE = 0;
  Q = 0;
  C = 0;
  W = 0;
  
}

// HELPER FUNCTION TO ADJUST LIGHTS
void change_lights(byte panel_lights, byte lg_lights){
  digitalWrite(PANEL_OUT_LATCH, LOW);
  shiftOut(PANEL_OUT_DATA, PANEL_OUT_CLOCK, LSBFIRST, panel_lights);
  digitalWrite(PANEL_OUT_LATCH, HIGH);

  digitalWrite(LG_OUT_LATCH, LOW);
  shiftOut(LG_OUT_DATA, LG_OUT_CLOCK, LSBFIRST, lg_lights);
  digitalWrite(LG_OUT_LATCH, HIGH);
}

// HELPER FUNCTION TO READ ALL BUTTONS
void read_buttons(){
  
    // READ ALL BUTTONS
  digitalWrite(PANEL_IN_SHIFT, LOW);
  digitalWrite(PANEL_IN_SHIFT, HIGH);
  for (int i = 0; i < 8; i++) {
    int bit = digitalRead(PANEL_IN_DATA);
    PANEL_BTN_ARRAY[i] = bit;
    digitalWrite(PANEL_IN_CLOCK, HIGH); // Shift out the next bit
    digitalWrite(PANEL_IN_CLOCK, LOW);
  }

    // READ ALL BUTTONS
  digitalWrite(LG_IN_SHIFT, LOW);
  digitalWrite(LG_IN_SHIFT, HIGH);
  for (int i = 0; i < 8; i++) {
    int bit = digitalRead(LG_IN_DATA);
    LG_BTN_ARRAY[i] = bit;

    digitalWrite(LG_IN_CLOCK, HIGH); // Shift out the next bit
    digitalWrite(LG_IN_CLOCK, LOW);
  }

}

// HELPER FUNCTION TO PLAY SAMPLES
void play_sample(int sample){
  Serial.write(sample);
  Serial.write(0x1E);
  Serial.write(0x45);
}

void loop()

{
  change_lights(LIGHT_NONE, LIGHT_ALL);
  read_buttons();
  
  if(!LG_BTN_LAST[LIND_BTN_MAIN] && LG_BTN_ARRAY[LIND_BTN_MAIN]){
      play_sample(SAMPLE_ONE);
  }
  if(!PANEL_BTN_LAST[PIND_BTN_B_L] && PANEL_BTN_ARRAY[PIND_BTN_B_L]){
      play_sample(SAMPLE_TWO);
  }
  if(!PANEL_BTN_LAST[PIND_BTN_B_C] && PANEL_BTN_ARRAY[PIND_BTN_B_C]){
    play_sample(SAMPLE_THREE);
  }
  if(!PANEL_BTN_LAST[PIND_BTN_B_R] && PANEL_BTN_ARRAY[PIND_BTN_B_R]){
    play_sample(SAMPLE_FOUR);
  }

  if(!PANEL_BTN_LAST[PIND_BTN_T_B] && PANEL_BTN_ARRAY[PIND_BTN_T_B]){
    play_sample(SAMPLE_FIVE);
  }

  if(!PANEL_BTN_LAST[PIND_BTN_T_R] && PANEL_BTN_ARRAY[PIND_BTN_T_R]){
    play_sample(SAMPLE_SIX);
  }

  if(!PANEL_BTN_LAST[PIND_BTN_T_Y] && PANEL_BTN_ARRAY[PIND_BTN_T_Y]){
    play_sample(SAMPLE_SEVEN);
  }

  if(!PANEL_BTN_LAST[PIND_BTN_T_G] && PANEL_BTN_ARRAY[PIND_BTN_T_G]){
    play_sample(SAMPLE_EIGHT);
  }

  if(!PANEL_BTN_LAST[PIND_BTN_T_W] && PANEL_BTN_ARRAY[PIND_BTN_T_W]){
    play_sample(SAMPLE_NINE);
  }


  
  
  if(!LG_BTN_LAST[LIND_BTN_BLUE] && LG_BTN_ARRAY[LIND_BTN_BLUE]){
    play_sample(SAMPLE_SIX);
  }

  if(!LG_BTN_LAST[LIND_BTN_RED] && LG_BTN_ARRAY[LIND_BTN_RED]){
    play_sample(SAMPLE_SEVEN);
  }

  if(!LG_BTN_LAST[LIND_BTN_YELLOW] && LG_BTN_ARRAY[LIND_BTN_YELLOW]){
    play_sample(SAMPLE_EIGHT);
  }

  if(!LG_BTN_LAST[LIND_BTN_GREEN] && LG_BTN_ARRAY[LIND_BTN_GREEN]){
    play_sample(SAMPLE_NINE);
  }

  if(!LG_BTN_LAST[LIND_BTN_WHITE] && LG_BTN_ARRAY[LIND_BTN_WHITE]){
    play_sample(SAMPLE_TEN);
  }

  // // MAIN MENU, CHECK IF GAME SELECTION
  // if(GAME_MODE == 0){
  //   // DOUBLE CHECK ALL LIGHTS ON, AND SET SCREEN
  //   change_lights(LIGHT_NONE, LIGHT_ALL);
  //   lcd.setCursor(0, 0);
  //   lcd.print("WAITING FOR GAME SELECTION");
  //   // TRIVIA SELECTED
    

  //   // TRIVIA SELECTED
  //   if(!LG_BTN_LAST[LIND_BTN_BLUE] && LG_BTN_ARRAY[LIND_BTN_BLUE]){
  //     GAME_MODE = 1;

  //     // DISPLAY SELECTION
  //     change_lights(PANEL_BLUE | PANEL_DELAY, LG_LIGHT_BLUE);
  //     lcd.clear();
  //     lcd.setCursor(0, 0);
  //     lcd.print("TRIVIA");
  //     lcd.setCursor(0, 1);
  //     lcd.print("Q:0 C:0/5 W:0/3");

  //     // SELECTION SAMPLE
  //     play_sample(SAMPLE_ONE);

  //     // 5 SEC INPUT DELAY AND LIGHTS OFF
  //     delay(5000);
  //     change_lights(LIGHT_NONE, LG_LIGHT_BLUE);
  //   }

  //   // LIMERICKS SELECTED
  //   if(!LG_BTN_LAST[LIND_BTN_RED] && LG_BTN_ARRAY[LIND_BTN_RED]){
  //     GAME_MODE = 2;

  //     // DISPLAY SELECTION
  //     change_lights(PANEL_RED | PANEL_DELAY, LG_LIGHT_RED);
  //     lcd.clear();
  //     lcd.setCursor(0, 0);
  //     lcd.print("LIMERICKS");
  //     lcd.setCursor(0, 1);
  //     lcd.print("L:0 C:0/5 W:0/3");

  //     // SELECTION SAMPLE
  //     play_sample(SAMPLE_ONE);

  //     // 5 SEC INPUT DELAY AND LIGHTS OFF
  //     delay(5000);
  //     change_lights(LIGHT_NONE, LG_LIGHT_RED);
  //   }

  //   // TRUTH?! SELECTED
  //   if(!LG_BTN_LAST[LIND_BTN_YELLOW] && LG_BTN_ARRAY[LIND_BTN_YELLOW]){
  //     GAME_MODE = 3;

  //     // DISPLAY SELECTION
  //     change_lights(PANEL_YELLOW | PANEL_DELAY, LG_LIGHT_YELLOW);
  //     lcd.clear();
  //     lcd.setCursor(0, 0);
  //     lcd.print("TRUTH?!");
  //     lcd.setCursor(0, 1);
  //     lcd.print("I:N C:0/5 W:0/5");

  //     // SELECTION SAMPLE
  //     play_sample(SAMPLE_ONE);

  //     // 5 SEC INPUT DELAY AND SET GAME LIGHTS
  //     delay(5000);
  //     change_lights(LIGHT_NONE, LG_LIGHT_YELLOW);
  //   }

  //   // WHO AM I SELECTED
  //   if(!LG_BTN_LAST[LIND_BTN_GREEN] && LG_BTN_ARRAY[LIND_BTN_GREEN]){
  //     GAME_MODE = 4;

  //     // DISPLAY SELECTION
  //     change_lights(PANEL_GREEN | PANEL_DELAY, LG_LIGHT_GREEN);
  //     lcd.clear();
  //     lcd.setCursor(0, 0);
  //     lcd.print("WHO AM I?");
  //     lcd.setCursor(0, 1);
  //     lcd.print("NOBODY KNOWS");

  //     // SELECTION SAMPLE
  //     play_sample(SAMPLE_ONE);

  //     // 5 SEC INPUT DELAY AND SET GAME LIGHTS
  //     delay(5000);
  //     change_lights(LIGHT_NONE, LG_LIGHT_GREEN);
  //   }

  //   // NO U PLAY SELECTED
  //   if(!LG_BTN_LAST[LIND_BTN_WHITE] && LG_BTN_ARRAY[LIND_BTN_WHITE]){
  //     play_sample(SAMPLE_EIGHT);
  //   }
  // }

  // // TRIVIA RULESET
  // if(GAME_MODE == 1){

  //   // TRIVIA ANSWER WRONG
  //   if(!PANEL_BTN_LAST[PIND_BTN_T_R] && PANEL_BTN_ARRAY[PIND_BTN_T_R]){
  //     // INCREMENT COUNTERS
  //     Q = Q+1;
  //     W = W+1;

  //     // PRINT RESULTS
  //     lcd.setCursor(2, 1);
  //     lcd.print(Q);
  //     lcd.setCursor(12, 1);
  //     lcd.print(W);

      

  //     // TRIVIA LOSS
  //     if(W == 3){
  //       change_lights(PANEL_RED, LG_LIGHT_RED);
  //       lcd.setCursor(6, 0);
  //       lcd.print("-LOSER!");

  //       // LOSS SAMPLE
  //       play_sample(SAMPLE_TWO);

  //       // 5 SEC INPUT DELAY
  //       delay(5000);

  //       // RESET COUNTERS
  //       Q = 0;
  //       W = 0;
  //       C = 0;
  //       GAME_MODE = 0;
  //       lcd.clear();
  //     } else {
  //       // WRONG ANSWER SAMPLE (IN ELSE SO IT DOES NOT PLAY ON A FULL LOSS)
  //       play_sample(SAMPLE_THREE);
  //     }
  //   }
  //       // TRIVIA ANSWER WRONG
  //   if(!PANEL_BTN_LAST[PIND_BTN_T_B] && PANEL_BTN_ARRAY[PIND_BTN_T_B]){
  //     // INCREMENT COUNTERS
  //     W = W-1;

  //     // PRINT RESULTS
  //     lcd.setCursor(2, 1);
  //     lcd.print(Q);
  //     lcd.setCursor(12, 1);
  //     lcd.print(W);
  //     // WRONG ANSWER SAMPLE (IN ELSE SO IT DOES NOT PLAY ON A FULL LOSS)
  //     play_sample(SAMPLE_THREE);
  //   }

  //   // TRIVIA ANSWER RIGHT
  //   if(!PANEL_BTN_LAST[PIND_BTN_T_W] && PANEL_BTN_ARRAY[PIND_BTN_T_W]){
  //     // INCREMENT COUNTERS
  //     Q = Q+1;
  //     C = C+1;

  //     // PRINT RESULTS
  //     lcd.setCursor(2, 1);
  //     lcd.print(Q);
  //     lcd.setCursor(6, 1);
  //     lcd.print(C);
      

  //     // TRIVIA WIN
  //     if(C == 5){
  //       change_lights(PANEL_GREEN, LG_LIGHT_GREEN);
  //       lcd.setCursor(6, 0);
  //       lcd.print("-WINNER!");

  //       // WIN SAMPLE
  //       play_sample(SAMPLE_FOUR);

  //       // 5 SEC INPUT DELAY
  //       delay(5000);

  //       // RESET COUNTERS
  //       Q = 0;
  //       W = 0;
  //       C = 0;
  //       GAME_MODE = 0;
  //       lcd.clear();
  //     } else {
  //       // RIGHT ANSWER SAMPLE (IN ELSE SO IT DOES NOT PLAY ON A WIN)
  //       play_sample(SAMPLE_FIVE);
  //     }
  //   }

  //       // TRIVIA ANSWER RIGHT
  //   if(!PANEL_BTN_LAST[PIND_BTN_T_G] && PANEL_BTN_ARRAY[PIND_BTN_T_G]){
  //     // INCREMENT COUNTERS
  //     C = C-1;

  //     // PRINT RESULTS
  //     lcd.setCursor(2, 1);
  //     lcd.print(Q);
  //     lcd.setCursor(6, 1);
  //     lcd.print(C);
      

  //     play_sample(SAMPLE_FIVE);
      
  //   }

  //   // RESET TO MENU
  //   if(!PANEL_BTN_LAST[PIND_BTN_T_Y] && PANEL_BTN_ARRAY[PIND_BTN_T_Y]){
  //     // RESET SAMPLE
  //     play_sample(SAMPLE_EIGHT);
  //     GAME_MODE = 0;
  //     lcd.clear();
  //   }
  // }

  // // LIMERICKS
  // if(GAME_MODE == 2){

  //   // LIMERICK WRONG
  //   if(!PANEL_BTN_LAST[PIND_BTN_T_R] && PANEL_BTN_ARRAY[PIND_BTN_T_R]){
  //     Q = Q+1;
  //     W = W+1;
  //     lcd.setCursor(2, 1);
  //     lcd.print(Q);
  //     lcd.setCursor(12, 1);
  //     lcd.print(W);

  //     // LIMERICK LOSS
  //     if(W == 3){
  //       change_lights(PANEL_RED, LG_LIGHT_RED);
  //       lcd.setCursor(9, 0);
  //       lcd.print("-LOSER!");

  //       // LOSS SAMPLE
  //       play_sample(SAMPLE_TWO);

  //       // 5 SEC INPUT DELAY
  //       delay(5000);

  //       // RESET COUNTERS
  //       Q = 0;
  //       W = 0;
  //       C = 0;
  //       GAME_MODE = 0;
  //       lcd.clear();
  //     } else {
  //       // WRONG ANSWER SAMPLE (IN ELSE SO IT DOES NOT PLAY ON A FULL LOSS)
  //       play_sample(SAMPLE_THREE);
  //     }
  //   }

  //       // LIMERICK WRONG
  //   if(!PANEL_BTN_LAST[PIND_BTN_T_B] && PANEL_BTN_ARRAY[PIND_BTN_T_B]){
  //     W = W-1;
  //     lcd.setCursor(2, 1);
  //     lcd.print(Q);
  //     lcd.setCursor(12, 1);
  //     lcd.print(W);

  //     play_sample(SAMPLE_THREE);
      
  //   }

  //   // LIMERICK CORRECT
  //   if(!PANEL_BTN_LAST[PIND_BTN_T_W] && PANEL_BTN_ARRAY[PIND_BTN_T_W]){
  //     Q = Q+1;
  //     C = C+1;
  //     lcd.setCursor(2, 1);
  //     lcd.print(Q);
  //     lcd.setCursor(6, 1);
  //     lcd.print(C);

  //     // LIMERICK WIN
  //     if(C == 5){
  //       change_lights(PANEL_GREEN, LG_LIGHT_GREEN);
  //       lcd.setCursor(9, 0);
  //       lcd.print("-WINNER!");

  //       // WIN SAMPLE
  //       play_sample(SAMPLE_FOUR);

  //       delay(5000);
  //       Q = 0;
  //       W = 0;
  //       C = 0;
  //       GAME_MODE = 0;
  //       lcd.clear();
  //     } else {
  //       // RIGHT ANSWER SAMPLE (IN ELSE SO IT DOES NOT PLAY ON A WIN)
  //       play_sample(SAMPLE_FIVE);
  //     }
  //   }

  //       // LIMERICK CORRECT
  //   if(!PANEL_BTN_LAST[PIND_BTN_T_G] && PANEL_BTN_ARRAY[PIND_BTN_T_G]){
  //     C = C-1;
  //     lcd.setCursor(2, 1);
  //     lcd.print(Q);
  //     lcd.setCursor(6, 1);
  //     lcd.print(C);

  //       play_sample(SAMPLE_FIVE);
      
  //   }

  //   // RESET TO MENU
  //   if(!PANEL_BTN_LAST[PIND_BTN_T_Y] && PANEL_BTN_ARRAY[PIND_BTN_T_Y]){
  //     // RESET SAMPLE
  //     play_sample(SAMPLE_EIGHT);
  //     GAME_MODE = 0;
  //     lcd.clear();
  //   }
  // }

  // // TRUTH?!
  // if(GAME_MODE == 3){
    
  //   // ANSWER SUBMIT TRUE
  //   if(!LG_BTN_LAST[LIND_BTN_RED] && LG_BTN_ARRAY[LIND_BTN_RED]){
  //     change_lights(PANEL_RED, LG_LIGHT_RED);
  //     lcd.setCursor(2, 1);
  //     lcd.print("T");
  //     // SUBMISSION TRUE SAMPLE
  //     play_sample(SAMPLE_SIX);
  //   }

  //   // ANSWER WRONG
  //   if(!PANEL_BTN_LAST[PIND_BTN_T_R] && PANEL_BTN_ARRAY[PIND_BTN_T_R]){
  //     W = W+1;
  //     lcd.setCursor(12, 1);
  //     lcd.print(W);
  //     change_lights(LIGHT_NONE, LG_LIGHT_RED);
  //     lcd.setCursor(2, 1);
  //     lcd.print("N");

  //     // TRUTH?! LOSS
  //     if(W == 5){
  //       change_lights(PANEL_RED, LG_LIGHT_RED);
  //       lcd.setCursor(7, 0);
  //       lcd.print("-LOSER!");
  //       // LOSS SAMPLE
  //       play_sample(SAMPLE_TWO);

  //       // 5 SEC INPUT DELAY
  //       delay(5000);

  //       // RESET COUNTERS
  //       W = 0;
  //       C = 0;
  //       GAME_MODE = 0;
  //       lcd.clear();
  //     } else {
  //       // WRONG ANSWER SAMPLE (IN ELSE SO IT DOES NOT PLAY ON A FULL LOSS)
  //       play_sample(SAMPLE_THREE);
  //     }
  //   }

  //       // ANSWER WRONG
  //   if(!PANEL_BTN_LAST[PIND_BTN_T_B] && PANEL_BTN_ARRAY[PIND_BTN_T_B]){
  //     W = W-1;
  //     lcd.setCursor(12, 1);
  //     lcd.print(W);
  //     change_lights(LIGHT_NONE, LG_LIGHT_RED);
  //     lcd.setCursor(2, 1);
  //     lcd.print("N");

  //     play_sample(SAMPLE_THREE);
      
  //   }

  //   // ANSWER RIGHT
  //   if(!PANEL_BTN_LAST[PIND_BTN_T_W] && PANEL_BTN_ARRAY[PIND_BTN_T_W]){
  //     C = C+1;
  //     lcd.setCursor(6, 1);
  //     lcd.print(C);
  //     change_lights(PANEL_GREEN, LG_LIGHT_GREEN);
  //     lcd.setCursor(2, 1);
  //     lcd.print("N");

  //     // TRUTH?! WIN
  //     if(C == 5){
  //       change_lights(PANEL_GREEN, LG_LIGHT_GREEN);
  //       lcd.setCursor(7, 0);
  //       lcd.print("-WINNER!");

  //       // WIN SAMPLE
  //       play_sample(SAMPLE_FOUR);

  //       W = 0;
  //       C = 0;
  //       GAME_MODE = 0;
  //       lcd.clear();
  //     } else {
  //       // RIGHT ANSWER SAMPLE (IN ELSE SO IT DOES NOT PLAY ON A WIN)
  //       play_sample(SAMPLE_FIVE);
  //       change_lights(LIGHT_NONE, LG_LIGHT_RED);
  //     }
  //   }

  //       // ANSWER RIGHT
  //   if(!PANEL_BTN_LAST[PIND_BTN_T_G] && PANEL_BTN_ARRAY[PIND_BTN_T_G]){
  //     C = C-1;
  //     lcd.setCursor(6, 1);
  //     lcd.print(C);
  //     lcd.setCursor(2, 1);
  //     lcd.print("N");

  //     play_sample(SAMPLE_FIVE);

  //   }

  //   // RESET TO MENU
  //   if(!PANEL_BTN_LAST[PIND_BTN_T_Y] && PANEL_BTN_ARRAY[PIND_BTN_T_Y]){
  //     // RESET SAMPLE
  //     play_sample(SAMPLE_EIGHT);
  //     GAME_MODE = 0;
  //     lcd.clear();
  //   }
  // }

  // // WHO AM I?
  // if(GAME_MODE == 4){

  //   // GUESS READY
  //   if(!LG_BTN_LAST[LIND_BTN_GREEN] && LG_BTN_ARRAY[LIND_BTN_GREEN]){
  //     change_lights(PANEL_GREEN, LG_LIGHT_GREEN);
  //     lcd.setCursor(0, 1);
  //     lcd.print("SOMEBODY KNOWS");

  //     // SUBMISSION TRUE SAMPLE
  //     play_sample(SAMPLE_SIX);

  //   }

  //   // GUESS WRONG
  //   if(!PANEL_BTN_LAST[PIND_BTN_T_R] && PANEL_BTN_ARRAY[PIND_BTN_T_R]){
  //     change_lights(PANEL_RED, LG_LIGHT_RED);
  //     lcd.setCursor(9, 0);
  //     lcd.print("-NO!");
  //     lcd.print("NOBODY KNOWS  ");

  //     // WRONG ANSWER SAMPLE
  //     play_sample(SAMPLE_THREE);
  //     delay(5000);
  //     change_lights(LIGHT_NONE, LG_LIGHT_GREEN);
  //   }

  //   // GUESS RIGHT AND WIN
  //   if(!PANEL_BTN_LAST[PIND_BTN_T_W] && PANEL_BTN_ARRAY[PIND_BTN_T_W]){
  //     change_lights(PANEL_GREEN, LG_LIGHT_GREEN);
  //     lcd.setCursor(10, 0);
  //     lcd.print("-YES!");
      
  //     // WIN SAMPLE
  //     play_sample(SAMPLE_FOUR);

  //     delay(5000);
  //     GAME_MODE = 0;
  //     lcd.clear();
  //     change_lights(LIGHT_NONE, LIGHT_ALL);
  //   }

  //   // RESET TO MENU
  //   if(!PANEL_BTN_LAST[PIND_BTN_T_Y] && PANEL_BTN_ARRAY[PIND_BTN_T_Y]){
  //     // RESET SAMPLE
  //     play_sample(SAMPLE_EIGHT);
  //     GAME_MODE = 0;
  //     lcd.clear();
  //   }
  // }

  
  // SAVE LAST BUTTON STATE FOR COMP
  memcpy(PANEL_BTN_LAST, PANEL_BTN_ARRAY, 8);
  memcpy(LG_BTN_LAST, LG_BTN_ARRAY, 8);
}

