#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/twai.h"
#include "driver/gpio.h"
#include "Freenove_WS2812_Lib_for_ESP32.h"
#include <Preferences.h>  // for eeprom/remember settings
#include <ESPUI.h>        // included for WiFi pages
#include <WiFi.h>         // included for WiFi pages
#include <ESPmDNS.h>      // included for WiFi pages
#include <ButtonLib.h>    // included for paddles
#include "TickTwo.h"  // for repeated tasks
#include <OpenHaldexC6_canID.h>

#define enableDebug 1
#define detailedDebug 1
#define detailedDebugCAN 1
#define detailedDebugWiFi 1

#define eepRefresh 2000       // EEPROM save in ms
#define wifiDisable 60000     // turn off WiFi in ms - check for 0 connections after 60s and disable WiFi - burning power otherwise

// Debugging macros
#ifdef enableDebug
#define DEBUG(x, ...) Serial.printf(x "\n", ##__VA_ARGS__)
#define DEBUG_(x, ...) Serial.printf(x, ##__VA_ARGS__)
#else
#define DEBUG(x, ...)
#define DEBUG_(x, ...)
#endif

#define CAN0_RS 2   // can_0 slope control
#define CAN0_RX 23  // can_0 rx
#define CAN0_TX 3   // can_0 tx
#define CAN1_RS 22  // can_1 slope control
#define CAN1_RX 20  // can_1 tx
#define CAN1_TX 21  // can_1 rx

#define led_gpio 8          // gpio for led
#define led_channel 0       // channel for led
#define led_brightness 255  // default brightness

#define gpio_mode 19     // gpio mode button internal
#define gpio_mode_ext 0  // gpio mode button external

#define EXAMPLE_TAG "TWAI v2"
#define wifiHostName "OpenHaldex-C6"  // the WiFi name

// setup - main inputs
bool isMPH = false;                   // 0 = kph, 1 = mph
#define mphFactor 621371  // to convert from kmh > mph

twai_message_t rx_message_hdx; // incoming haldex message
twai_message_t rx_message_chs; // incoming chassis message
 
twai_message_t tx_message_hdx; // outgoing haldex message
twai_message_t tx_message_chs; // outgoing chassis message

void updateLED();

void Gen1_frames10();
void Gen1_frames20();
void Gen1_frames25();
void Gen1_frames100();
void Gen1_frames200();
void Gen1_frames1000();

void Gen2_frames10();
void Gen2_frames20();
void Gen2_frames25();
void Gen2_frames100();
void Gen2_frames200();
void Gen2_frames1000();

void Gen4_frames10();
void Gen4_frames20();
void Gen4_frames25();
void Gen4_frames100();
void Gen4_frames200();
void Gen4_frames1000();

void parseCAN_chs();
void parseCAN_hdx();
void broadcastOpenHaldex();
void showHaldexState();

void setupTickers();
void updateTickers();

void get_lock_data(twai_message_t &rx_message_chs);

// Values received from Haldex CAN
uint8_t received_haldex_state;
uint8_t received_haldex_engagement;
bool received_report_clutch1;
bool received_report_clutch2;
bool received_temp_protection;
bool received_coupling_open;
bool received_speed_limit;

// values received from Chassis CAN
float received_pedal_value;
uint8_t received_vehicle_speed;
uint8_t haldexGeneration = 4;

bool ledRed = true;
bool ledGreen = false;
bool ledBlue = false;

bool isGen1Standalone = false;
bool isGen2Standalone = false;
bool isGen4Standalone = false;

bool hasDiag = false;

bool isBusFailure = false;

bool broadcastOpenHaldexOverCAN = true;

uint32_t alerts_to_enable = 0;

enum openhaldex_mode_t {
  MODE_STOCK,
  MODE_FWD,
  MODE_5050,
  MODE_7525,
  MODE_CUSTOM,
  openhaldex_mode_t_MAX
};

struct lockpoint_t {
  uint8_t speed;
  uint8_t lock;
  uint8_t intensity;
};

#define CUSTOM_LOCK_POINTS_MAX_COUNT 10
struct openhaldex_custom_mode_t {
  lockpoint_t lockpoints[CUSTOM_LOCK_POINTS_MAX_COUNT];
  uint8_t lockpoint_bitfield_high_byte;
  uint8_t lockpoint_bitfield_low_byte;
  uint8_t lockpoint_count;
};

struct openhaldex_state_t {
  openhaldex_mode_t mode;
  openhaldex_custom_mode_t custom_mode;
  uint8_t pedal_threshold;
  bool mode_override;
};

// internal variables
openhaldex_state_t state;
float lock_target = 0;

// Convert a value of type openhaldex_mode_t to a string.
const char *get_openhaldex_mode_string(openhaldex_mode_t mode) {
  switch (mode) {
    case MODE_STOCK:
      return "STOCK";
    case MODE_FWD:
      return "FWD";
    case MODE_5050:
      return "5050";
    case MODE_7525:
      return "7525";
    case MODE_CUSTOM:
      return "CUSTOM";
    default:
      break;
  }
  return "?";
}

// for running through vars to see effects
uint8_t tempCounter;
uint8_t tempCounter1;
uint16_t tempCounter2;

// checksum values (for calculating module checksums in standalone mode)
uint8_t MOTOR5_counter = 0;
uint8_t MOTOR6_counter = 254;
uint8_t MOTOR6_counter2 = 0;

uint8_t BRAKES1_counter = 10;  // 0
uint8_t BRAKES2_counter = 0;   // starting counter for Brakes2 is 3
uint8_t BRAKES4_counter = 0;
uint8_t BRAKES4_counter2 = 0x64;
uint8_t BRAKES4_crc = 0;

