/* AFE Firmware for smart home devices
  LICENSE: https://github.com/tschaban/AFE-Firmware/blob/master/LICENSE
  DOC: http://smart-house.adrian.czabanowski.com/afe-firmware-pl/ */

#include "AFE-MQTT.h"
#include <AFE-Configuration.h>
#include <AFE-Data-Access.h>
#include <AFE-Device.h>
#include <AFE-LED.h>
#include <AFE-Relay.h>
#include <AFE-Sensor-DS18B20.h>
#include <AFE-Switch.h>
#include <AFE-Upgrader.h>
#include <AFE-Web-Server.h>
#include <AFE-WiFi.h>
#include <Streaming.h>

AFEDataAccess Data;
AFEDevice Device;
AFEWiFi Network;
AFEMQTT Mqtt;
AFEWebServer WebServer;
AFELED Led;
AFESwitch Switch;
AFESwitch ExternalSwitch;
AFERelay Relay;
AFESensorDS18B20 SensorDS18B20;
MQTT MQTTConfiguration;

float temperature;

void setup() {

  Serial.begin(115200);
  delay(10);

  /* Turn off publishing information to Serial */
  Serial.swap();

  /* Checking if the device is launched for a first time. If so it sets up
   * the device (EEPROM) */
  if (Device.isFirstTimeLaunch()) {
    Device.setDevice();
  }

  /* Perform post upgrade changes (if any) */
  AFEUpgrader Upgrader;
  if (Upgrader.upgraded()) {
    Upgrader.upgrade();
  }
  Upgrader = {};

  /* Checking if WiFi is onfigured, if not than it runs access point mode */
  if (Device.getMode() != MODE_ACCESS_POINT && !Device.isConfigured()) {
    Device.reboot(MODE_ACCESS_POINT);
  }

  /* Initializing relay and setting it's default state at power on*/
  if (Device.getMode() == MODE_NORMAL && Device.configuration.isRelay[0]) {
    Relay.begin(0);
    Relay.setRelayAfterRestoringPower();
  }

  /* Initialzing network */
  Network.begin(Device.getMode());

  /* Initializing LED, checking if LED exists is made on Class level  */
  Led.begin(0);

  /* If device in configuration mode then start LED blinking */
  if (Device.getMode() == MODE_ACCESS_POINT) {
    Led.blinkingOn(100);
  }

  /* Initializing Switches */
  if (Device.configuration.isSwitch[0]) {
    Switch.begin(0);
  }
  if (Device.configuration.isSwitch[1]) {
    ExternalSwitch.begin(1);
  }

  /* Initializing sensor: DS18B20 */
  if (Device.configuration.isDS18B20) {
    SensorDS18B20.begin();
  }

  /* Initializing MQTT */
  if (Device.getMode() != MODE_ACCESS_POINT && Device.configuration.mqttAPI) {
    MQTTConfiguration = Data.getMQTTConfiguration();
    Mqtt.begin();
  }

  Network.listener();

  /* Initializing HTTP WebServer */
  WebServer.handle("/", handleHTTPRequests);
  WebServer.handle("/favicon.ico", handleFavicon);
  WebServer.begin();
}

void loop() {

  if (Device.getMode() != MODE_ACCESS_POINT) {
    if (Network.connected()) {
      if (Device.getMode() == MODE_NORMAL) {
        /* Connect to MQTT if not connected */
        if (Device.configuration.mqttAPI) {
          Mqtt.listener();
        }

        WebServer.listener();

        /* Checking if there was received HTTP API Command */
        if (Device.configuration.httpAPI) {
          if (WebServer.httpAPIlistener()) {
            Led.on();
            processHTTPAPIRequest(WebServer.getHTTPCommand());
            Led.off();
          }
        }

        /* Sensor: DS18B20 related code */
        if (Device.configuration.isDS18B20) {

          /* Sensor: DS18B20 listener */
          SensorDS18B20.listener();

          if (SensorDS18B20.isReady()) {
            Led.on();
            temperature = SensorDS18B20.getLatest();

            /* Thermostat listener */
            Relay.Thermostat.listener(temperature);

            /* Thermal Protection listener */
            Relay.ThermalProtection.listener(temperature);

            /* Relay control by thermostat code */
            if (Relay.Thermostat.isReady()) {
              if (Relay.Thermostat.getRelayState() == RELAY_ON &&
                  !Relay.ThermalProtection.protectionOn()) {
                Relay.on();
              } else {
                Relay.off();
              }
              MQTTPublishRelayState();
            }

            /* Checking if relay should be switched off based on device thermal
             * protection */
            if (Relay.get() == RELAY_ON &&
                Relay.ThermalProtection.protectionOn()) {
              Relay.off();
              MQTTPublishRelayState();
            }

            /* Publishing temperature to MQTT Broker if enabled */
            Mqtt.publish("temperature", temperature);
            Led.off();
          }
        }

      } else { // Configuration Mode
        WebServer.listener();
      }
    } else { // Device not connected to WiFi. Reestablish connection
      Network.listener();
    }
  } else { // Access Point Mode
    Network.APListener();
    WebServer.listener();
  }

  /* Listens for switch events */
  Switch.listener();
  ExternalSwitch.listener();

  /* One of the Multifunction switches pressed for 10 seconds */
  if ((Switch.getFunctionality() == SWITCH_MULTI && Switch.is10s()) ||
      (ExternalSwitch.getFunctionality() == SWITCH_MULTI &&
       ExternalSwitch.is10s())) {
    Device.getMode() == MODE_NORMAL ? Device.reboot(MODE_ACCESS_POINT)
                                    : Device.reboot(MODE_NORMAL);
  }

  /* One of the Multifunction switches pressed for 5 seconds */
  if ((Switch.getFunctionality() == SWITCH_MULTI && Switch.is5s()) ||
      (ExternalSwitch.getFunctionality() == SWITCH_MULTI &&
       ExternalSwitch.is5s())) {
    Device.getMode() == MODE_NORMAL ? Device.reboot(MODE_CONFIGURATION)
                                    : Device.reboot(MODE_NORMAL);
  }

  /* Led listener */
  Led.loop();
}
