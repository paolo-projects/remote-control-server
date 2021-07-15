#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <Arduino.h>
#include <EEPROM.h>

// We read and write the whole configuration data at once, to avoid 
// reducing the read/write cycles
// Structure:
// <BSSID:fixed char[256]><PASS:fixed char[256]><CHECKSUM: uint16_t>
// char[256] are null terminated, so the max string length is 255
// the 16-bit checksum is a very tiny layer of validation, should be improved
// using better methods

// The EEPROM size (it's emulated so we define here how much Flash memory
// we reserve for permanent storage)
#define EEPROM_TOTAL_SIZE 514
// The max string length
#define EEPROM_STR_SIZE 256

class Configuration
{
public:
  Configuration();
  String getBSSID() const;
  String getPass() const;
  bool isValid() const;
  bool updateConfig(String WLAN_BSSID, String PASS);
  void reload();
private:
  String WLAN_BSSID, PASS;
  bool valid = false;
  bool writeStringToEEPROM(String str, size_t baseAddr);
};

#endif
