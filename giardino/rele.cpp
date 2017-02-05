/*
 * rele.cpp - control a single rele. It can be local or (to be developed) remoted to an I2C expansion device
 *  Carlo Amaglio, Via Emigli 10 - 25081 Bedizzole (Italy)
 * 
 * Released into the public domain
 */

#include <arduino.h>
#include "rele.h"

#define ACTIVELOW 1

Rele::Rele() {
}

void Rele::setAddr(int addr, int state=0) {
  this->addr = addr;
  pinMode(addr, OUTPUT);
  setState(state);
}

void Rele::setState(int state) {
  digitalWrite(addr, state^ACTIVELOW?HIGH:LOW);
  this->state = state;
}

int Rele::getState() {
  return state;
}


