#include <TM1637.h>


/*
   TM1637.cpp
   A library for the 4 digit display

   Copyright (c) 2012 seeed technology inc.
   Website    : www.seeed.cc
   Author     : Frankie.Chu
   Create Time: 9 April,2012
   Change Log :

   The MIT License (MIT)

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
   THE SOFTWARE.
*/
#include "TM1637.h"
#define CLK 9 // D4
#define DIO 10

#define BUTTON_PIN 5
#define RAS_PIN A0


static unsigned char display_state[] = {0, 0, 0, 0};
static unsigned long start_time, end_time;
static unsigned int button;
static enum State {INIT = 0, CONFIGURE_MINUTES = 1, CONFIGURE_SECONDS = 2, RUNNING = 3, STOPPED = 4, COMPLETED = 5, RESET = 6} state;
static unsigned int btn_state;
TM1637 tm1637(CLK, DIO);

// Variables will change:
int buttonState = 0;         // current state of the button
int lastButtonState = 0;     // previous state of the button

void setup()
{
  btn_state = 0;
  end_time = 1000;
  start_time = 0;
  pinMode(BUTTON_PIN, INPUT);
//  time = millis();
  tm1637.init();
  tm1637.point(true);
  tm1637.set(BRIGHTEST);//BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7;  
  Serial.begin(115200);
}
void loop()
{
  unsigned long t = btn_pressed();
  switch (state) {
    case INIT:
      {
        blink(250);
        if(t < 500){
          state = CONFIGURE_MINUTES;
        }
//        blink(250);
//        if(btn_pressed()) {
//          state = CONFIGURE_MINUTES;
//        }

        break;
      }
    case CONFIGURE_MINUTES:
    {
      blink(50);
      break;  
    }
  }
  delay(1);
}

unsigned long btn_pressed() {
  int btn = digitalRead(BUTTON_PIN);
  if (btn_state == 0 && btn) {
    btn_state = 1; start_time = millis(); 
  } else if (btn_state == 1 && !btn) {
    btn_state = 0; end_time = millis();
  }
  Serial.println(end_time - start_time);
  return end_time - start_time;
}

//  int button = digitalRead(BUTTON_PIN);
//  blink(250);
//  if(state == INIT && button) {
//    while(digitalRead(BUTTON_PIN)) {}
//    state = CONFIGURE_MINUTES;
//  } else if (state == CONFIGURE_MINUTES && !button) {
//    while(!digitalRead(BUTTON_PIN)) {
//      int value = map(analogRead(RAS_PIN), 0, 930, 0, 10);
//      set_minutes(value);
//      blink_minutes(50);
//    }
//    state = CONFIGURE_MINUTES_TO_SECONDS;
//  } else if (state == CONFIGURE_MINUTES && button) {
//    state = CONFIGURE_MINUTES_TO_SECONDS;
//  } 
   
//  switch (state) {
//    case (INIT && prev_state):
//      if (button) {
//        await_btn_low();
//        state = CONFIGURE_MINUTES;
//      }
//      break;
//    case CONFIGURE_MINUTES:
//    {
//      blink(100);
//      if (button) {
//        time = millis();
//      } else {
//        int value = map(analogRead(RAS), 0, 935, 0, 10);
//        set_minutes(value);
//      }
//    }
//    break;
//    case CONFIGURE_SECONDS:
//    {
//      blink(100);
//      if (button) {
//        long delta = await_btn_low();
//        if (delta <= 300) {
//          state = CONFIGURE_MINUTES;
//        } else {
//          state = RUNNING;
//        }
//      } else {
//        int value = map(analogRead(RAS), 0, 935, 0, 59);
//        set_seconds(value);
//      }
//    }
//    break;
//    case RUNNING:
//    {
//      int seconds_till_end = remaining_seconds();
//      if (button) {
//        await_btn_low();
//      }
//      while (seconds_till_end > 0) {
//        if(digitalRead(BUTTON)) {
//          await_btn_low();
//          state = STOPPED;
//          break;
//        }
//        seconds_till_end -= 1;
//        update_display(seconds_till_end);
//        delay(1000);
//      }
//      state = COMPLETED;
//    }
//    break;
//    case STOPPED:
//    {
//      blink(100);
//      if (button) {
//        long delta = await_btn_low();
//        if (delta <= 150) {
//          state = RUNNING;
//        } else {
//          state = INIT;
//        }
//      }
//    }
//    break;
//    case COMPLETED:
//    {
//      Serial.println("COMPLETED");
//    } 
//    break;
//  }

//long await_btn_low() {
//  while (digitalRead(BUTTON_PIN)) { };
//  unsigned long endtime = millis();
//  return endtime - time;
//}

int remaining_seconds() {
  return (display_state[0] * 10 + display_state[1]) * 60 + display_state[2] * 10 + display_state[3];
}


void set_minutes(int value) {
  display_state[0] = value / 10;
  display_state[1] = value % 10;
}

void set_seconds(int value) {
  display_state[2] = value / 10;
  display_state[3] = value % 10;
}

void update_display(int remaining_seconds) {
  set_minutes(remaining_seconds / 60);
  set_seconds(remaining_seconds % 60);
  tm1637.display(0, display_state[0]);
  tm1637.display(1, display_state[1]);
  tm1637.display(2, display_state[2]);
  tm1637.display(3, display_state[3]);
}

void blink(int ms)
{
  tm1637.clearDisplay();
  delay(ms);
  tm1637.display(0, display_state[0]);
  tm1637.display(1, display_state[1]);
  tm1637.display(2, display_state[2]);
  tm1637.display(3, display_state[3]);
  delay(ms);
}

void blink_minutes(int ms) {
   tm1637.clearDigit(0);
   tm1637.clearDigit(1);
   delay(ms);
   tm1637.display(0, display_state[0]);
   tm1637.display(1, display_state[1]);
   tm1637.display(2, display_state[2]);
   tm1637.display(3, display_state[3]);
   delay(ms);
}

void blink_seconds(int ms) {
   tm1637.clearDigit(2);
   tm1637.clearDigit(3);
   delay(ms);
   tm1637.display(0, display_state[0]);
   tm1637.display(1, display_state[1]);
   tm1637.display(2, display_state[2]);
   tm1637.display(3, display_state[3]);
   delay(ms);
}
