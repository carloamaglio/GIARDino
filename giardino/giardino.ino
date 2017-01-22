/*
 * GIARDino - YAAGA Yet Another Arduino Garden Automation
 *  Carlo Amaglio, Via Emigli 10 - 25081 Bedizzole (Italy)
 * 
 * Released into the public domain
 */

#include <avr/wdt.h>
#include <Wire.h>
#include <TimeLib.h>
#include <RTClib.h>
#include <LiquidCrystal.h>
#include "keypad.h"
#include "utils.h"
#include "edit.h"
#include "menu.h"

#include "irrigazione.h"
#include "lightings.h"

RTC_DS1307 RTC;

struct Task {
  void (*init)();
  void (*loop)();
};

static struct Task tasks[] {
  { irrigazioneInit, irrigazioneLoop }, 
  { lightingsInit, lightingsLoop }, 
};

static void dateTimeShow(void) { showDateTime(); irrigazioneShowSummary(); }
static void dateTimeSelect(void) { setDateAndTime(); }

static const Item mainMenuItems[] {
  ITEM(dateTime), 
  ITEM(irrigazione), 
  ITEM(lightings), 
};

static MENU(mainMenu, mainMenuItems, 1);


static void myloop(void) {
  for (int i=0; i<sizeof(tasks)/sizeof(tasks[0]); i++) tasks[i].loop();
  wdt_reset();
}

/**
 * 
 */
void setup() {
  Serial.begin(9600);
  while (!Serial) ; // wait for Arduino Serial Monitor
  delay(200);

  keypad.setup(myloop);
  print(0, 0, "*** BEDIZZOLE ***"); delay(500);

  Wire.begin();
  lcd.clear();
  if (!RTC.isrunning()) {
    print(0, 0, "RTC is NOT"); print(0, 1, "running!"); delay(1000);
    // following line sets the RTC to the date &amp; time this sketch was compiled
    RTC.adjust(DateTime(__DATE__, __TIME__));
  } else {
    print(0, 0, "RTC is running"); print(0, 1, "properly."); delay(1000);
  }

  for (int i=0; i<sizeof(tasks)/sizeof(tasks[0]); i++) tasks[i].init();
  menuInit(mainMenu);
  wdt_enable(WDTO_2S);

  lcd.clear();
}

/**
 * 
 */
void loop() {
  menuTask(mainMenu);
}
