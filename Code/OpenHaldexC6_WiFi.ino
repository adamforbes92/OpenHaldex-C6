void connectWifi() {
  int connect_timeout;

  WiFi.hostname(wifiHostName);
  Serial.println("Beginning WiFi...");

  Serial.println("\nCreating Access Point...");
  //WiFi.setTxPower(WIFI_POWER_8_5dBm);
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(IPAddress(192, 168, 1, 1), IPAddress(192, 168, 1, 1), IPAddress(255, 255, 255, 0));
  WiFi.softAP(wifiHostName);
  WiFi.setSleep(false);  // for the ESP32: turn off sleeping to increase UI responsivness (at the cost of power use)
}

void setupUI() {
  ESPUI.setVerbosity(Verbosity::Verbose);  // turn off verbose debugging (Verbose for ON; Quiet for OFF)
  ESPUI.sliderContinuous = false;          // update slider valves constantly disabled.  No need and can cause crashes

  // create basic tab
  auto tabModes = ESPUI.addControl(Tab, "", "Haldex Modes");
  ESPUI.addControl(Separator, "Current Mode", "", Dark, tabModes);
  int16_currentMode = ESPUI.addControl(Select, "Current Mode", "", Dark, tabModes, generalCallback);
  ESPUI.addControl(Option, "Stock", "Stock", Dark, int16_currentMode);
  ESPUI.addControl(Option, "FWD", "FWD", Dark, int16_currentMode);
  ESPUI.addControl(Option, "50/50", "5050", Dark, int16_currentMode);
  ESPUI.addControl(Option, "75/25", "7525", Dark, int16_currentMode);

  ESPUI.addControl(Separator, "Current Locking", "", Dark, tabModes);
  label_currentLocking = ESPUI.addControl(Label, "", "0", Dark, tabModes, generalCallback);

  int16_disableThrottle = ESPUI.addControl(Slider, "Disable Throttle", "0", Dark, tabModes, generalCallback);
  ESPUI.addControl(Min, "", "0", Dark, int16_disableThrottle);
  ESPUI.addControl(Max, "", "100", Dark, int16_disableThrottle);

  int16_disableSpeed = ESPUI.addControl(Slider, "Disable Speed", "0", Dark, tabModes, generalCallback);
  ESPUI.addControl(Min, "", "0", Dark, int16_disableSpeed);
  ESPUI.addControl(Max, "", "300", Dark, int16_disableSpeed);

  bool_disableControl = ESPUI.addControl(Switcher, "Disable Controller", "", Dark, tabModes, generalCallback);

  auto tabSetup = ESPUI.addControl(Tab, "", "General Setup");
  ESPUI.addControl(Separator, "Haldex Generation", "", Dark, tabSetup);
  int16_haldexGeneration = ESPUI.addControl(Select, "Haldex Generation", "", Dark, tabSetup, generalCallback);
  ESPUI.addControl(Option, "Generation 1", "Gen1", Dark, int16_haldexGeneration);
  ESPUI.addControl(Option, "Generation 2", "Gen2", Dark, int16_haldexGeneration);
  ESPUI.addControl(Option, "Generation 4", "Gen4", Dark, int16_haldexGeneration);

  ESPUI.addControl(Separator, "Follow", "", Dark, tabSetup);
  bool_followHandbrake = ESPUI.addControl(Switcher, "Follow Handbrake", "", Dark, tabSetup, generalCallback);
  bool_followBrakeSwitch = ESPUI.addControl(Switcher, "Follow Brake Switch", "", Dark, tabSetup, generalCallback);

  ESPUI.addControl(Separator, "Haldex Standalone", "", Dark, tabSetup);
  bool_isStandalone = ESPUI.addControl(Switcher, "Standalone", "", Dark, tabSetup, generalCallback);

  ESPUI.addControl(Separator, "Broadcast Haldex", "", Dark, tabSetup);
  bool_broadcastHaldex = ESPUI.addControl(Switcher, "Broadcast Haldex", "", Dark, tabSetup, generalCallback);

  auto tabDiag = ESPUI.addControl(Tab, "", "Diagnostics");
  ESPUI.addControl(Separator, "Chassis CAN", "", Dark, tabDiag);
  label_hasChassisCAN = ESPUI.addControl(Label, "", "0", Dark, tabDiag, generalCallback);
  ESPUI.addControl(Separator, "Haldex CAN", "", Dark, tabDiag);
  label_hasHaldexCAN = ESPUI.addControl(Label, "", "0", Dark, tabDiag, generalCallback);
  ESPUI.addControl(Separator, "Bus Failures", "", Dark, tabDiag);
  label_hasBusFailure = ESPUI.addControl(Label, "", "0", Dark, tabDiag, generalCallback);

  ESPUI.addControl(Separator, "Haldex State", "", Dark, tabDiag);
  label_HaldexState = ESPUI.addControl(Label, "", "0", Dark, tabDiag, generalCallback);
  ESPUI.addControl(Separator, "Haldex Temp", "", Dark, tabDiag);
  label_HaldexTemp = ESPUI.addControl(Label, "", "0", Dark, tabDiag, generalCallback);
  ESPUI.addControl(Separator, "Haldex Clutch 1", "", Dark, tabDiag);
  label_HaldexClutch1 = ESPUI.addControl(Label, "", "0", Dark, tabDiag, generalCallback);
  ESPUI.addControl(Separator, "Haldex Clutch 2", "", Dark, tabDiag);
  label_HaldexClutch2 = ESPUI.addControl(Label, "", "0", Dark, tabDiag, generalCallback);
  ESPUI.addControl(Separator, "Haldex Coupling", "", Dark, tabDiag);
  label_HaldexCoupling = ESPUI.addControl(Label, "", "0", Dark, tabDiag, generalCallback);
  ESPUI.addControl(Separator, "Haldex Speed Limit", "", Dark, tabDiag);
  label_HaldexSpeedLimit = ESPUI.addControl(Label, "", "0", Dark, tabDiag, generalCallback);

  ESPUI.addControl(Separator, "Speed", "", Dark, tabDiag);
  label_currentSpeed = ESPUI.addControl(Label, "", "0", Dark, tabDiag, generalCallback);
  ESPUI.addControl(Separator, "RPM", "", Dark, tabDiag);
  label_currentRPM = ESPUI.addControl(Label, "", "0", Dark, tabDiag, generalCallback);
  ESPUI.addControl(Separator, "Boost", "", Dark, tabDiag);
  label_currentBoost = ESPUI.addControl(Label, "", "0", Dark, tabDiag, generalCallback);
  //Finally, start up the UI.
  //This should only be called once we are connected to WiFi.
  ESPUI.begin(wifiHostName);
  WiFi.setTxPower(WIFI_POWER_8_5dBm);  // set a lower power mode (some C3 aerials aren't great and leaving it high causes failures)
}

