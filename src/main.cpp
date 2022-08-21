#include <Arduino.h>

#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include <ArduinoJson.h>
#include <AsyncJson.h>

#include <Wire.h>
#include <LittleFS.h>

#include "wificonnect.h"
#include "rtc.h"

int jamawal;
int jamakhir;

// Define millis variable
const unsigned long eventInterval = 2000;
unsigned long previousTime = 0;

// Define Wire I2c pin
#define SDA_PIN 4
#define SCL_PIN 5

// Define peripehalps pin
#define relay1 12

/* Put your SSID & Password */
const char* ssid;  // Enter SSID here
const char* password;  //Enter Password here

// Initialize server
AsyncWebServer server(80);
// Make variable prototype
void handleRequest(AsyncWebServerRequest *request);
void getData(AsyncWebServerRequest *request);
void onFailed(AsyncWebServerRequest *request);
void getTimesetting();

void setup() {
  // Initialize Serial port and I2C pin -----------------------
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);
  while (!Serial); // wait for serial port to connect. Needed for native USB

  // Initialize peripehalps pin -----------------------
  pinMode(relay1, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  digitalWrite(relay1, LOW);

// Initialize filesystem -----------------------
  if(!LittleFS.begin()){
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }

  // Initialize rtc variable via function -----------------------
  rtcSetup();
  getTimesetting();

  //  Connecting to wifi -----------------------
  ssid = "Puzzle24";  // Enter SSID here
  password = "gzcmb94463";  //Enter Password here
  wificonnect(ssid, password);

  //  Setting up control allow origin on server -----------------------
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");

  //  Creating handle request from client -----------------------
  server.on("/", HTTP_ANY, handleRequest);
  server.on("/getdata", HTTP_POST, getData);
  server.on("/post", HTTP_POST,[](AsyncWebServerRequest * request){},NULL,[](AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total) {

    String json = "";
    for (size_t i = 0; i < len; i++) {
      json += (char)data[i];
    }

    File this_file = LittleFS.open("/config.json", "w");
    if (!this_file) { // failed to open the file, return false
      const char* openfail = "Failed to open file for reading";
      request->send(200, "text/plain", openfail);
      return;
    }
    int bytesWritten = this_file.print(json);

    if (bytesWritten == 0) { // write failed
      const char *writefail = "Gagal menulis data";
      request->send(200, "text/plain", writefail);
      return;
    }

    const char *writedone = "Berhasil menulis data, resetting device..";
    request->send(200, "text/plain", writedone);
    this_file.close();
    getTimesetting();
    delay(1000);
    //ESP.restart(); //ESP.reset();    
  });

  //send the files request by the browser that are contained in the SPIFFS Filesystem
  //This means we don't have to deal with mime types of files requested.
  server.serveStatic("/", LittleFS, "/");
  server.onNotFound(onFailed);
  server.begin();
}

void loop() {
    unsigned long currentTime = millis();

  // Millis Function
  if (currentTime - previousTime >= eventInterval) {
    /* Event code */
    rtcFunction();
    
   /* Update the timing for the next time around */
    previousTime = currentTime;
  }

  DateTime now = rtc.now();

  if(now.hour() >= jamawal && now.hour() <= jamakhir-1) {
      digitalWrite(relay1, HIGH); // sets the digital pin 13 on
  } else {
      digitalWrite(relay1, LOW);
  }
}

void handleRequest(AsyncWebServerRequest *request){
  request->send(LittleFS, "/index.html", String(), false);
}

void getData(AsyncWebServerRequest *request){
  // Get all data from RTC and SPIFFS, convert it to JSON array and send to client request
  DateTime now = rtc.now();
  StaticJsonDocument<96> doc;

  JsonArray sensorrtc = doc.createNestedArray("sensorrtc");
  sensorrtc.add(now.hour());
  sensorrtc.add(now.minute());

  JsonArray looptime = doc.createNestedArray("looptime");
  looptime.add(jamawal);
  looptime.add(jamakhir);

  String output;
  serializeJson(doc, output);

  request->send(200, "application/json", output);
}

void onFailed(AsyncWebServerRequest *request){
  //Handle Unknown Request
  request->send(404, "text/plain", "Failed to reach file");
}

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