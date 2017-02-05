/*
 * irrigazione.cpp - Garden irrigation management module (irrigazione is "garden irrigation" in Italian)
 *  Carlo Amaglio, Via Emigli 10 - 25081 Bedizzole (Italy)
 * 
 * Released into the public domain
 */

#include <arduino.h>
#include <EEPROM.h>
#include "rele.h"
#include "keypad.h"
#include "edit.h"
#include "utils.h"
#include "menu.h"
#include "irrigazione.h"

#define EEPROMOFFSET  0

#define RELAYS 6  //(ATMEGA?6:4)

static Rele rele[RELAYS];
static int addr[] { 2, 3, 11, 12, A2, A3 };


typedef struct GTimer {
  char rele;      // rele affected by the timer
  unsigned int active:1;
  unsigned int sunday:1;
  unsigned int monday:1;
  unsigned int tuesday:1;
  unsigned int wednesday:1;
  unsigned int thursday:1;
  unsigned int friday:1;
  unsigned int saturday:1;
  unsigned int tStart;
  unsigned int tDuration;
} GTimer;

#define TIMERS 10

static char xChar;
static char vChar;
static char clockChar;
static char openChar;
static char closedChar;

void irrigazioneInit() {
  for (int i=0; i<RELAYS; i++) rele[i].setAddr(addr[i]);

  byte cg0[8] = { 0x00, 0x1b, 0x0e, 0x04, 0x0e, 0x1b, 0x00, 0x00 }; 
  xChar = createChar(cg0);  // Carattere personalizzato (X -> Spento)

  byte cg1[8] = { 0x00, 0x01, 0x03, 0x16, 0x1c, 0x08, 0x00, 0x00 }; 
  vChar = createChar(cg1);  // Carattere personalizzato (V -> Acceso)

  byte cg2[8] = { 0x00, 0x0e, 0x15, 0x17, 0x11, 0x0e, 0x00, 0x00 }; 
  clockChar = createChar(cg2);  // Carattere personalizzato (orologio -> Timer)

  byte cg3[8] = { 0x08, 0x08, 0x01, 0x02, 0x04, 0x08, 0x08, 0x00 }; 
  openChar = createChar(cg3);  // Carattere personalizzato (contatto aperto)

  byte cg4[8] = { 0x08, 0x08, 0x04, 0x04, 0x04, 0x08, 0x08, 0x00 }; 
  closedChar = createChar(cg4);  // Carattere personalizzato (contatto chiuso)

  lcd.clear();
  lcd.print("GTimer size="); lcd.print(sizeof(GTimer));
  delay(2000);
}

/**
 * tn - timer number
 */
static int eepromOffset(int tn) {
  return EEPROMOFFSET+tn*sizeof(GTimer);
}

/**
 * t - timer reference
 * tn - timer number
 */
static GTimer readTimer(GTimer& t, int tn) {
  EEPROM.get(eepromOffset(tn), t);
}

/**
 * t - timer reference
 * tn - timer number
 */
static void writeTimer(GTimer& t, int tn) {
  EEPROM.put(eepromOffset(tn), t);
}

static char day(DateTime &now, GTimer &t) {
  switch (now.dayOfTheWeek()) {
    case 0: return t.sunday;
    case 1: return t.monday;
    case 2: return t.tuesday;
    case 3: return t.wednesday;
    case 4: return t.thursday;
    case 5: return t.friday;
    case 6: return t.saturday;
  }
}

/*
 *
 */
void irrigazioneLoop() {
  DateTime now = RTC.now();
  int nowmin = now.hour()*60+now.minute();
  GTimer t;
  int states[RELAYS];
  for (int i=0; i<RELAYS; i++) states[i]=0;
  for (int i=0; i<TIMERS; i++) {
    readTimer(t, i);
    t.rele--;
    if (t.rele>=0 && t.rele<RELAYS) {
      int tEnd = t.tStart + t.tDuration;
      states[t.rele] |= 
        t.active && 
        day(now, t) && 
        nowmin >= t.tStart && 
        nowmin < tEnd
      ;
    }
  }
  for (int i=0; i<RELAYS; i++) rele[i].setState(states[i]);
  //rele[0].setState((millis() % 1000) > 700);	// demo
}