void disconnectWifi() {
  DEBUG("Number of connections: ");
  //DEBUG(WiFi.softAPgetStationNum());

  if (WiFi.softAPgetStationNum() == 0) {
    DEBUG("No connections, turning off");
    WiFi.disconnect(true, false);
    WiFi.mode(WIFI_OFF);
  }
}

void generalCallback(Control *sender, int type) {
#ifdef detailedDebugWiFi
  Serial.print("CB: id(");
  Serial.print(sender->id);
  Serial.print(") Type(");
  Serial.print(type);
  Serial.print(") '");
  Serial.print(sender->label);
  Serial.print("' = ");
  Serial.println(sender->value);
#endif

  uint8_t tempID = int(sender->id);
  switch (tempID) {
    case 3:
      if (sender->value == "Stock") {
        state.mode = MODE_STOCK;
      }
      if (sender->value == "FWD") {
        state.mode = MODE_FWD;
      }
      if (sender->value == "5050") {
        state.mode = MODE_5050;
      }
      if (sender->value == "7525") {
        state.mode = MODE_7525;
      }
      break;
  }
}

void extendedCallback(Control *sender, int type, void *param) {
#ifdef serialDebugWifi
  Serial.print("CB: id(");
  Serial.print(sender->id);
  Serial.print(") Type(");
  Serial.print(type);
  Serial.print(") '");
  Serial.print(sender->label);
  Serial.print("' = ");
  Serial.println(sender->value);
  Serial.print("param = ");
  Serial.println((long)param);
#endif
}

