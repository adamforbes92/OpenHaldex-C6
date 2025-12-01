/*
OpenHaldex-C6 - Forbes Automotive
Haldex Controller for Gen1, Gen2 and Gen4 Haldex Controllers.  Supports WiFi.  Version: 1.03.  Versions in '_ver.ino'

Codebase derived from OpenHaldex 4.0 - CAN data is the same, just ported to ESP32 C6.
*/

#include <OpenHaldexC6_defs.h>

// for EEP
Preferences pref;  // for EEPROM / storing settings

// for LED
Freenove_ESP32_WS2812 strip = Freenove_ESP32_WS2812(1, gpio_led, led_channel, TYPE_RGB);  // 1 led, gpio pin, channel, type of LED

// for mode changing (buttons & external inputs)
InterruptButton btnMode(gpio_mode, HIGH, GPIO_MODE_INPUT, 1000, 500, 750, 80000);          // pin, GPIO_MODE_INPUT, state when pressed, long press, autorepeat, double-click, debounce
InterruptButton btnMode_ext(gpio_mode_ext, HIGH, GPIO_MODE_INPUT, 1000, 500, 750, 80000);  // pin, GPIO_MODE_INPUT, state when pressed, long press, autorepeat, double-click, debounce

void setup() {
#if enableDebug || detailedDebug || detailedDebugCAN || detailedDebugWiFi || detailedDebugEEP
  Serial.begin(500000);  // if ANY Serial is required, begin
  DEBUG("OpenHaldex-C6 Launching...");
#endif

  readEEP();       // read EEPROM for stored settings  - in '_EEP.ino'
  setupIO();       // setup gpio for input / output  - in '_io.ino'
  setupCAN();      // setup two CAN buses  - in '_io.ino'
  setupButtons();  // setup  'buttons' for changing mode (internal and external) - in '_io.ino'
  setupTasks();    // setup tasks for each of the main functions - CAN Chassis/Haldex handling, Serial prints, Standalone, etc - in '_io.ino'

  connectWifi();  // enable / start WiFi - in '_wifi.ino'
  setupUI();      // setup wifi user interface - in '_wifi.ino'
}

void loop() {
  delay(100); // literally here to give more CPU time to tasks
}