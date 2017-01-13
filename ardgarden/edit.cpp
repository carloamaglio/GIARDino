#include "keypad.h"
#include "utils.h"

static int getIntLen(unsigned int val) {
  int i=0;
  if (val==0) return 1;
  do {
    val/=10; 
    i++;
  } while(val>0);
  return i;
}

static int getListLen(char **values) {
  int rv=0;
  while (*values) {
    rv++;
    values++;
  }
  return rv;
}

static int getListMaxLen(char **values) {
  int rv=0;
  while (*values) {
    rv = max(rv, strlen(*values));
    values++;
  }
  return rv;
}

void LcdPrintInt_Zero(int value, int maxLen) {
  int nZero = maxLen-getIntLen(value);

  while (nZero>0) {
    lcd.print('0');
    nZero--;
  }
  lcd.print(value);
}

static void LcdPrintListEdit(char* val, int x, int y, int maxLen) {
  char s[maxLen+1];
  int len = strlen(val);
  int padlen = maxLen-len;
  if (padlen>0) memset(s, ' ', padlen);
  strcpy(s+padlen, val);
  print(x, y, s);
  lcd.setCursor(x+maxLen-1, y);
  //p(val); p(", s='"); p(s); pln("'");
}

static void LcdPrintIntEdit(int val, int x, int y, int maxLen) {
  lcd.setCursor(x, y);
  LcdPrintInt_Zero(val, maxLen);
  lcd.setCursor(x+maxLen-1, y);
}

#define EDITTIMEOUT 20000

void dumpList(char **values) {
	while (*values) {
		p("'"); p(*values); p("' ");
		values++;
	}
	pln("");
}

/*
 *
 */
int editList(int *pVal, char **values, int x, int y)
{
  int key;
  int val = *pVal;
  int exit = -1;
  int maxLen = getListMaxLen(values);
  int min = 0;
  int max = getListLen(values)-1;

  //p("val="); p(val); p(", maxLen="); p(maxLen); p(", max="); pln(max);
  //dumpList(values);

  lcd.blink();

  if (val>max) val=max;
  else if (val<min) val=min;
  LcdPrintListEdit(values[val], x, y, maxLen);

  while (exit==-1) {
    key = keypad.waitForButton(EDITTIMEOUT);

    if (key==btnUP) {
      if (val>=max) val=min; else val++;
      LcdPrintListEdit(values[val], x, y, maxLen);
    } else if (key==btnDOWN) {
      if (val<=min) val=max; else val--;
      LcdPrintListEdit(values[val], x, y, maxLen);
    } else if (key==btnSELECT) exit=1;
    else exit=0;
  }

  lcd.noBlink();

  if (exit==1) *pVal=val;
  else LcdPrintListEdit(values[*pVal], x, y, maxLen);

  return key;
}

int editUnsigned(int *pVal, int min, int max, int x, int y) {
  int key;
  int val = *pVal;
  int exit = -1;
  int maxLen = getIntLen(max);

  lcd.blink();

  if (val>max) val=max;
  else if (val<min) val=min;
  LcdPrintIntEdit(val, x, y, maxLen);

  while (exit==-1) {
    key = keypad.waitForButton(EDITTIMEOUT);
    
    if (key==btnUP) {
      if (val>=max) val=min; else val++;
      LcdPrintIntEdit(val, x, y, maxLen);
    } else if (key==btnDOWN) {
      if (val<=min) val=max; else val--;
      LcdPrintIntEdit(val, x, y, maxLen);
    } else if (key==btnSELECT) exit=1;
    else exit=0;
  }

  lcd.noBlink();

  if (exit==1) *pVal=val;
  else {
    lcd.setCursor(x, y);
    LcdPrintInt_Zero(*pVal, maxLen);
  }

  return key;
}

int editUnsignedChar(char *pVal, int min, int max, int x, int y) {
  int v = *pVal;
  int key = editUnsigned(&v, min, max, x, y);
  if (key==btnSELECT) *pVal = v;
  return key;
}

static int getField(DateTime *dt, int i) {
  switch (i) {
    case 0: return dt->year()-2000;
    case 1: return dt->month();
    case 2: return dt->day();
    case 3: return dt->hour();
    case 4: return dt->minute();
    default: return dt->second();
  }
}

static void setField(int i, int v) {
  DateTime now = RTC.now();
  switch (i) {
    case 0: now = DateTime(v+2000, now.month(), now.day(), now.hour(), now.minute(), now.second()); break;
    case 1: now = DateTime(now.year(), v, now.day(), now.hour(), now.minute(), now.second()); break;
    case 2: now = DateTime(now.year(), now.month(), v, now.hour(), now.minute(), now.second()); break;
    case 3: now = DateTime(now.year(), now.month(), now.day(), v, now.minute(), now.second()); break;
    case 4: now = DateTime(now.year(), now.month(), now.day(), now.hour(), v, now.second()); break;
    default: now = DateTime(now.year(), now.month(), now.day(), now.hour(), now.minute(), v); break;
  }
  RTC.adjust(now);
}

static const int min[] = { 0, 1, 1, 0, 0, 0 };
static int getMin(DateTime *dt, int i) { return min[i]; }

static int getMax(DateTime *dt, int i) {
  switch (i) {
    case 0: return 99;
    case 1: return 12;
    case 2: return 31;
    case 3: return 23;
    case 4: return 59;
    default: return 59;
  }
}

static const int xField[] = { 6, 3, 0, 0, 3, 6 };
static int getXField(DateTime *dt, int i) { return xField[i]; }

static const int yField[] = { 0, 0, 0, 1, 1, 1 };
static int getYField(DateTime *dt, int i) { return yField[i]; }

static int edit(int i) {
  int key;
  DateTime now = RTC.now();
  int v = getField(&now, i);
  if ((key=editUnsigned(&v, getMin(&now, i), getMax(&now, i), getXField(&now, i), getYField(&now, i)))==btnSELECT) {
    setField(i, v);
  }
  return key;
}

#define LEN(a)  (sizeof(a)/sizeof(a[0]))

static const int indexes[] = { 2, 1, 0, 3, 4, 5 };
void setDateAndTime() {
  int i=0;
  while (i>=0 && i<LEN(indexes)) {
    switch (edit(indexes[i])) {
      case btnSELECT:
      case btnRIGHT:
        i++;
        break;
      case btnLEFT:
        i--;
        break;
      case btnNONE:
        i=-1;
        break;
    }
  }
}

