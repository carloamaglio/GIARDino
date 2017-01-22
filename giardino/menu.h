/*
 * menu.h - my simple menu functions for small text LCD (16x2)
 *  Carlo Amaglio, Via Emigli 10 - 25081 Bedizzole (Italy)
 * 
 * Released into the public domain
 */

#ifndef MENU_H
#define MENU_H

#include <arduino.h>
#include <RTClib.h>
#include <LiquidCrystal.h>

typedef struct Item {
  void (*show)();
  void (*select)();
} Item;
#define ITEM(NAME)  { NAME ## Show, NAME ## Select }
#define ITEM_NOSELECT(NAME)  { NAME ## Show, unsupportedSelect }
#define ITEM_N(NAME, N)  { NAME ## N ## Show, NAME ## N ## Select }


typedef struct Menu {
  Item *items;
  unsigned int nItems;
  unsigned int root:1;

  unsigned int curItem;
} Menu;

#define MENU(NAME, ITEMS, ROOT) Menu NAME[] { ITEMS, sizeof(ITEMS)/sizeof(ITEMS[0]), ROOT }

extern void menuInit(struct Menu *menu);
extern void menuTask(struct Menu *menu);


#endif
