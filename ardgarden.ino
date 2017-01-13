/*
 * ardgarden - ARDuino GARDEN automation
 *  Carlo Amaglio, Via Emigli 10 - 25081 Bedizzole (Italy)
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

RTC_DS1307 RTC;

static void dateTimeShow(void) { showDateTime(); irrigazioneShowSummary(); }
static void dateTimeSelect(void) { setDateAndTime(); }

static const Item items[] {
  ITEM(dateTime), 
  ITEM_N(irrigazioneTimerDetail, 00), 
  ITEM_N(irrigazioneTimerDetail, 01), 
  ITEM_N(irrigazioneTimerDetail, 02), 
  ITEM_N(irrigazioneTimerDetail, 03), 
  ITEM_N(irrigazioneTimerDetail, 04), 
  ITEM_N(irrigazioneTimerDetail, 05), 
  ITEM_N(irrigazioneTimerDetail, 06), 
  ITEM_N(irrigazioneTimerDetail, 07), 
  ITEM_N(irrigazioneTimerDetail, 08), 
  ITEM_N(irrigazioneTimerDetail, 09), 
  ITEM_N(irrigazioneOutState, 00), 
  ITEM_N(irrigazioneOutState, 01), 
  ITEM_N(irrigazioneOutState, 02), 
  ITEM_N(irrigazioneOutState, 03), 
  ITEM_N(irrigazioneOutState, 04), 
  ITEM_N(irrigazioneOutState, 05), 
};

static MENU(mainMenu, items, 1);


static void myloop(void) {
  irrigazioneLoop();
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
  print(0, 0, "*** BEDIZZOLE ***"); delay(1000);

  Wire.begin();
  lcd.clear();
  if (!RTC.isrunning()) {
    print(0, 0, "RTC is NOT"); print(0, 1, "running!"); delay(2000);
    // following line sets the RTC to the date &amp; time this sketch was compiled
    RTC.adjust(DateTime(__DATE__, __TIME__));
  } else {
    print(0, 0, "RTC is running"); print(0, 1, "properly."); delay(2000);
  }

  lcd.clear();

  irrigazioneInit();
  menuInit(mainMenu);
  wdt_enable(WDTO_2S);

  lcd.clear();
}

/**
 * 
 */
void loop() {
  delay(10);
  menuTask(mainMenu);
}
