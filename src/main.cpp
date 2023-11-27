#include <Arduino.h>
#include <STM32FreeRTOS.h>

#include <Debugger.h>
#include <i2c_EEPROM.h>
#include <Handler.h>
#include <SerialHandler.h>
#include <WatchdogHandler.h>

void setup() {
  SerialHandler serial;
  WatchdogHandler watchdog;

  serial.start("serial handler", 1024, 1); // On démarre le port série de debug / configuration
  watchdog.start("watchdog", 256, 1); // On démarre le watchdog en tache de fond
}

void loop() {
  // put your main code here, to run repeatedly:
}