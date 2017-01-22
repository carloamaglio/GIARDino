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

#define NUMRELE 6

static Rele rele[NUMRELE];

typedef struct GTimer {
  char rele;      // rele affected by the timer (0=all available rele)
  unsigned int active:1;
  unsigned int sunday:1;
  unsigned int monday:1;
  unsigned int tuesday:1;
  unsigned int wednesday:1;
  unsigned int thursday:1;
  unsigned int friday:1;
  unsigned int saturday:1;
  unsigned int tStart;
  unsigned int tEnd;
} GTimer;

#define NUMOFTIMERS 10


void irrigazioneInit() {
  rele[0].setAddr(12);
  rele[1].setAddr(0);
  rele[2].setAddr(1);
  rele[3].setAddr(2);
  rele[4].setAddr(3);
  rele[5].setAddr(11);

  byte cg0[8] = {0x0,0x1b,0xe,0x4,0xe,0x1b,0x0,0x0}; 
  lcd.createChar(1, cg0);  // Carattere personalizzato (X -> Spento)
  byte cg1[8] = {0x0,0x1,0x3,0x16,0x1c,0x8,0x0,0x0}; 
  lcd.createChar(2, cg1);  // Carattere personalizzato (V -> Acceso)
  byte cg2[8] = {0x0,0xe,0x15,0x17,0x11,0xe,0x0,0x0}; 
  lcd.createChar(3, cg2);  // Carattere personalizzato (orologio -> Timer)
  byte cg3[8] = {0x8,0x8,0x0,0x2,0x4,0x8,0x8,0x0}; 
  lcd.createChar(4, cg3);  // Carattere personalizzato (contatto aperto)
  byte cg4[8] = {0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x0}; 
  lcd.createChar(5, cg4);  // Carattere personalizzato (contatto chiuso)

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
  int states[NUMRELE];
  for (int i=0; i<NUMRELE; i++) states[i]=0;
  for (int i=0; i<NUMOFTIMERS; i++) {
    readTimer(t, i);
    states[t.rele] = 
      t.active && 
      day(now, t) && 
      nowmin>=t.tStart && 
      nowmin<t.tEnd
    ;
  }
  for (int i=0; i<NUMRELE; i++) rele[i].setState(states[i]);
  //rele[0].setState((millis() % 1000) > 700);	// demo
}

static char releState(int nr) {
  return rele[nr].getState()?5:4;
}

static void printReleState(int nr) {
  lcd.print(releState(nr));
}

static void printReleStates(int r, int n) {
  while (n-->0) printReleState(r++);
}

void irrigazioneShowSummary() {
  lcd.setCursor(13, 0); printReleStates(0, 3);
  lcd.setCursor(13, 1); printReleStates(2, 3);
}

void irrigazioneShow() {
  print(0, 0, "IRRIGAZIONE  ");
  print(0, 1, "             ");
  irrigazioneShowSummary();
}

void irrigazioneOutStateShow(int n) {
  print(0, 0, "Outputs "); printInt(n+1, 2); print(":     ");
  print(0, 1, "                ");
}
void irrigazioneOutState00Show(void) { irrigazioneOutStateShow(0); }
void irrigazioneOutState01Show(void) { irrigazioneOutStateShow(1); }
void irrigazioneOutState02Show(void) { irrigazioneOutStateShow(2); }
void irrigazioneOutState03Show(void) { irrigazioneOutStateShow(3); }
void irrigazioneOutState04Show(void) { irrigazioneOutStateShow(4); }
void irrigazioneOutState05Show(void) { irrigazioneOutStateShow(5); }

void irrigazioneOutStateSelect(int n) {
  unsupportedSelect();
}
void irrigazioneOutState00Select(void) { irrigazioneOutStateSelect(0); }
void irrigazioneOutState01Select(void) { irrigazioneOutStateSelect(1); }
void irrigazioneOutState02Select(void) { irrigazioneOutStateSelect(2); }
void irrigazioneOutState03Select(void) { irrigazioneOutStateSelect(3); }
void irrigazioneOutState04Select(void) { irrigazioneOutStateSelect(4); }
void irrigazioneOutState05Select(void) { irrigazioneOutStateSelect(5); }

