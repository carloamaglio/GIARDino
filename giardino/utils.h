/*
 * utils.h - 
 *  Carlo Amaglio, Via Emigli 10 - 25081 Bedizzole (Italy)
 * 
 * Released into the public domain
 */

#ifndef UTILS_H
#define UTILS_H

#include <arduino.h>
#include <stdio.h>
#include <RTClib.h>
#include <LiquidCrystal.h>

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)

#define ATMEGA  1
#define UNO 0

#else defined(__AVR_ATmega328P__)

#define ATMEGA  0
#define UNO 1

#endif

#define LENOF(a)  (sizeof(a)/sizeof(a[0]))

typedef struct timer {
  unsigned long start;
  unsigned long interval;
} timer;

extern void timerSet(timer* t, unsigned long interval);
extern int timerExpired(timer* t);

extern RTC_DS1307 RTC;

extern LiquidCrystal lcd;
extern void print(int x, int y, char* s);
extern void print(char* s);
//extern void print(int n);
extern int getIntLen(unsigned int val);
extern void printInt(int value, int maxLen);

extern char* monthName(int m);
extern void showDateTime();

extern void unsupportedSelect();

#define p(S)	Serial.print(S)
#define pln(S)	Serial.println(S)


#endif
