/*
 * keypad.cpp - library for controlling the keypad
 *  Carlo Amaglio, Via Emigli 10 - 25081 Bedizzole (Italy)
 * 
 * Released into the public domain
 */

#include <arduino.h>
#include "keypad.h"
#include "utils.h"

#define BACKLIT 10

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
Keypad keypad;

#define KAVAILABLE  (key!=btnNONE)

Keypad::Keypad() {
	lastKey = key = btnNONE;
	adc_key_in = 0;
}

void Keypad::setup(void (*task)(void)) {
  this->task = task;
  lcd.begin(16, 2);
  pinMode(BACKLIT, OUTPUT);
  backlit(1);
}

void Keypad::loop() {
  static int state;
  static int tk;
  static timer tmr[1];

  int k = readButtons();
  if (k!=lastKey && k!=btnNONE) {
    tk = k;
    timerSet(tmr, 20);
    state = 1;
  } else if (state==1 && timerExpired(tmr)) {
    key = k;
    state = 0;
  }
  lastKey=k;
  screensaver();
  task();
}

void Keypad::backlit(int on) {
  digitalWrite(BACKLIT, on);
}

int Keypad::buttonAvailable() {
  loop();
  return KAVAILABLE;
}

int Keypad::button() {
  loop();
	int rv = key;
	key = btnNONE;
	return rv;
}

int Keypad::waitForButton(unsigned long timeout) {
  int rv=-1;

  timer tmr[1];
  timerSet(tmr, timeout);
  while (rv==-1) {
    if (keypad.buttonAvailable()) {
      rv = keypad.button();
    } else if (timerExpired(tmr)) rv=btnNONE;
  }
  return rv;
}

/**
 * read the buttons
 */
int Keypad::readButtons() {
	adc_key_in = analogRead(0);

	// We make this the 1st option for speed reasons since it will be the most likely result
	if (adc_key_in >= 850) return btnNONE;
	if (adc_key_in < 50) return btnRIGHT;
	if (adc_key_in < 200) return btnUP;
	if (adc_key_in < 350) return btnDOWN;
	if (adc_key_in < 500) return btnLEFT;
	return btnSELECT;
}

char* Keypad::buttonName(int b) {
  switch (b) {
    case btnSELECT: return "SELECT";
    case btnLEFT:   return "LEFT  ";
    case btnUP:     return "UP    ";
    case btnDOWN:   return "DOWN  ";
    case btnRIGHT:  return "RIGHT ";
    default:        return "------";
  }
}

#define SCRSVRTIME  60000

void Keypad::screensaver() {
  static int state;
  static timer tmr[1];

  switch (state) {
    case 0:
      backlit(1);
      timerSet(tmr, SCRSVRTIME);
      state++;
      break;
    case 1:
      if (KAVAILABLE) {
        timerSet(tmr, SCRSVRTIME);
      } else if (timerExpired(tmr)) {
        backlit(0);
        state++;
      }
      break;
    case 2:
      if (KAVAILABLE) {
        state = 0;
        key = btnNONE;
      }
      break;
  }
}

void Keypad::printButton(int x, int y, char* lbl, int b) {
	lcd.setCursor(x, y);
  if (lbl) print(lbl);
  print(buttonName(b));
}

void Keypad::printButton(int x, int y, char* lbl) {
  printButton(x, y, lbl, button());
}

void Keypad::printButton(int x, int y) {
  printButton(x, y, NULL);
}

void Keypad::debug(int x, int y) {
  lcd.setCursor(x, y);
  lcd.print(millis()/1000);   // display seconds elapsed since power-up
	lcd.print(" ");
	lcd.print(adc_key_in);
	lcd.print("  ");

	printButton(0, 1);
}

