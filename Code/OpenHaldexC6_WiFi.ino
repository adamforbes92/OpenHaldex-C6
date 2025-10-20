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
  label_haldexState = ESPUI.addControl(Label, "", "0", Dark, tabDiag, generalCallback);
  ESPUI.addControl(Separator, "Haldex Temp", "", Dark, tabDiag);
  label_haldexTemp = ESPUI.addControl(Label, "", "0", Dark, tabDiag, generalCallback);
  ESPUI.addControl(Separator, "Haldex Clutch 1", "", Dark, tabDiag);
  label_HaldexClutch1 = ESPUI.addControl(Label, "", "0", Dark, tabDiag, generalCallback);
  ESPUI.addControl(Separator, "Haldex Clutch 2", "", Dark, tabDiag);
  label_HaldexClutch2 = ESPUI.addControl(Label, "", "0", Dark, tabDiag, generalCallback);

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