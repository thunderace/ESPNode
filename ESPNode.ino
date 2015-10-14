#include <Arduino.h>
#include <EEPROM.h>
#include <Time.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

#include "Settings.h"
#include "NTPClient.h"
#include "WebServer.h"
/*
#define DEFAULT_AP_PASSWORD  "esp"
#define WIFI_CONNECT_TIMEOUT 10  // in seconds
#define NTP_SYNC_INTERVAL 3600  // in seconds
*/
static const char DEFAULT_AP_PASSWORD[] = "esp";
static const char WIFI_CONNECT_TIMEOUT = 10; // in seconds

static void print_date_time() {
  time_t t = now();
  String curtime = dayStr(weekday(t));
  curtime += " ";
  curtime += day(t);
  curtime += " ";
  curtime += monthStr(month(t));
  curtime += " ";
  curtime += year(t);
  curtime += " ";
  if (hour() < 10) {
    curtime += "0";
  }
  curtime += hour(t);
  curtime += "h";
  if (minute() < 10) {
    curtime += "0";
  }
  curtime += minute(t);
  curtime += "m";
  if (second() < 10) {
    curtime += "0";
  }
  curtime += second(t);
  curtime += "s";
  Serial.println(curtime);
}

void connect_wifi() {
  WiFi.hostname(Settings.hostname);
  WiFi.begin(Settings.ssid, Settings.password);
  int s = 0;
  while ((WiFi.status() != WL_CONNECTED) && (s < 2*WIFI_CONNECT_TIMEOUT)) {
    delay(500);  // Must match while condition above
    ++s;
  }

  if (WiFi.status() != WL_CONNECTED) {
    // Fall back to soft-AP mode
    WiFi.softAP(Settings.hostname, DEFAULT_AP_PASSWORD);
  }
}

static void time_cb(time_t time) {
  print_date_time();
}


void manage_network() {
  WiFi.hostname(Settings.hostname);
  WiFi.begin(Settings.ssid, Settings.password);
  int s = 0;
  while ((WiFi.status() != WL_CONNECTED) && (s < 2*WIFI_CONNECT_TIMEOUT)) {
    delay(500);  // Must match while condition above
    ++s;
  }

  if (WiFi.status() == WL_CONNECTED) {
    NTPClient.begin(time_cb);
  } else {
    // Fall back to soft-AP mode
    WiFi.softAP(Settings.hostname, DEFAULT_AP_PASSWORD);
  }
  WebServer.begin();
}


void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("**** START ****");
  Settings.begin();
  manage_network();
/*
  connect_wifi();

  if (WiFi.status() == WL_CONNECTED) {
    NTPClient.begin(time_cb);
    NTPClient.startRequest();
  }
  WebServer.begin();
*/  
}

void loop() {
  WebServer.loop();
  NTPClient.loop();
}

