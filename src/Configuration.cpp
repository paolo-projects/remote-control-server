#include "Configuration.h"

Configuration::Configuration()
{
  EEPROM.begin(EEPROM_TOTAL_SIZE);
  reload();
}

String Configuration::getBSSID() const
{
  return valid ? WLAN_BSSID : "";
}

String Configuration::getPass() const
{
  return valid ? PASS : "";
}

bool Configuration::updateConfig(String WLAN_BSSID, String PASS)
{
  bool result = writeStringToEEPROM(WLAN_BSSID, 0);
  if (result == false)
    return false;
  result = writeStringToEEPROM(PASS, EEPROM_STR_SIZE);
  if (result == false)
    return false;

  uint16_t checksum = 0;
  for (size_t i = 0; i < WLAN_BSSID.length(); i++)
    checksum += WLAN_BSSID[i];
  for (size_t i = 0; i < PASS.length(); i++)
    checksum += PASS[i];

  EEPROM.write(EEPROM_TOTAL_SIZE - 2, (checksum >> 8) & 0xFF);
  EEPROM.write(EEPROM_TOTAL_SIZE - 1, checksum & 0xFF);

  result = EEPROM.commit();
  if (result == false)
    return false;

  valid = true;
  this->WLAN_BSSID = WLAN_BSSID;
  this->PASS = PASS;
  return true;
}

void Configuration::reload()
{
  char buffer[EEPROM_TOTAL_SIZE];
  for (size_t i = 0; i < EEPROM_TOTAL_SIZE; i++)
  {
    buffer[i] = EEPROM.read(i);
  }
  // Set the bytes at STR_SIZE to 0 to avoid buffer overflow
  buffer[EEPROM_STR_SIZE - 1] = 0;
  buffer[EEPROM_TOTAL_SIZE - 3] = 0;

  uint16_t chks = buffer[EEPROM_TOTAL_SIZE - 2] << 8 | buffer[EEPROM_TOTAL_SIZE - 1];
  uint16_t calc_chks = 0;

  String bssid_ = String(buffer);
  String pass_ = String(buffer + EEPROM_STR_SIZE);

  for (size_t i = 0; i < bssid_.length(); i++)
    calc_chks += bssid_[i];
  for (size_t i = 0; i < pass_.length(); i++)
    calc_chks += pass_[i];

  if (chks == calc_chks && bssid_.length() > 0)
  {
    valid = true;
    this->WLAN_BSSID = bssid_;
    this->PASS = pass_;
  }
  else
  {
    valid = false;
  }
}

bool Configuration::writeStringToEEPROM(String str, size_t baseAddr)
{
  size_t str_len = str.length();
  if (str_len > EEPROM_STR_SIZE - 1)
    return false;
  const char *str_c = str.c_str();
  for (size_t i = 0; i < str_len + 1; i++)
  {
    EEPROM.write(baseAddr + i, str_c[i]);
  }
  return true;
}

bool Configuration::isValid() const
{
  return valid;
}
