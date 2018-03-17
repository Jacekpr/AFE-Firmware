/* AFE Firmware for smart home devices
  LICENSE: https://github.com/tschaban/AFE-Firmware/blob/master/LICENSE
  DOC: http://smart-house.adrian.czabanowski.com/afe-firmware-pl/ */

#include "AFE-Defaults.h"

AFEDefaults::AFEDefaults() {}

void AFEDefaults::set() {

  AFEDataAccess *Data;

  DEVICE deviceConfiguration;
  FIRMWARE firmwareConfiguration;
  NETWORK networkConfiguration;
  MQTT MQTTConfiguration;
  RELAY RelayConfiguration;
  REGULATOR RegulatorConfiguration;
  SWITCH SwitchConfiguration;
  LED LEDConfiguration;
  DS18B20 DS18B20Configuration;

  sprintf(firmwareConfiguration.version, FIRMWARE_VERSION);
  firmwareConfiguration.type = FIRMWARE_TYPE;
  firmwareConfiguration.autoUpgrade = 0;
  sprintf(firmwareConfiguration.upgradeURL, "");
  Data->saveConfiguration(firmwareConfiguration);

  sprintf(deviceConfiguration.name, "AFE-Device");
  deviceConfiguration.isLED[0] = true;
  deviceConfiguration.isLED[1] = false;
  deviceConfiguration.isRelay[0] = true;
  deviceConfiguration.isSwitch[0] = true;
  deviceConfiguration.isSwitch[1] = false;
  deviceConfiguration.isDS18B20 = false;
  deviceConfiguration.mqttAPI = false;
  deviceConfiguration.httpAPI = true;
  Data->saveConfiguration(deviceConfiguration);

  sprintf(networkConfiguration.ssid, "");
  sprintf(networkConfiguration.password, "");
  networkConfiguration.isDHCP = true;
  networkConfiguration.ip = IPAddress(0, 0, 0, 0);
  networkConfiguration.gateway = IPAddress(0, 0, 0, 0);
  networkConfiguration.subnet = IPAddress(255, 255, 255, 0);
  networkConfiguration.noConnectionAttempts = 10;
  networkConfiguration.waitTimeConnections = 1;
  networkConfiguration.waitTimeSeries = 60;
  Data->saveConfiguration(networkConfiguration);

  sprintf(MQTTConfiguration.host, "");
  MQTTConfiguration.ip = IPAddress(0, 0, 0, 0);
  sprintf(MQTTConfiguration.user, "");
  sprintf(MQTTConfiguration.password, "");
  MQTTConfiguration.port = 1883;
  // sprintf(MQTTConfiguration.topic, "/device/");
  sprintf(MQTTConfiguration.topic, "/device/");
  Data->saveConfiguration(MQTTConfiguration);

  RelayConfiguration.gpio = 12;
  RelayConfiguration.timeToOff = 0;
  RelayConfiguration.statePowerOn = 3;
  RelayConfiguration.stateMQTTConnected = 0;
  sprintf(RelayConfiguration.name, "switch");
  RelayConfiguration.ledID = 0;

  RelayConfiguration.thermalProtection = 0;
  Data->saveConfiguration(0, RelayConfiguration);

  RegulatorConfiguration.enabled = false;
  RegulatorConfiguration.turnOn = 0;
  RegulatorConfiguration.turnOnAbove = false;
  RegulatorConfiguration.turnOff = 0;
  RegulatorConfiguration.turnOffAbove = true;
  Data->saveConfiguration(RegulatorConfiguration);

  SwitchConfiguration.gpio = 0;
  SwitchConfiguration.type = 0;
  SwitchConfiguration.sensitiveness = 50;
  SwitchConfiguration.functionality = 0;
  SwitchConfiguration.relayID = 1;
  Data->saveConfiguration(0, SwitchConfiguration);

  SwitchConfiguration.gpio = 5;
  SwitchConfiguration.type = 1;
  SwitchConfiguration.functionality = 1;
  Data->saveConfiguration(1, SwitchConfiguration);

  LEDConfiguration.gpio = 13;
  LEDConfiguration.changeToOppositeValue = false;
  Data->saveConfiguration(0, LEDConfiguration);

  LEDConfiguration.gpio = 3;
  Data->saveConfiguration(1, LEDConfiguration);

  DS18B20Configuration.gpio = 14;
  DS18B20Configuration.correction = 0;
  DS18B20Configuration.interval = 60;
  DS18B20Configuration.unit = 0;
  DS18B20Configuration.sendOnlyChanges = true;

  Data->saveConfiguration(DS18B20Configuration);

  Data->saveSystemLedID(1);

  Data->saveDeviceMode(2);
  Data->saveRelayState(0, false);
  Data->saveLanguage(1);
}

void AFEDefaults::eraseConfiguration() { Eeprom.erase(); }
