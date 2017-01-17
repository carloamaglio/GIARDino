/*
 * utils.cpp - 
 *  Carlo Amaglio, Via Emigli 10 - 25081 Bedizzole (Italy)
 * 
 * Released into the public domain
 */

#include <TimeLib.h>
#include "keypad.h"
#include "utils.h"

void timerSet(timer* t, unsigned long interval) {
  t->interval = interval;
  t->start = millis();
}

int timerExpired(timer* t) {
  return (millis() - t->start) >= t->interval;
}


void print(int x, int y, char* s) {
	lcd.setCursor(x, y);
	print(s);
}

void print(char* s) {
  lcd.print(s);
}

//void print(int n) {
//  char s[10];
//  print(itoa(n, s, 10));
//}

int getIntLen(unsigned int val) {
  int i=0;
  if (val==0) return 1;
  do {
    val/=10; 
    i++;
  } while(val>0);
  return i;
}

void printInt(int value, int maxLen) {
  int nZero = maxLen-getIntLen(value);
  while (nZero-- > 0) lcd.print('0');
  lcd.print(value);
}


static const char* months[] = { "GEN", "FEB", "MAR", "APR", "MAG", "GIU", "LUG", "AGO", "SET", "OTT", "NOV", "DIC" };
char* monthName(int m) { return months[m-1]; }

static const char* days[] = { "Do", "Lu", "Ma", "Me", "Gi", "Ve", "Sa" };

void showDateTime() {
  DateTime now = RTC.now();
  char s[20];
  sprintf(s, "%02d/%02d/%02d%s ", now.day(), now.month(), (now.year()%100), days[now.dayOfTheWeek()]);
  print(0, 0, s);
  sprintf(s, "%02d:%02d:%02d", now.hour(), now.minute(), now.second());
  print(0, 1, s);
}

void unsupportedSelect() {
  lcd.clear();
  print(0, 0, "Funzione non de-");
  print(0, 1, "finita.  (TASTO)");
  keypad.button();
}
