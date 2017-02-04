/*
 * lightings.cpp - 
 *  Carlo Amaglio, Via Emigli 10 - 25081 Bedizzole (Italy)
 * 
 * Released into the public domain
 */

#include <arduino.h>
#include <EEPROM.h>
#include <Sunrise.h>

#include "rele.h"
#include "keypad.h"
#include "edit.h"
#include "utils.h"
#include "menu.h"
#include "lightings.h"

static Rele rele;

static int light;
static void setLight(int on) { rele.setState(light=on); }
static int getLight() { return light; }

// DMS() - Convert Degrees Minutes Seconts to decimal degrees
#define DMS(_d, _m, _s)	((_d) + (_m) / 60.0 + (_s) / 3600.0)
#define Latitude	DMS(45, 30, 41)
#define Longitude	-DMS(10, 25, 27)

static Sunrise sunrise(Latitude, Longitude, -2);

static int currentDay;
static int sunset;
static int state;

static void updateCurrentDay() {
  DateTime now = RTC.now();
  currentDay = now.day();

  // t = minutes past midnight of sunrise (6 am would be 360)
  sunset = sunrise.Set(now.month(), currentDay); // (month,day) - january=1
}

void lightingsInit() {
  rele.setAddr(13);
  sunrise.Astronomical(); //Actual, Civil, Nautical, Astronomical
  updateCurrentDay();
  state = 0;
}

/*
 *
 */
void lightingsLoop() {
  DateTime now = RTC.now();
  if (currentDay != now.day()) {
    updateCurrentDay();
  }
  int minutes = now.hour()*60+now.minute();
  int night = (minutes >= sunset-10);
  switch (state) {
    case 0: // waiting for turning light ON. In this state light can be ON or OFF
      if (night || (minutes < 60)) { setLight(1); state++; }
      break;
    case 1: // waiting for turning light OFF. In this state light can be ON or OFF
      if (minutes >= 60 && !night) { setLight(0); state--; }
      break;
  }
}

void lightingsShowSummary() {
  lcd.setCursor(12, 0); lcd.print((char)(getLight()?2:1));
}

void lightingsShow() {
  print(0, 0, "SUNSET="); lcd.print(sunset/60); lcd.print(":"); printInt(sunset%60, 2);
  print(0, 1, "STATE="); lcd.print(state); lcd.print(", L="); lcd.print(getLight()?"ON ":"OFF");
  lightingsShowSummary();
}

void lightingsSelect() {
  lcd.blink();
  while (1) {
    lcd.clear();
    lightingsShow();
    lcd.setCursor(11, 1);
    int k = keypad.waitForButton(20000);
    if (k==btnUP) setLight(1);
    else if (k==btnDOWN) setLight(0);
    else break;
  }
  lcd.noBlink();
}
