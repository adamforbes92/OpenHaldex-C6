void connectWifi() {
  WiFi.hostname(wifiHostName);
#if detailedDebugWiFi
  DEBUG("Beginning WiFi...");
  DEBUG("Creating Access Point...");
#endif
  //WiFi.setTxPower(WIFI_POWER_8_5dBm);
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(IPAddress(192, 168, 1, 1), IPAddress(192, 168, 1, 1), IPAddress(255, 255, 255, 0));
  WiFi.softAP(wifiHostName);
  WiFi.setSleep(false);  // for the ESP32: turn off sleeping to increase UI responsivness (at the cost of power use)
}

void setupUI() {
  ESPUI.setVerbosity(Verbosity::Quiet);  // turn off verbose debugging (Verbose for ON; Quiet for OFF)
  ESPUI.sliderContinuous = true;         // update slider valves constantly disabled.  No need and can cause crashes

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

  int16_disableThrottle = ESPUI.addControl(Slider, "Disable Below Throttle", String(disableThrottle), Dark, tabModes, generalCallback);
  ESPUI.addControl(Min, "", "0", Dark, int16_disableThrottle);
  ESPUI.addControl(Max, "", "100", Dark, int16_disableThrottle);

  int16_disableSpeed = ESPUI.addControl(Slider, "Disable Above Speed", String(disableSpeed), Dark, tabModes, generalCallback);
  ESPUI.addControl(Min, "", "0", Dark, int16_disableSpeed);
  ESPUI.addControl(Max, "", "300", Dark, int16_disableSpeed);

  bool_disableControl = ESPUI.addControl(Switcher, "Disable Controller", String(disableController), Dark, tabModes, generalCallback);

  auto tabSetup = ESPUI.addControl(Tab, "", "General Setup");
  ESPUI.addControl(Separator, "Haldex Generation", "", Dark, tabSetup);
  int16_haldexGeneration = ESPUI.addControl(Select, "Haldex Generation", "", Dark, tabSetup, generalCallback);
  ESPUI.addControl(Option, "Generation 1", "Gen1", Dark, int16_haldexGeneration);
  ESPUI.addControl(Option, "Generation 2", "Gen2", Dark, int16_haldexGeneration);
  ESPUI.addControl(Option, "Generation 4", "Gen4", Dark, int16_haldexGeneration);

  ESPUI.addControl(Separator, "Follow", "", Dark, tabSetup);
  bool_followHandbrake = ESPUI.addControl(Switcher, "Follow Handbrake", String(followHandbrake), Dark, tabSetup, generalCallback);
  bool_invertHandbrake = ESPUI.addControl(Switcher, "Invert Handbrake", String(invertHandbrake), Dark, tabSetup, generalCallback);

  bool_followBrake = ESPUI.addControl(Switcher, "Follow Brake", String(followBrake), Dark, tabSetup, generalCallback);
  bool_invertBrake = ESPUI.addControl(Switcher, "Invert Brake", String(invertBrake), Dark, tabSetup, generalCallback);

  ESPUI.addControl(Separator, "Haldex Standalone", "", Dark, tabSetup);
  bool_isStandalone = ESPUI.addControl(Switcher, "Standalone", String(isStandalone), Dark, tabSetup, generalCallback);

  ESPUI.addControl(Separator, "Broadcast Haldex", "", Dark, tabSetup);
  bool_broadcastHaldex = ESPUI.addControl(Switcher, "Broadcast Haldex", String(broadcastOpenHaldexOverCAN), Dark, tabSetup, generalCallback);

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

  ESPUI.addControl(Separator, "Brake Signal In", "", Dark, tabDiag);
  label_brakeIn = ESPUI.addControl(Label, "", "0", Dark, tabDiag, generalCallback);

  ESPUI.addControl(Separator, "Brake Signal Out", "", Dark, tabDiag);
  label_brakeOut = ESPUI.addControl(Label, "", "0", Dark, tabDiag, generalCallback);

  ESPUI.addControl(Separator, "Handbrake Signal In", "", Dark, tabDiag);
  label_handbrakeIn = ESPUI.addControl(Label, "", "0", Dark, tabDiag, generalCallback);

  ESPUI.addControl(Separator, "Handbrake Signal Out", "", Dark, tabDiag);
  label_handbrakeOut = ESPUI.addControl(Label, "", "0", Dark, tabDiag, generalCallback);

  //Finally, start up the UI.
  //This should only be called once we are connected to WiFi.
  ESPUI.begin(wifiHostName);
  //WiFi.setTxPower(WIFI_POWER_8_5dBm);  // set a lower power mode (some C3 aerials aren't great and leaving it high causes failures)
}

