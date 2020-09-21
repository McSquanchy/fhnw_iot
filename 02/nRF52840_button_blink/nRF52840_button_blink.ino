/*
  Blink

  Turns an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino
  model, check the Technical Specs of your board at:
  https://www.arduino.cc/en/Main/Products

  modified 8 May 2014
  by Scott Fitzgerald
  modified 2 Sep 2016
  by Arturo Guadalupi
  modified 8 Sep 2016
  by Colby Newman

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/Blink
*/

const int BUTTON_PIN = 9;
const int LED_PIN = 5;

int state = 0;

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(BUTTON_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(115200);
}

// the loop function runs over and over again forever
void loop() {
  int b = digitalRead(BUTTON_PIN);
  if (state == 0 && b) { // s is state
    state = 1; digitalWrite(LED_PIN, HIGH); // on
  } else if (state == 1 && !b) {
    state = 2;
  } else if (state == 2 && b) {
    state = 3; digitalWrite(LED_PIN, LOW); // off
  } else if (state == 3 && !b) {
    state = 0;
  }
  delay(1);
}
