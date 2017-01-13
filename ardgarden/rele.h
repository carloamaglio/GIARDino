/*
 * rele.h - 
 * 
 * Released into the public domain
 */

#ifndef RELE_H
#define RELE_H

#include <arduino.h>

class Rele {
  public:
    Rele();
    void setAddr(int addr, int state=0);
    void setState(int state);
    int getState();
  private:
    char state;
    int addr;
};

#endif

