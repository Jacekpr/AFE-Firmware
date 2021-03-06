/* AFE Firmware for smart home devices
  LICENSE: https://github.com/tschaban/AFE-Firmware/blob/master/LICENSE
  DOC: http://smart-house.adrian.czabanowski.com/afe-firmware-pl/ */

#ifndef _AFE_Data_Access_h
#define _AFE_Data_Access_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include <AFE-Data-Structures.h>
#include <AFE-EEPROM.h>
//#include <Streaming.h>

class AFEDataAccess {
private:
  AFEEEPROM Eeprom;

public:
  AFEDataAccess();

  /* Methods read configuration from EEPROM */
  DEVICE getDeviceConfiguration();
  FIRMWARE getFirmwareConfiguration();
  NETWORK getNetworkConfiguration();
  MQTT getMQTTConfiguration();
  LED getLEDConfiguration(uint8_t id);
  RELAY getRelayConfiguration(uint8_t id);
  SWITCH getSwitchConfiguration(uint8_t id);
  CONTACTRON getContactronConfiguration(uint8_t id);
  DH getDHTConfiguration();
  GATE getGateConfiguration();

  /* Methods save configuration to EEPROM */
  void saveConfiguration(DEVICE configuration);
  void saveConfiguration(FIRMWARE configuration);
  void saveConfiguration(NETWORK configuration);
  void saveConfiguration(MQTT configuration);
  void saveConfiguration(uint8_t id, LED configuration);
  void saveConfiguration(uint8_t id, RELAY configuration);
  void saveConfiguration(uint8_t id, SWITCH configuration);
  void saveConfiguration(uint8_t id, CONTACTRON configuration);
  void saveConfiguration(DH configuration);
  void saveConfiguration(GATE configuration);

  /* Methods read and save firmware version from/to EEPROM */
  const char getVersion();
  void saveVersion(String version);

  /* Methods: exists only for compatibility purpose */
  boolean getRelayState(uint8_t id);
  void saveRelayState(uint8_t id, boolean state);

  /* Methods read and save device mode from/to EEPROM */
  uint8_t getDeviceMode();
  void saveDeviceMode(uint8_t mode);

  /* Methods read and save firmware Language from/to EEPROM */
  uint8_t getLanguage();
  void saveLanguage(uint8_t language);

  /* Methods reads and saves sysyem LED */
  uint8_t getSystemLedID();
  void saveSystemLedID(uint8_t id);
};
#endif
