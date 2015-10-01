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

static const char DEFAULT_AP_PASSWORD[] = "esp";
static const char WIFI_CONNECT_TIMEOUT = 10;  // in seconds
static const time_t NTP_SYNC_INTERVAL = 3600;  // in seconds

static void update_time(bool force = false) {
  static time_t last_update = 0;
  static char buf[12];
  static char led_state = LOW;
  time_t time_now = now();
  if ((time_now != last_update && (time_now % 60) == 0) || force) {
    int hr = hour();
    sprintf(buf, "%02d:%02d", hr, minute());
    Serial.print("New time: "); Serial.println(buf);  Serial.flush();  // DEBUG
    last_update = time_now;
  }
}

static void connect_wifi() {
  if (Settings.ssid[0] != '\0') {
    WiFi.hostname(Settings.hostname);
    WiFi.begin(Settings.ssid, Settings.password);
    int s = 0;
    while ((WiFi.status() != WL_CONNECTED) && (s < 2*WIFI_CONNECT_TIMEOUT)) {
      delay(500);  // Must match while condition above
      ++s;
    }
  }

  if (WiFi.status() != WL_CONNECTED) {
    // Fall back to soft-AP mode
    WiFi.softAP(Settings.hostname, DEFAULT_AP_PASSWORD);
  }
}

static void time_cb(time_t time) {
  setTime(time);
  update_time(true);
}


void setup() {
  Serial.begin(115200);
  Serial.println("**** START ****");
  Settings.begin();
  delay(200);
  connect_wifi();

  if (WiFi.status() == WL_CONNECTED) {
    NTPClient.begin(time_cb);
    NTPClient.startRequest();
  }
  WebServer.begin();
}

void loop() {
  WebServer.loop();
  update_time();

  if (WiFi.status() == WL_CONNECTED) {
    NTPClient.loop();

    static time_t last = 0;
    time_t t_now = now();
    if (t_now - last > NTP_SYNC_INTERVAL) {
      NTPClient.startRequest();
      last = t_now;
    }
  }
}

