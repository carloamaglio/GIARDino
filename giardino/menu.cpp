/*
 * menu.cpp - 
 *  Carlo Amaglio, Via Emigli 10 - 25081 Bedizzole (Italy)
 * 
 * Released into the public domain
 */

#include "keypad.h"
#include "utils.h"
#include "menu.h"

void menuInit(struct Menu *menu) {
  menu->curItem = 0;
}

void menuTask(struct Menu *menu) {
  int done = 0;
  while (!done) {
    menu->items[menu->curItem].show();

    int key = keypad.button();

    if (key==btnSELECT) {
      menu->items[menu->curItem].select();
    } else if (key==btnUP) {
      if (menu->curItem >= menu->nItems-1) menu->curItem=0; else menu->curItem++;
      lcd.clear();
    } else if (key==btnDOWN) {
      if (menu->curItem <= 0) menu->curItem=menu->nItems-1; else menu->curItem--;
      lcd.clear();
    } else if (key==btnLEFT && !menu->root) {
      done=1;
    } else if (key==btnRIGHT) {
    }
  }
  lcd.clear();
}
