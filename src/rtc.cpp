#include <Arduino.h>
#include "rtc.h"

// Initialize rtc variable
RTC_DS3231 rtc;

void rtcSetup() {
      // Initialize rtc module -----------------------
    if (! rtc.begin()) {
        Serial.println("Couldn't find RTC");
        Serial.flush();
        while (1) delay(10);
    }
}

void rtcFunction() {
    // Ouuput data to the serial port for debug -----------------------
    DateTime now = rtc.now();

    digitalWrite(LED_BUILTIN, 0);
    delay(100);
    digitalWrite(LED_BUILTIN, 1);

    Serial.print(now.day(), DEC);
    Serial.print(" - ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
}