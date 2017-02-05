/*
 * edit.h - 
 * 
 * Released into the public domain
 */

#ifndef EDIT_H
#define EDIT_H

extern int editList(int *pVal, char **values, int x, int y);
extern int editUnsigned(int *pVal, int min, int max, int x, int y);
extern int editUnsignedChar(char *pVal, int min, int max, int x, int y);
extern void setDateAndTime();

#endif