static char releState(int nr) {
  return rele[nr].getState() ? closedChar : openChar;
}

static int x[] { 13, 14, 15, 13, 14, 15 };
static int y[] { 0, 0, 0, 1, 1, 1 };

static void printReleState(int nr) {
  lcd.setCursor(x[nr], y[nr]);
  lcd.print(releState(nr));
}

void irrigazioneShowSummary() {
  for (int i=0; i<RELAYS; i++) printReleState(i);
}

void irrigazioneShow() {
  print(0, 0, "IRRIGAZIONE  ");
  print(0, 1, "             ");
  irrigazioneShowSummary();
}

static void irrigazioneOutStateShow(int n) {
  print(0, 0, "Outputs "); printInt(n+1, 2); print(":     ");
  print(0, 1, "                ");
}
static void irrigazioneOutState00Show(void) { irrigazioneOutStateShow(0); }
static void irrigazioneOutState01Show(void) { irrigazioneOutStateShow(1); }
static void irrigazioneOutState02Show(void) { irrigazioneOutStateShow(2); }
static void irrigazioneOutState03Show(void) { irrigazioneOutStateShow(3); }
static void irrigazioneOutState04Show(void) { irrigazioneOutStateShow(4); }
static void irrigazioneOutState05Show(void) { irrigazioneOutStateShow(5); }

static void irrigazioneOutStateSelect(int n) {
  unsupportedSelect();
}
static void irrigazioneOutState00Select(void) { irrigazioneOutStateSelect(0); }
static void irrigazioneOutState01Select(void) { irrigazioneOutStateSelect(1); }
static void irrigazioneOutState02Select(void) { irrigazioneOutStateSelect(2); }
static void irrigazioneOutState03Select(void) { irrigazioneOutStateSelect(3); }
static void irrigazioneOutState04Select(void) { irrigazioneOutStateSelect(4); }
static void irrigazioneOutState05Select(void) { irrigazioneOutStateSelect(5); }

static void irrigazioneTimerDetailShow(int n) {
  char s[20];
  GTimer t;
  readTimer(t, n);

  sprintf(s, "T%02d ", n+1);
  print(0, 0, s);
  lcd.print(t.active?" ON":"OFF");
  lcd.print("  ");
  lcd.print(t.monday?"L":"-");
  lcd.print(t.tuesday?"M":"-");
  lcd.print(t.wednesday?"M":"-");
  lcd.print(t.thursday?"G":"-");
  lcd.print(t.friday?"V":"-");
  lcd.print(t.saturday?"S":"-");
  lcd.print(t.sunday?"D":"-");

  sprintf(s, "%02d:%02d > %02d   U=%1d", t.tStart/60, t.tStart%60, t.tDuration, t.rele);
  print(0, 1, s);
}
static void irrigazioneTimerDetail00Show(void) { irrigazioneTimerDetailShow(0); }
static void irrigazioneTimerDetail01Show(void) { irrigazioneTimerDetailShow(1); }
static void irrigazioneTimerDetail02Show(void) { irrigazioneTimerDetailShow(2); }
static void irrigazioneTimerDetail03Show(void) { irrigazioneTimerDetailShow(3); }
static void irrigazioneTimerDetail04Show(void) { irrigazioneTimerDetailShow(4); }
static void irrigazioneTimerDetail05Show(void) { irrigazioneTimerDetailShow(5); }
static void irrigazioneTimerDetail06Show(void) { irrigazioneTimerDetailShow(6); }
static void irrigazioneTimerDetail07Show(void) { irrigazioneTimerDetailShow(7); }
static void irrigazioneTimerDetail08Show(void) { irrigazioneTimerDetailShow(8); }
static void irrigazioneTimerDetail09Show(void) { irrigazioneTimerDetailShow(9); }

static char* ABILITAZIONE[] { "OFF", " ON", 0 };

#define editListField(rv, lst, val, x, y) \
  {                                       \
    int v = (val);                        \
    rv = editList(&v, lst, x, y);         \
    if (rv==btnSELECT) val = v;           \
  }