void disconnectWifi() {
  WiFi.disconnect(true, true);
  WiFi.mode(WIFI_OFF);

  connectWifi();  // enable / start WiFi - in '_wifi.ino'
  setupUI();      // setup wifi user interface - in '_wifi.ino'
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
    case 10:
      disableThrottle = sender->value.toInt();
      state.pedal_threshold = disableThrottle;
      break;
    case 13:
      disableSpeed = sender->value.toInt();
      break;
    case 16:
      disableController = sender->value.toInt();
      break;
    case 19:
      if (sender->value == "Gen1") {
        haldexGeneration = 1;
      }
      if (sender->value == "Gen2") {
        haldexGeneration = 2;
      }
      if (sender->value == "Gen4") {
        haldexGeneration = 4;
      }
      break;
    case 24:
      followHandbrake = sender->value.toInt();
      break;
    case 25:
      invertHandbrake = sender->value.toInt();
      break;
    case 26:
      followBrake = sender->value.toInt();
      break;
    case 27:
      invertBrake = sender->value.toInt();
      break;
    case 29:
      isStandalone = sender->value.toInt();

      if (!isStandalone) {
        vTaskSuspend(handle_frames1000);
        vTaskSuspend(handle_frames200);
        vTaskSuspend(handle_frames100);
        vTaskSuspend(handle_frames25);
        vTaskSuspend(handle_frames20);
        vTaskSuspend(handle_frames10);
      } else {
        vTaskResume(handle_frames1000);
        vTaskResume(handle_frames200);
        vTaskResume(handle_frames100);
        vTaskResume(handle_frames25);
        vTaskResume(handle_frames20);
        vTaskResume(handle_frames10);
      }
      break;
    case 31:
      broadcastOpenHaldexOverCAN = sender->value.toInt();
      break;
  }
}

void extendedCallback(Control *sender, int type, void *param) {
#ifdef serialDebugWifi
  Serial.print("eCB: id(");
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

void updateLabels(void *arg) {
  while (1) {
    stackupdateLabels = uxTaskGetStackHighWaterMark(NULL);

    if ((millis() - lastCANChassisTick) > 500) {
      hasCANChassis = false;
    } else {
      hasCANChassis = true;
    }

    if ((millis() - lastCANHaldexTick) > 500) {
      hasCANHaldex = false;
    } else {
      hasCANHaldex = true;
    }

    hasCANChassis ? ESPUI.updateLabel(label_hasChassisCAN, "Yes") : ESPUI.updateLabel(label_hasChassisCAN, "No");
    hasCANHaldex ? ESPUI.updateLabel(label_hasHaldexCAN, "Yes") : ESPUI.updateLabel(label_hasHaldexCAN, "No");

    isBusFailure ? ESPUI.updateLabel(label_hasBusFailure, "Yes") : ESPUI.updateLabel(label_hasBusFailure, "No");

    disableController ? state.mode = MODE_STOCK : state.mode = state.mode;

    switch (haldexGeneration) {
      case 1:
        ESPUI.updateSelect(int16_haldexGeneration, "Gen1");
        isStandalone ? isGen1Standalone = true : isGen1Standalone = false;
        break;
      case 2:
        ESPUI.updateSelect(int16_haldexGeneration, "Gen2");
        isStandalone ? isGen2Standalone = true : isGen2Standalone = false;
        break;
      case 4:
        ESPUI.updateSelect(int16_haldexGeneration, "Gen4");
        isStandalone ? isGen4Standalone = true : isGen4Standalone = false;
        break;
    }

    switch (state.mode) {
      case 0:
        if (isStandalone) {
          state.mode = MODE_FWD;
          lastMode = 1;
          break;
        }

        lastMode = 0;
        ESPUI.updateSelect(int16_currentMode, "Stock");
        strip.setLedColorData(led_channel, led_brightness, 0, 0);
        break;
      case 1:
        lastMode = 1;
        ESPUI.updateSelect(int16_currentMode, "FWD");
        strip.setLedColorData(led_channel, 0, led_brightness, 0);
        break;
      case 2:
        lastMode = 2;
        ESPUI.updateSelect(int16_currentMode, "5050");
        strip.setLedColorData(led_channel, 0, 0, led_brightness);
        break;
      case 3:
        lastMode = 3;
        ESPUI.updateSelect(int16_currentMode, "7525");
        strip.setLedColorData(led_channel, 0, led_brightness, led_brightness);
        break;
    }
    strip.show();

    char haldexTemp[50];
    sprintf(haldexTemp, "Req:Act: %d:%d", int(appliedTorque), received_haldex_engagement);  //lock_target appliedTorque
    ESPUI.updateLabel(label_currentLocking, String(haldexTemp));

    haldexTemp[0] = '\0';
    sprintf(haldexTemp, "%d kmh", received_vehicle_speed);
    ESPUI.updateLabel(label_currentSpeed, String(haldexTemp));

    haldexTemp[0] = '\0';
    sprintf(haldexTemp, "%d mbar", received_vehicle_boost);
    ESPUI.updateLabel(label_currentBoost, String(haldexTemp));

    haldexTemp[0] = '\0';
    sprintf(haldexTemp, BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(received_haldex_state));
    ESPUI.updateLabel(label_HaldexState, String(haldexTemp));

    ESPUI.updateLabel(label_HaldexClutch1, received_report_clutch1 ? "Open" : "Closed");
    ESPUI.updateLabel(label_HaldexClutch2, received_report_clutch2 ? "Open" : "Closed");
    ESPUI.updateLabel(label_HaldexCoupling, received_coupling_open ? "Open" : "Closed");
    ESPUI.updateLabel(label_HaldexSpeedLimit, received_speed_limit ? "Active" : "Not Active");

    ESPUI.updateLabel(label_currentRPM, String(received_vehicle_rpm));

    ESPUI.updateLabel(label_brakeIn, brakeSignalActive ? "Active" : "Not Active");
    ESPUI.updateLabel(label_handbrakeIn, handbrakeSignalActive ? "Active" : "Not Active");

    ESPUI.updateLabel(label_brakeOut, brakeActive ? "Active" : "Not Active");
    ESPUI.updateLabel(label_handbrakeOut, handbrakeActive ? "Active" : "Not Active");


    vTaskDelay(labelRefresh / portTICK_PERIOD_MS);
  }
}