void irrigazioneTimerDetailShow(int n) {
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

  sprintf(s, "%02d:%02d>%02d:%02d  U=%1d", t.tStart/60, t.tStart%60, t.tEnd/60, t.tEnd%60, t.rele);
  print(0, 1, s);
}
void irrigazioneTimerDetail00Show(void) { irrigazioneTimerDetailShow(0); }
void irrigazioneTimerDetail01Show(void) { irrigazioneTimerDetailShow(1); }
void irrigazioneTimerDetail02Show(void) { irrigazioneTimerDetailShow(2); }
void irrigazioneTimerDetail03Show(void) { irrigazioneTimerDetailShow(3); }
void irrigazioneTimerDetail04Show(void) { irrigazioneTimerDetailShow(4); }
void irrigazioneTimerDetail05Show(void) { irrigazioneTimerDetailShow(5); }
void irrigazioneTimerDetail06Show(void) { irrigazioneTimerDetailShow(6); }
void irrigazioneTimerDetail07Show(void) { irrigazioneTimerDetailShow(7); }
void irrigazioneTimerDetail08Show(void) { irrigazioneTimerDetailShow(8); }
void irrigazioneTimerDetail09Show(void) { irrigazioneTimerDetailShow(9); }

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


#define editTimeField(val, tov, toval, max, x, y)   \
  {                                                 \
    int v = tov;                                    \
    int rv = editUnsigned(&v, 0, max, x, y);        \
    if (rv==btnSELECT) val = toval;                 \
    return rv;                                      \
  }

#define _editHour(val, x, y)   editTimeField(val, val/60, v*60+(val%60), 23, x, y)
static int editStartHour(GTimer &t, int x, int y) { _editHour(t.tStart, x, y); }
static int editEndHour(GTimer &t, int x, int y) { _editHour(t.tEnd, x, y); }

#define _editMinute(val, x, y) editTimeField(val, val%60, val-(val%60)+v, 59, x, y)
static int editStartMinute(GTimer &t, int x, int y) { _editMinute(t.tStart, x, y); }
static int editEndMinute(GTimer &t, int x, int y) { _editMinute(t.tEnd, x, y); }

void irrigazioneTimerDetailSelect(int n) {
  int i=0;
  GTimer t;
  readTimer(t, n);
  while (i>=0 && i<13) {
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
      case 10: key=editEndHour(t, 6, 1); break;
      case 11: key=editEndMinute(t, 9, 1); break;
      case 12: key=editUnsignedChar(&t.rele, 0, NUMRELE, 15, 1); break;
    }
    switch (key) {
      case btnSELECT: writeTimer(t, n);
      case btnRIGHT: i++; break;
      case btnLEFT: i--; break;
      case btnNONE: i=-1; break;
    }
  }
}
void irrigazioneTimerDetail00Select(void) { irrigazioneTimerDetailSelect(0); }
void irrigazioneTimerDetail01Select(void) { irrigazioneTimerDetailSelect(1); }
void irrigazioneTimerDetail02Select(void) { irrigazioneTimerDetailSelect(2); }
void irrigazioneTimerDetail03Select(void) { irrigazioneTimerDetailSelect(3); }
void irrigazioneTimerDetail04Select(void) { irrigazioneTimerDetailSelect(4); }
void irrigazioneTimerDetail05Select(void) { irrigazioneTimerDetailSelect(5); }
void irrigazioneTimerDetail06Select(void) { irrigazioneTimerDetailSelect(6); }
void irrigazioneTimerDetail07Select(void) { irrigazioneTimerDetailSelect(7); }
void irrigazioneTimerDetail08Select(void) { irrigazioneTimerDetailSelect(8); }
void irrigazioneTimerDetail09Select(void) { irrigazioneTimerDetailSelect(9); }

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
  ITEM_N(irrigazioneOutState, 04), 
  ITEM_N(irrigazioneOutState, 05), 
};

static MENU(menu, items, 0);

void irrigazioneSelect() {
  menuTask(menu);
}
