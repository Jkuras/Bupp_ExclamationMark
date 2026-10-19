#include <Arduino.h>
#include <MIDI.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);  
const int CTRL_DATA = 7;
const int CTRL_LATCH = 6;
const int CTRL_CLOCK = 5;
const int OUTER_DATA = 4;
const int OUTER_LATCH = 3;
const int OUTER_CLOCK = 2;
const int COUT_SRCLK = 8;
const int COUT_RCLK = 9;
const int COUT_DATA = 10;
bool OUTER_PRESSED[4];
bool OUTER_LAST[4];
int pressed_order[4];
bool KEYPAD_PRESSED[10];
bool KEYPAD_LAST[10];
byte CTRL_LAST;
const int BLUE = 1;
const String B_STR = "BLU";
const int RED = 2;
const String R_STR = "RED";
const int YELLOW = 3;
const String Y_STR = "YEL";
const int GREEN = 4;
const String G_STR = "GRN";
const int CORRECT = 192;
const int WRONG = 3;
byte CTRL_STATE;
String TOP_LINE = "1st 2nd 3rd 4th ";
String BOTTOM_LINE = "RED YEL GRN BLU";

void setup()
{
  Serial.begin(31250);
  pinMode(CTRL_DATA, INPUT);
  pinMode(CTRL_CLOCK, OUTPUT);
  pinMode(CTRL_LATCH, OUTPUT);
  pinMode(OUTER_DATA, INPUT);
  pinMode(OUTER_CLOCK, OUTPUT);
  pinMode(OUTER_LATCH, OUTPUT);
  CTRL_LAST = 0;
  digitalWrite(OUTER_CLOCK, LOW);
  digitalWrite(OUTER_LATCH, HIGH);
  pressed_order[0] = 0;
  pressed_order[1] = 0;
  pressed_order[2] = 0;
  pressed_order[3] = 0;
  lcd.init();


  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print(TOP_LINE);

  lcd.setCursor(0, 1);
  lcd.print(BOTTOM_LINE);

}

void read_all_buttons(){


  digitalWrite(CTRL_LATCH, LOW);
  delayMicroseconds(5); // Tiny pause to ensure chip registers the signal
  digitalWrite(CTRL_LATCH, HIGH);


  CTRL_STATE = shiftIn(CTRL_DATA, CTRL_CLOCK, MSBFIRST);

  // TODO: Add outer button panel and keypad reading

}

void clear_order(){
  BOTTOM_LINE = "??? ??? ??? ??? ";
  lcd.setCursor(0, 1);
  lcd.print(BOTTOM_LINE);
}

void set_top(bool listening){
  // listening is for a visual indicator that the device is either listening for new inputs or in a locked state
  TOP_LINE = "1st 2nd 3rd 4th";
  if(listening){
    TOP_LINE = TOP_LINE + "*";
  }
}

void set_order(int pressed[4]){
  BOTTOM_LINE = "";
  for (int i = 0; i < 4; i++){
    if (pressed[i] == RED){
      BOTTOM_LINE = BOTTOM_LINE + R_STR + " ";
    }
    else if (pressed[i] == BLUE){
      BOTTOM_LINE = BOTTOM_LINE + B_STR + " ";
    }
    else if (pressed[i] == GREEN){
      BOTTOM_LINE = BOTTOM_LINE + G_STR + " ";
    }
    else if (pressed[i] == YELLOW){
      BOTTOM_LINE = BOTTOM_LINE + Y_STR + " ";
    } else {
      BOTTOM_LINE = BOTTOM_LINE + "??? ";
    }
  }
  lcd.setCursor(0, 1);
  lcd.print(BOTTOM_LINE);

}


void loop()
{
  
  read_all_buttons();
  if(CTRL_STATE != CTRL_LAST){
    
    if(CTRL_STATE == 0){
      if(CTRL_LAST == 192){

        delay(300);
      }
      if(CTRL_LAST == 3){

        delay(300);
      }
      
    }

    // TODO: breakout midi play sample functions
    if(CTRL_STATE == 192){
      // "Correct" button pressed
      Serial.write(0x90);
      Serial.write(0x1E);
      Serial.write(0x45);
      delay(300);
      clear_order();
    }
    if(CTRL_STATE == 3){
      // "Wrong" button pressed
      Serial.write(0x91);
      Serial.write(0x1E);
      Serial.write(0x45);

      delay(300);
      int order[4] = {1, 3, 2, 4};
      set_order(order);
    }
  }
  // TODO: Handle all other button presses

  
  CTRL_LAST = CTRL_STATE;
}