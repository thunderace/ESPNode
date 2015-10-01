#include <Arduino.h>
#include <EEPROM.h>

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

#include "Settings.h"

SettingsClass Settings;

static const size_t EEPROM_SIZE = 512;

void SettingsClass::begin() {
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.get(0, *this);

  // Populate defaults if flash magic is unexpected
  if (magic != FLASH_MAGIC) {
    if (magic != FLASH_MAGIC_R1) {
      bzero(this, sizeof(*this));
/*      
      uint8_t mac[WL_MAC_ADDR_LENGTH];
      WiFi.macAddress(mac);
      sprintf(hostname, "espclock%02x", mac[WL_MAC_ADDR_LENGTH-1]);
*/      
      strcpy(ssid, "YOUR_SSID");
      strcpy(password, "YOUR_PASSWORD");
      strcpy(hostname, "ESP8266Node1");
      strcpy(ntpServer, "fr.pool.ntp.org");
      utcOffset = 1; 
      daylightSaving = 0; 
    }
    magic = FLASH_MAGIC;
    save();
  }
}

void SettingsClass::save() {
  EEPROM.put(0, *this);
  EEPROM.commit();
}