uint8_t BRAKES5_counter = 0x04;
uint8_t BRAKES5_counter2 = 3;  // starting counter for Brake5 is 3

uint8_t BRAKES8_counter = 0x00;  // starting counter for Brakes9 is 11

uint8_t BRAKES9_counter = 0x03;  // starting counter for Brakes9 is 11
uint8_t BRAKES9_counter2 = 0x00;

uint8_t BRAKES10_counter = 0;

uint8_t mLW_1_counter = 0;  // was 0
uint8_t mLW_1_counter2 = 0x77;
uint8_t mLW_1_crc = 0;

uint8_t mDiagnose_1_counter = 0;

const uint8_t lws_1[16][8] = {
  { 0x88, 0x00, 0x00, 0x00, 0x80, 0x00, 0xA3, 0x77 },
  { 0x88, 0x00, 0x00, 0x00, 0x80, 0x10, 0x86, 0x67 },
  { 0x88, 0x00, 0x00, 0x00, 0x80, 0x20, 0xE9, 0x57 },
  { 0x88, 0x00, 0x00, 0x00, 0x80, 0x30, 0xCC, 0x47 },
  { 0x88, 0x00, 0x00, 0x00, 0x80, 0x40, 0x37, 0x37 },
  { 0x88, 0x00, 0x00, 0x00, 0x80, 0x50, 0x12, 0x27 },
  { 0x88, 0x00, 0x00, 0x00, 0x80, 0x60, 0x7D, 0x17 },
  { 0x88, 0x00, 0x00, 0x00, 0x80, 0x70, 0x58, 0x07 },
  { 0x88, 0x00, 0x00, 0x00, 0x80, 0x80, 0xA4, 0xF7 },
  { 0x88, 0x00, 0x00, 0x00, 0x80, 0x90, 0x81, 0xE7 },
  { 0x88, 0x00, 0x00, 0x00, 0x80, 0xA0, 0xEE, 0xD7 },
  { 0x88, 0x00, 0x00, 0x00, 0x80, 0xB0, 0xCB, 0xC7 },
  { 0x88, 0x00, 0x00, 0x00, 0x80, 0xC0, 0x30, 0xB7 },
  { 0x88, 0x00, 0x00, 0x00, 0x80, 0xD0, 0x15, 0xA7 },
  { 0x88, 0x00, 0x00, 0x00, 0x80, 0xE0, 0x7A, 0x97 },
  { 0x88, 0x00, 0x00, 0x00, 0x80, 0xF0, 0x5F, 0x87 }
  // ... add as many as you have; 16 fills the LUT fully
};
const uint8_t lws_2[16][8] = {
  { 0x22, 0x00, 0x00, 0x00, 0x80, 0x00, 0xA0, 0xDD },
  { 0x22, 0x00, 0x00, 0x00, 0x80, 0x10, 0x85, 0xCD },
  { 0x22, 0x00, 0x00, 0x00, 0x80, 0x20, 0xEA, 0xBD },
  { 0x22, 0x00, 0x00, 0x00, 0x80, 0x30, 0xCF, 0xAD },
  { 0x22, 0x00, 0x00, 0x00, 0x80, 0x40, 0x34, 0x9D },
  { 0x22, 0x00, 0x00, 0x00, 0x80, 0x50, 0x11, 0x8D },
  { 0x22, 0x00, 0x00, 0x00, 0x80, 0x60, 0x7E, 0x7D },
  { 0x22, 0x00, 0x00, 0x00, 0x80, 0x70, 0x5B, 0x6D },
  { 0x22, 0x00, 0x00, 0x00, 0x80, 0x80, 0xA7, 0x5D },
  { 0x22, 0x00, 0x00, 0x00, 0x80, 0x90, 0x82, 0x4D },
  { 0x22, 0x00, 0x00, 0x00, 0x80, 0xA0, 0xED, 0x3D },
  { 0x22, 0x00, 0x00, 0x00, 0x80, 0xB0, 0xC8, 0x2D },
  { 0x22, 0x00, 0x00, 0x00, 0x80, 0xC0, 0x33, 0x1D },
  { 0x22, 0x00, 0x00, 0x00, 0x80, 0xD0, 0x16, 0x0D },
  { 0x22, 0x00, 0x00, 0x00, 0x80, 0xE0, 0x79, 0xFD },
  { 0x22, 0x00, 0x00, 0x00, 0x80, 0xF0, 0x5C, 0xED }
};


// for EEP
extern void readEEP();
extern void writeEEP();

// for WiFi Function Prototypes
extern void connectWifi();
extern void disconnectWifi();
extern void setupUI();
extern void textCallback(Control *sender, int type);
extern void generalCallback(Control *sender, int type);
extern void selectCallback(Control *sender, int type);
extern void updateCallback(Control *sender, int type);
extern void getTimeCallback(Control *sender, int type);
extern void graphAddCallback(Control *sender, int type);
extern void graphClearCallback(Control *sender, int type);
extern void randomString(char *buf, int len);
extern void extendedCallback(Control *sender, int type, void *param);
extern void updateLabels();

// WiFi UI handles
uint16_t int16_currentMode, label_currentLocking, int16_disableThrottle, int16_disableSpeed,int16_haldexGeneration;
uint16_t bool_broadcastHaldex, bool_disableControl, bool_followHandbrake, bool_followBrakeSwitch, bool_isStandalone;

int label_hasChassisCAN, label_hasHaldexCAN, label_hasBusFailure, label_haldexState, label_haldexTemp, label_HaldexClutch1, label_HaldexClutch2, label_currentSpeed, label_currentRPM, label_currentBoost;

uint16_t graph;
uint16_t mainTime;
volatile bool updates = false;