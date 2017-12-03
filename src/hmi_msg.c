#include <avr/pgmspace.h>
#ifndef HMI_MSG_C_
#define HMI_MSG_C_
const char n1[] PROGMEM = "Zero";
const char n2[] PROGMEM = "One";
const char n3[] PROGMEM = "Two";
const char n4[] PROGMEM = "Three";
const char n5[] PROGMEM = "Four";
const char n6[] PROGMEM = "Five";
const char n7[] PROGMEM = "Six";
const char n8[] PROGMEM = "Seven";
const char n9[] PROGMEM = "Eight";
const char n10[] PROGMEM = "Nine";

PGM_P const numbers[] PROGMEM = {n1, n2, n3, n4, n5, n6, n7, n8, n9, n10};

#endif
