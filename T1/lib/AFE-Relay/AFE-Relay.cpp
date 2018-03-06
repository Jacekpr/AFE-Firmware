/* AFE Firmware for smart home devices
  LICENSE: https://github.com/tschaban/AFE-Firmware/blob/master/LICENSE
  DOC: http://smart-house.adrian.czabanowski.com/afe-firmware-pl/ */

#include "AFE-Relay.h"

AFERelay::AFERelay(){};

AFERelay::AFERelay(uint8_t id) { begin(id); }

void AFERelay::begin(uint8_t id) {
  MQTT MQTTConfiguration;
  MQTTConfiguration = Data.getMQTTConfiguration();
  _id = id;
  RelayConfiguration = Data.getRelayConfiguration(_id);
  pinMode(RelayConfiguration.gpio, OUTPUT);
  sprintf(mqttTopic, "%s%s/", MQTTConfiguration.topic, RelayConfiguration.name);
  /* Initialzing Thermostat functionality for a relay */
  Thermostat.begin(_id, RelayConfiguration.thermostat);
  /* Initialzing thermal protection functionality for a relay */
  ThermalProtection.begin(RelayConfiguration.thermalProtection);
  if (RelayConfiguration.ledID > 0) {
    Led.begin(RelayConfiguration.ledID - 1);
  }
}

const char *AFERelay::getMQTTTopic() { return RelayConfiguration.mqttTopic; }

byte AFERelay::get() {
  return digitalRead(RelayConfiguration.gpio) == HIGH ? RELAY_ON : RELAY_OFF;
}

/* Set relay to ON */
void AFERelay::on() {
  if (get() == RELAY_OFF) {
    digitalWrite(RelayConfiguration.gpio, HIGH);
    Led.on();
    if (RelayConfiguration.timeToOff >
        0) { // Start counter if relay should be automatically turned off
      turnOffCounter = millis();
    }
  }
  Data.saveRelayState(_id, RELAY_ON);
}

/* Set relay to OFF */
void AFERelay::off() {
  if (get() == RELAY_ON) {
    digitalWrite(RelayConfiguration.gpio, LOW);
    Led.off();
  }
  Data.saveRelayState(_id, RELAY_OFF);
}

/* Toggle relay */
void AFERelay::toggle() {
  if (digitalRead(RelayConfiguration.gpio) == LOW) {
    on();
  } else {
    off();
  }
}

void AFERelay::setRelayAfterRestoringPower() {
  setRelayAfterRestore(RelayConfiguration.statePowerOn);
}

boolean AFERelay::setRelayAfterRestoringMQTTConnection() {
  if (RelayConfiguration.stateMQTTConnected ==
      5) { // request state from MQTT Broker
    return false;
  } else {
    setRelayAfterRestore(RelayConfiguration.stateMQTTConnected);
    return true;
  }
}

void AFERelay::setRelayAfterRestore(uint8_t option) {

  if (option == 1) {
    off();
  } else if (option == 2) {
    on();
  } else if (option == 3) {
    Data.getRelayState(_id) == RELAY_ON ? on() : off();
  } else if (option == 4) {
    Data.getRelayState(_id) == RELAY_ON ? off() : on();
  }
}

boolean AFERelay::autoTurnOff() {
  if (RelayConfiguration.timeToOff > 0 && get() == RELAY_ON &&
      millis() - turnOffCounter >= RelayConfiguration.timeToOff * 1000) {
    off();
    return true;
  } else {
    return false;
  }
}

const char *AFERelay::getName() { return RelayConfiguration.name; }