static char* MONDAY[] { "-", "L", 0 };
static char* TUESDAY[] { "-", "M", 0 };
static char* WEDNESDAY[] { "-", "M", 0 };
static char* THURSDAY[] { "-", "G", 0 };
static char* FRIDAY[] { "-", "V", 0 };
static char* SATURDAY[] { "-", "S", 0 };
static char* SUNDAY[] { "-", "D", 0 };

#define _editUnsignedField(val, tov, toval, min, max, x, y)   \
  {                                                           \
    int v = tov;                                              \
    int rv = editUnsigned(&v, min, max, x, y);                \
    if (rv==btnSELECT) val = toval;                           \
    return rv;                                                \
  }

#define editUnsignedField(val, min, max, x, y)  _editUnsignedField(val, val, val, min, max, x, y)


#define editTimeField(val, tov, toval, max, x, y)   \
  {                                                 \
    int v = tov;                                    \
    int rv = editUnsigned(&v, 0, max, x, y);        \
    if (rv==btnSELECT) val = toval;                 \
    return rv;                                      \
  }

#define _editHour(val, x, y)   editTimeField(val, val/60, v*60+(val%60), 23, x, y)
static int editStartHour(GTimer &t, int x, int y) { _editHour(t.tStart, x, y); }

#define _editMinute(val, x, y) editTimeField(val, val%60, val-(val%60)+v, 59, x, y)
static int editStartMinute(GTimer &t, int x, int y) { _editMinute(t.tStart, x, y); }

static int editDuration(GTimer &t, int x, int y) { editUnsignedField(t.tDuration, 0, 59, x, y); }

static void irrigazioneTimerDetailSelect(int n) {
  int i=0;
  GTimer t;
  readTimer(t, n);
  while (i>=0 && i<12) {
    int key;
    switch (i) {
      case 0: editListField(key, ABILITAZIONE, t.active, 4, 0); break;
      case 1: editListField(key, MONDAY, t.monday, 9, 0); break;
      case 2: editListField(key, TUESDAY, t.tuesday, 10, 0); break;
      case 3: editListField(key, WEDNESDAY, t.wednesday, 11, 0); break;
      case 4: editListField(key, THURSDAY, t.thursday, 12, 0); break;
      case 5: editListField(key, FRIDAY, t.friday, 13, 0); break;
      case 6: editListField(key, SATURDAY, t.saturday, 14, 0); break;
      case 7: editListField(key, SUNDAY, t.sunday, 15, 0); break;
      case 8: key=editStartHour(t, 0, 1); break;
      case 9: key=editStartMinute(t, 3, 1); break;
      case 10: key=editUnsigned(&t.tDuration, 1, 59, 8, 1); break;
      case 11: key=editUnsignedChar(&t.rele, 1, RELAYS, 15, 1); break;
    }
    switch (key) {
      case btnSELECT: writeTimer(t, n);
      case btnRIGHT: i++; break;
      case btnLEFT: i--; break;
      case btnNONE: i=-1; break;
    }
  }
}
static void irrigazioneTimerDetail00Select(void) { irrigazioneTimerDetailSelect(0); }
static void irrigazioneTimerDetail01Select(void) { irrigazioneTimerDetailSelect(1); }
static void irrigazioneTimerDetail02Select(void) { irrigazioneTimerDetailSelect(2); }
static void irrigazioneTimerDetail03Select(void) { irrigazioneTimerDetailSelect(3); }
static void irrigazioneTimerDetail04Select(void) { irrigazioneTimerDetailSelect(4); }
static void irrigazioneTimerDetail05Select(void) { irrigazioneTimerDetailSelect(5); }
static void irrigazioneTimerDetail06Select(void) { irrigazioneTimerDetailSelect(6); }
static void irrigazioneTimerDetail07Select(void) { irrigazioneTimerDetailSelect(7); }
static void irrigazioneTimerDetail08Select(void) { irrigazioneTimerDetailSelect(8); }
static void irrigazioneTimerDetail09Select(void) { irrigazioneTimerDetailSelect(9); }

static const Item items[] {
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
#if RELAYS>4
  ITEM_N(irrigazioneOutState, 04), 
#if RELAYS>5
  ITEM_N(irrigazioneOutState, 05), 
#endif
#endif
};

static MENU(menu, items, 0);

void irrigazioneSelect() {
  menuTask(menu);
}
