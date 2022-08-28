#include <Arduino.h>
#include "gettimesetting.h"

// Initialize rtc variable
unsigned int jamawal;
unsigned int jamakhir;

void getTimesetting() {
    // Get data from SPIFFS and send it to global variable
    String result = "";
    File file = LittleFS.open("/config.json", "r");

    if(!file){
    Serial.println("Failed to open file for reading");
    return;
    }

    while (file.available()) {
        result += (char)file.read();
    }
    file.close();

    StaticJsonDocument<96> doc;
    DeserializationError error = deserializeJson(doc, result);

    if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
    }

    jamawal = doc["jamawal"]; // 7
    jamakhir = doc["jamakhir"]; // 17
}