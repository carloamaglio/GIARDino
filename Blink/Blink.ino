/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO 
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino model, check
  the Technical Specs of your board  at https://www.arduino.cc/en/Main/Products
  
  This example code is in the public domain.

  modified 8 May 2014
  by Scott Fitzgerald
  
  modified 2 Sep 2016
  by Arturo Guadalupi
  
  modified 8 Sep 2016
  by Colby Newman
*/

#include "coroutine.h"

static Coroutines<5> coroutines;

static int flashAnalogPin;
static int flashAnalogTimes;

// flashes a LED attached to analog pin 5 for 100ms
static void flashAnalog(COROUTINE_CONTEXT(coroutine))
{
    COROUTINE_LOCAL(int, i);
    COROUTINE_LOCAL(int, pin);
    COROUTINE_LOCAL(int, times);

    BEGIN_COROUTINE;

    pin = flashAnalogPin;
    times = flashAnalogTimes;

    for (i=0; i<times; i++) {
      analogWrite(pin, 255);
      coroutine.wait(100);
      COROUTINE_YIELD;
  
      analogWrite(pin, 0);
      coroutine.wait(50);
      COROUTINE_YIELD;
    }

    END_COROUTINE;
}

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  flashAnalogPin = 5;
  flashAnalogTimes = 3;
  coroutines.start(flashAnalog);

  delay(50);
  flashAnalogPin = 4;
  flashAnalogTimes = 300;
  coroutines.start(flashAnalog);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second
  coroutines.update();
}

