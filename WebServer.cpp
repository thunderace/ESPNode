#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266mDNS.h>

#include <errno.h>

#include "Settings.h"
#include "WebServer.h"
#include "NTPClient.h"

ESP8266WebServer WebServerClass::_server;

const char TEXT_PLAIN[] PROGMEM = "text/plain";
const char TEXT_HTML[] PROGMEM = "text/html";
const char APPLICATION_JSON[] PROGMEM = "application/json";

WebServerClass WebServer;

#include "settings_html.h"  // Not really a header file, but Arduino build barfs with other extensions
#include "basicsetup_html.h"  // Ditto

void WebServerClass::handleNotFound() {
  _server.send_P(404, TEXT_PLAIN, PSTR("Invalid webpage"));
}

void WebServerClass::handleBasicSetup() {
  _server.send_P(200, TEXT_HTML, (const char*)basicsetup_html);
}

void WebServerClass::handleNetConfig() {
  if (!_server.hasArg("ssid") || !_server.hasArg("password")) {
    _server.send_P(400, TEXT_PLAIN, PSTR("Missing fields"));
    return;
  }

  String ssid = _server.arg("ssid"),
         password = _server.arg("password");
  if (ssid.length() > sizeof(Settings.ssid) - 1 ||
      password.length() > sizeof(Settings.password) - 1) {
    _server.send_P(400, TEXT_PLAIN, PSTR("Fields too long"));
    return;
  }

  strcpy(Settings.ssid, ssid.c_str());
  strcpy(Settings.password, password.c_str());
  
  Settings.save();
  _server.send_P(200, TEXT_PLAIN, PSTR("Settings stored; please power cycle."));
}

void WebServerClass::handleSettingsHtml() {
  _server.send_P(200, TEXT_HTML, (const char*)settings_html);
}

void WebServerClass::handleApiSettings() {
  StaticJsonBuffer<1024> _json;
  JsonObject &s = _json.createObject();

  s["ssid"] = Settings.ssid;
  s["password"] = Settings.password;
  s["hostname"] = Settings.hostname;
  s["ntpserver"] = Settings.ntpServer;
  s["utcoffset"] = Settings.utcOffset;
  s["daylight"] = Settings.daylightSaving;
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
  s["curtime"] = curtime;
  // printTo(String) does not seem to work?
  //String data;
  //s.printTo(data);
  char json_str[1024];
  s.printTo(json_str, sizeof(json_str));
  _server.send(200, "application/json", json_str);
}

void WebServerClass::handleApiUpdate() {
    bool updateTime = false;
    bool showMessage = false;

  // Check if JSON payload is present
  if (!_server.hasArg("plain")) {
    _server.send_P(400, TEXT_PLAIN, PSTR("Missing JSON payload"));
    return;
  }

  //StaticJsonBuffer<2048> _json;  // Size 512 failed, although it was sufficient to construct object above
  DynamicJsonBuffer _json;
  JsonObject &s = _json.parseObject(_server.arg("plain"));

  if (!s.success()) {
    _server.send_P(400, TEXT_PLAIN, PSTR("JSON parse error"));
    return;
  }

  // Check that UTC offset is valid
  int utc_offset = s["utcoffset"].as<int>();
  if (utc_offset < -12 || utc_offset > 12) {
    _server.send_P(400, TEXT_PLAIN, PSTR("Invalid UTC offset value"));
    return;
  }

  // Check length of string arguments (except device hostnames)
  String ssid = s["ssid"].as<String>();
  String password = s["password"].as<String>();
  String hostname = s["hostname"].as<String>();
  String ntpServer = s["ntpserver"].as<String>();
  if (ssid.length() > sizeof(Settings.ssid) - 1 ||
       password.length() > sizeof(Settings.password) - 1 ||
       ntpServer.length() > sizeof(Settings.ntpServer) - 1 ||
       hostname.length() > sizeof(Settings.hostname) - 1 ) {
      _server.send_P(400, TEXT_PLAIN, PSTR("Argument too long"));
      return;
   }
  // test changes
  if (Settings.utcOffset != (int16_t)utc_offset || strcmpi(Settings.ntpServer, ntpServer.c_str()) != 0 ||
      Settings.daylightSaving != (unsigned char)s["daylight"].as<int>()) {
    // here we have to start again the NTP client 
    updateTime = true;
  }
  if (strcmpi(Settings.ssid, ssid.c_str()) != 0 || strcmpi(Settings.password, password.c_str())) {
    showMessage = true;
  }
  // Update and save settings
  strcpy(Settings.ssid, ssid.c_str());
  strcpy(Settings.password, password.c_str());
  strcpy(Settings.hostname, hostname.c_str());
  strcpy(Settings.ntpServer, ntpServer.c_str());
  Settings.utcOffset = (int16_t)utc_offset;
  Settings.daylightSaving = (unsigned char)s["daylight"].as<int>();

  Settings.save();
  if (updateTime == true) {
    NTPClient.startRequest();
  }
  if (showMessage == true) {
    _server.send_P(200, APPLICATION_JSON, PSTR("{ \"showMessage\": true }"));
  } else {
    _server.send_P(200, APPLICATION_JSON, PSTR("{ \"showMessage\": false }"));
  }
}

void WebServerClass::begin() {
  if (WiFi.status() == WL_CONNECTED) {
    _server.on("/", handleSettingsHtml);
    _server.on("/api/settings", HTTP_GET, handleApiSettings);
    _server.on("/api/update", HTTP_POST, handleApiUpdate);
  } else {
    // Internet is not available, cannot get Bootstrap, etc
    // so fall back to a very basic webpage
    _server.on("/", handleBasicSetup);
    _server.on("/netconfig", handleNetConfig);    
  }
  _server.onNotFound(handleNotFound);

  _server.begin();

  // Start mDNS responder and register web interface
  if (WiFi.status() == WL_CONNECTED && MDNS.begin(Settings.hostname)) {
    MDNS.addService("http", "tcp", 80);
  }
}

void WebServerClass::loop() {
  _server.handleClient();
}

