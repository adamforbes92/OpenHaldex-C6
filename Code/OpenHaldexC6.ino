/*
OpenHaldex-C6 - Forbes Automotive
Haldex Controller for Gen1, Gen2 and Gen4 Haldex Controllers.  Supports WiFi.

Version: 1.00.

Versions in '_ver.ino'
*/

#include <OpenHaldexC6_defs.h>

static twai_handle_t twai_bus_0;
static twai_handle_t twai_bus_1;

uint32_t rxtxcount = 0;  // frame counter
uint32_t stackCHS = 0;
uint32_t stackHDX = 0;

Preferences pref;

Freenove_ESP32_WS2812 strip = Freenove_ESP32_WS2812(1, led_gpio, led_channel, TYPE_RGB);  // 1 led, gpio pin, channel, type of LED
TickTwo tickUpdateLED(updateLED, 8000);                                                   // timer for updating LED colour (in 'IO.ino')
TickTwo tickBroadcastOpenHaldex(broadcastOpenHaldex, 200);                                // timer for broadcasting speed over CAN.  Adjustable in '_can.ino' for address to broadcast to
TickTwo tickHaldexState(showHaldexState, 1000);                                           // timer for updating Serial Monitor / General Feedback
TickTwo tickEEP(writeEEP, eepRefresh);           // timer to update/save current EEP values
TickTwo tickWiFi(disconnectWifi, wifiDisable);   // timer for disconnecting wifi after 30s if no connections - saves power
buttonClass btnMode(gpio_mode, 0, true);
buttonClass btnModeExt(gpio_mode_ext, 0, true);

// Gen 1 Standalone Frames Timers:
TickTwo Gen1_tickFrames10(Gen1_frames10, 10);        // timer broadcasting speed over CAN.  Adjustable in '_can.ino' for address to broadcat to
TickTwo Gen1_tickFrames20(Gen1_frames20, 20);        // timer broadcasting speed over CAN.  Adjustable in '_can.ino' for address to broadcat to
TickTwo Gen1_tickFrames25(Gen1_frames25, 25);        // timer broadcasting speed over CAN.  Adjustable in '_can.ino' for address to broadcat to
TickTwo Gen1_tickFrames100(Gen1_frames100, 100);     // timer broadcasting speed over CAN.  Adjustable in '_can.ino' for address to broadcat to
TickTwo Gen1_tickFrames200(Gen1_frames200, 200);     // timer broadcasting speed over CAN.  Adjustable in '_can.ino' for address to broadcat to
TickTwo Gen1_tickFrames1000(Gen1_frames1000, 1000);  // timer broadcasting speed over CAN.  Adjustable in '_can.ino' for address to broadcat to

// Gen 2 Standalone Frames Timers:
TickTwo Gen2_tickFrames10(Gen2_frames10, 10);        // timer broadcasting speed over CAN.  Adjustable in '_can.ino' for address to broadcat to
TickTwo Gen2_tickFrames20(Gen2_frames20, 20);        // timer broadcasting speed over CAN.  Adjustable in '_can.ino' for address to broadcat to
TickTwo Gen2_tickFrames25(Gen2_frames25, 25);        // timer broadcasting speed over CAN.  Adjustable in '_can.ino' for address to broadcat to
TickTwo Gen2_tickFrames100(Gen2_frames100, 100);     // timer broadcasting speed over CAN.  Adjustable in '_can.ino' for address to broadcat to
TickTwo Gen2_tickFrames200(Gen2_frames200, 200);     // timer broadcasting speed over CAN.  Adjustable in '_can.ino' for address to broadcat to
TickTwo Gen2_tickFrames1000(Gen2_frames1000, 1000);  // timer broadcasting speed over CAN.  Adjustable in '_can.ino' for address to broadcat to

// Gen 4 Standalone Frames Timers:
TickTwo Gen4_tickFrames10(Gen4_frames10, 10);        // timer broadcasting speed over CAN.  Adjustable in '_can.ino' for address to broadcat to
TickTwo Gen4_tickFrames20(Gen4_frames20, 20);        // timer broadcasting speed over CAN.  Adjustable in '_can.ino' for address to broadcat to
TickTwo Gen4_tickFrames25(Gen4_frames25, 25);        // timer broadcasting speed over CAN.  Adjustable in '_can.ino' for address to broadcat to
TickTwo Gen4_tickFrames100(Gen4_frames100, 100);     // timer broadcasting speed over CAN.  Adjustable in '_can.ino' for address to broadcat to
TickTwo Gen4_tickFrames200(Gen4_frames200, 200);     // timer broadcasting speed over CAN.  Adjustable in '_can.ino' for address to broadcat to
TickTwo Gen4_tickFrames1000(Gen4_frames1000, 1000);  // timer broadcasting speed over CAN.  Adjustable in '_can.ino' for address to broadcat to

void setup() {
#if enableDebug || detailedDebug || detailedDebugCAN
  Serial.begin(115200);
#endif
  DEBUG("OpenHaldex-C6");

  setupIO();
  setupTickers();

  connectWifi();                       // enable / start WiFi
  setupUI();                           // setup wifi user interface

  xTaskCreate(parseCAN_chs, "parseChassis", 2048, NULL, 5, NULL);  // create a task for FreeRTOS for incoming chassis CAN
  xTaskCreate(parseCAN_hdx, "parseHaldex", 2048, NULL, 4, NULL);   // create a task for FreeRTOS for incoming haldex CAN
}

void loop() {
  //updateWiFi();  // parse WiFi
  //parseTriggers(); // buttons / external mode change /

  updateTickers();
  uint32_t alerts_triggered;
  twai_read_alerts(&alerts_triggered, pdMS_TO_TICKS(0));
  twai_status_info_t twaistatus;
  twai_get_status_info(&twaistatus);
  if (alerts_triggered & TWAI_ALERT_BUS_OFF) {
    isBusFailure = true;
  }
  if (alerts_triggered & TWAI_ALERT_BUS_ERROR) {
    isBusFailure = true;
  }
  if (alerts_triggered & TWAI_ALERT_RX_QUEUE_FULL) {
    isBusFailure = true;
  }
}