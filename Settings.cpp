#include <Arduino.h>
#include <EEPROM.h>

#include <ESP8266WiFi.h>
#include "Settings.h"
#include "utils.h"

SettingsClass Settings;

static const size_t EEPROM_SIZE = 512;

void SettingsClass::begin() {
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.get(0, *this);
  // Populate defaults if flash magic is unexpected
  if (magic != FLASH_MAGIC) {
    if (magic != FLASH_MAGIC_R1) {
      bzero(this, sizeof(*this));
      strcpy(ssid, "YOUR_SSID");
      strcpy(password, "YOUR_PASSWORD");
      strcpy(hostname, getAPSSID().c_str());
      strcpy(ntpServer, "fr.pool.ntp.org");
      utcOffset = 1; 
      daylightSaving = 0; 
    }
  }
}

void SettingsClass::save() {
  EEPROM.put(0, *this);
  EEPROM.commit();
}

