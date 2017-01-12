/*
 * keypad.h - 
 * 
 * Released into the public domain
 */

#ifndef KEYPAD_H
#define KEYPAD_H

#include <arduino.h>
#include <LiquidCrystal.h>

#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

class Keypad {
	public:
		Keypad();
    void setup(void (*task)(void));
		void loop();
    void backlit(int on);
    int buttonAvailable();
		int button();
		int adc_key_in;

    char* buttonName(int b);
    void printButton(int x, int y, char* lbl, int b);
    void printButton(int x, int y, char* lbl);
		void printButton(int x, int y);
		void debug(int x, int y);
	private:
		int key;
    int lastKey;
		int readButtons();
    void screensaver();
    void (*task)(void);
};

extern LiquidCrystal lcd;
extern Keypad keypad;

#endif

