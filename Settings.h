#ifndef ESPClock_Settings_h
#define ESPClock_Settings_h

#include <IPAddress.h>
#include <EEPROM.h>

#define FLASH_MAGIC_R1 0x5aa5e126
#define FLASH_MAGIC 0x5aa5e147

struct SettingsClass {
  uint32_t magic;
  char ssid[32];  // ESP seems to have a 31-char limit
  char password[64];
  char hostname[32];  // For mDNS; *not* FQDN
  char ntpServer[64];
  int16_t utcOffset;  // in hours
  unsigned char daylightSaving;
  void begin();
  void save();
};

extern SettingsClass Settings;

#endif  // ESPClock_Settings_h

