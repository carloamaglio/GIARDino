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
#ifdef IRDEMO
#include <IRremote.h>
#include <IRremoteInt.h>
#endif
#include "keypad.h"
#include "utils.h"
#include "edit.h"
#include "menu.h"

#include "irrigazione.h"
#include "lightings.h"

//#define RELEDEMO

RTC_DS1307 RTC;


struct Task {
  void (*init)();
  void (*loop)();
};

static struct Task tasks[] {
  { irrigazioneInit, irrigazioneLoop }, 
  { lightingsInit, lightingsLoop }, 
};

static void dateTimeShow(void) { showDateTime(); irrigazioneShowSummary(); lightingsShowSummary(); }
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

#ifdef RELEDEMO
static int addr[] { 0, 1, 2, 3, 11, 12 };
static int n = LENOF(addr);
#endif

#ifdef IRDEMO
int RECV_PIN = 13;
IRrecv irrecv(RECV_PIN);
decode_results results;
#endif

/**
 * 
 */
void setup() {
#ifdef RELEDEMO
  for (int i=0; i<n; i++) pinMode(addr[i], OUTPUT);
  return;
#endif
  Serial.begin(9600);
  while (!Serial) ; // wait for Arduino Serial Monitor
  delay(200);

  keypad.setup(myloop);
  print(0, 0, "*** BEDIZZOLE **"); delay(500);

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

#ifdef IRDEMO
  irrecv.enableIRIn(); // Start the receiver
#endif

  lcd.clear();
  wdt_enable(WDTO_2S);
}

#ifdef RELEDEMO
static void setRele(int r) {
    digitalWrite(addr[r], 0);
    delay(50);
    digitalWrite(addr[r], 1);
    delay(5);
}
#endif

#ifdef IRDEMO
void dump(decode_results *results) {
#if 0
  // Dumps out the decode_results structure.
  // Call this after IRrecv::decode()
  int count = results->rawlen;
  if (results->decode_type == UNKNOWN) {
    Serial.print("Unknown encoding: ");
  }
  else if (results->decode_type == NEC) {
    Serial.print("Decoded NEC: ");
  }
  else if (results->decode_type == SONY) {
    Serial.print("Decoded SONY: ");
  }
  else if (results->decode_type == RC5) {
    Serial.print("Decoded RC5: ");
  }
  else if (results->decode_type == RC6) {
    Serial.print("Decoded RC6: ");
  }
  else if (results->decode_type == PANASONIC) {
    Serial.print("Decoded PANASONIC - Address: ");
    Serial.print(results->address, HEX);
    Serial.print(" Value: ");
  }
  else if (results->decode_type == LG) {
    Serial.print("Decoded LG: ");
  }
  else if (results->decode_type == JVC) {
    Serial.print("Decoded JVC: ");
  }
  else if (results->decode_type == AIWA_RC_T501) {
    Serial.print("Decoded AIWA RC T501: ");
  }
  else if (results->decode_type == WHYNTER) {
    Serial.print("Decoded Whynter: ");
  }
  Serial.print(results->value, HEX);
  Serial.print(" (");
  Serial.print(results->bits, DEC);
  Serial.println(" bits)");
  Serial.print("Raw (");
  Serial.print(count, DEC);
  Serial.print("): ");

  for (int i = 1; i < count; i++) {
    if (i & 1) {
      Serial.print(results->rawbuf[i]*USECPERTICK, DEC);
    }
    else {
      Serial.write('-');
      Serial.print((unsigned long) results->rawbuf[i]*USECPERTICK, DEC);
    }
    Serial.print(" ");
  }
  Serial.println();
#endif
}
#endif

/**
 * 
 */
void loop() {
#ifdef RELEDEMO
  for (int i=0; i<n; i++) setRele(i);
  for (int i=n-1; i>=0; i--) setRele(i);
  return;
#endif

#ifdef IRDEMO
  if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);
    dump(&results);
    irrecv.resume(); // Receive the next value
  }
  wdt_reset();
#endif

  menuTask(mainMenu);
}
