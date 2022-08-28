#ifndef rtc_h

#define rtc_h
#include "Arduino.h"
#include <RTClib.h>

// the #include statement and code go here...
#define relay1 12

// Initialize rtc variable and tell sketch to use it on every links
extern RTC_DS3231 rtc;

void rtcSetup();
void rtcFunction();

#endif