void updateLabels() {
  if (lastCANChassis < 500) {
    hasCANChassis = true;
  } else {
    hasCANChassis = false;
  }
  if (lastCANHaldex < 500) {
    hasCANHaldex = true;
  } else {
    hasCANHaldex = false;
  }

  if (hasCANChassis) {
    ESPUI.updateLabel(label_hasChassisCAN, "Yes");
  } else {
    ESPUI.updateLabel(label_hasChassisCAN, "No");
  }
  if (hasCANHaldex) {
    ESPUI.updateLabel(label_hasHaldexCAN, "Yes");
  } else {
    ESPUI.updateLabel(label_hasHaldexCAN, "No");
  }

  if (isBusFailure) {
    ESPUI.updateLabel(label_hasBusFailure, "Yes");
  } else {
    ESPUI.updateLabel(label_hasBusFailure, "No");
  }

  char haldexTemp[50];
  sprintf(haldexTemp, "Req:Act: %d", received_haldex_engagement);
  ESPUI.updateLabel(label_currentLocking, String(haldexTemp));

  haldexTemp[0] = '\0';
  sprintf(haldexTemp, "State: %d", received_haldex_state);
  ESPUI.updateLabel(label_HaldexTemp, String(haldexTemp));

  haldexTemp[0] = '\0';
  sprintf(haldexTemp, "Clutch 1: %d", received_report_clutch1);
  ESPUI.updateLabel(label_HaldexClutch1, String(haldexTemp));

  haldexTemp[0] = '\0';
  sprintf(haldexTemp, "Clutch 2: %d", received_report_clutch2);
  ESPUI.updateLabel(label_HaldexClutch2, String(haldexTemp));

  haldexTemp[0] = '\0';
  sprintf(haldexTemp, "Coupling: %d", received_coupling_open);
  ESPUI.updateLabel(label_HaldexCoupling, String(haldexTemp));

  haldexTemp[0] = '\0';
  sprintf(haldexTemp, "Speed Limit: %d", received_speed_limit);
  ESPUI.updateLabel(label_HaldexSpeedLimit, String(haldexTemp));

  haldexTemp[0] = '\0';
  sprintf(haldexTemp, "Speed: %d", received_vehicle_speed);
  ESPUI.updateLabel(label_currentSpeed, String(haldexTemp));

  haldexTemp[0] = '\0';
  sprintf(haldexTemp, "RPM: %d", received_pedal_value);
  ESPUI.updateLabel(label_currentRPM, String(haldexTemp));

  haldexTemp[0] = '\0';
  sprintf(haldexTemp, "Boost: %d(mbar)", received_speed_limit);
  ESPUI.updateLabel(label_currentBoost, String(haldexTemp));

  switch (haldexGeneration) {
    case 1:
      ESPUI.updateSelect(int16_haldexGeneration, "Gen1");
      break;
    case 2:
      ESPUI.updateSelect(int16_haldexGeneration, "Gen2");
      break;
    case 4:
      ESPUI.updateSelect(int16_haldexGeneration, "Gen4");
      break;
  }

  switch (state.mode) {
    case 0:
      ESPUI.updateSelect(int16_currentMode, "Stock");
      break;
    case 1:
      ESPUI.updateSelect(int16_currentMode, "FWD");
      break;
    case 2:
      ESPUI.updateSelect(int16_currentMode, "5050");
      break;
    case 3:
      ESPUI.updateSelect(int16_currentMode, "7525");
      break;
  }

  if (state.mode == MODE_STOCK) {
    strip.setLedColorData(led_channel, led_brightness, 0, 0);
  }
  if (state.mode == MODE_FWD) {
    strip.setLedColorData(led_channel, 0, led_brightness, 0);
  }
  if (state.mode == MODE_5050) {
    strip.setLedColorData(led_channel, 0, 0, led_brightness);
  }
  if (state.mode == MODE_7525) {
    strip.setLedColorData(led_channel, 0, led_brightness, led_brightness);
  }
  strip.show();
}