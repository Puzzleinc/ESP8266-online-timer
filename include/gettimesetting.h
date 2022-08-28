#ifndef gettimesetting_h

#define gettimesetting_h
#include "Arduino.h"
#include <ArduinoJson.h>
#include <LittleFS.h>

// the #include statement and code go here...
extern unsigned int jamawal;
extern unsigned int jamakhir;

// Initialize rtc variable and tell sketch to use it on every links
void getTimesetting();

#endif