void setupIO() {
  pinMode(CAN0_RS, OUTPUT);    // gpio for controlling can_0 slope
  pinMode(CAN1_RS, OUTPUT);    // gpio for controlling can_0 slope
  digitalWrite(CAN0_RS, LOW);  // set slope low (for fast speed)
  digitalWrite(CAN1_RS, LOW);  // set slope low (for fast speed)

  pinMode(gpio_mode, INPUT);  // gpio for internal mode button
  //pinMode(gpio_mode_ext, INPUT);  // gpio for internal mode button

  strip.begin();                        // begin RGB LED onboard
  strip.setBrightness(led_brightness);  // default brightness

  twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(gpio_num_t(CAN0_TX), gpio_num_t(CAN0_RX), TWAI_MODE_NO_ACK);  // TWAI_MODE_NORMAL, TWAI_MODE_NO_ACK or TWAI_MODE_LISTEN_ONLY
  twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();                                                             // default CAN speed
  twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();                                                           // accept all messages

  //g_config.intr_flags = ESP_INTR_FLAG_LOWMED;  //Optional - move canbus irq to free up the default level 1 IRQ it will take up.  Todo
  g_config.tx_queue_len = 4096;  //<TWAI_GENERAL_CONFIG_DEFAULT default is 5, use this to increase if needed
  g_config.rx_queue_len = 4096;  //<TWAI_GENERAL_CONFIG_DEFAULT default is 5, use this to increase if needed
  //g_config.intr_flags = ESP_INTR_FLAG_IRAM;

  //g_config.ss = 1; // single shot - todo

  // setup CAN Controller (0)
  g_config.controller_id = 0;
  ESP_ERROR_CHECK(twai_driver_install_v2(&g_config, &t_config, &f_config, &twai_bus_0));
  DEBUG("Driver 0 Installed");
  ESP_ERROR_CHECK(twai_start_v2(twai_bus_0));
  DEBUG("Driver 0 Started");

  // setup CAN Controller (1)
  g_config.controller_id = 1;
  g_config.tx_io = gpio_num_t(CAN1_TX);
  g_config.rx_io = gpio_num_t(CAN1_RX);
  ESP_ERROR_CHECK(twai_driver_install_v2(&g_config, &t_config, &f_config, &twai_bus_1));
  DEBUG("Driver 1 Installed");
  ESP_ERROR_CHECK(twai_start_v2(twai_bus_1));
  DEBUG("Driver 1 Started");

  // Reconfigure alerts to detect frame receive, Bus-Off error and RX queue full states
  uint32_t alerts_to_enable = TWAI_ALERT_RX_DATA | TWAI_ALERT_ERR_PASS | TWAI_ALERT_BUS_ERROR | TWAI_ALERT_RX_QUEUE_FULL;
  if (twai_reconfigure_alerts(alerts_to_enable, NULL) == ESP_OK) {
    DEBUG("Reconfiguration of CAN alerts");
  } else {
    DEBUG("Failed to reconfigure CAN alerts!");
    return;
  }
}

void updateLED() {
  if (state.mode == MODE_STOCK) {
    state.mode = MODE_FWD;
    strip.setLedColorData(led_channel, 0, led_brightness, 0);
    strip.show();
    return;
  }
  if (state.mode == MODE_FWD) {
    state.mode = MODE_5050;
    strip.setLedColorData(led_channel, 0, 0, led_brightness);
    strip.show();
    return;
  }
  if (state.mode == MODE_5050) {
    state.mode = MODE_7525;
    strip.setLedColorData(led_channel, 0, led_brightness, led_brightness);
    strip.show();
    return;
  }
  if (state.mode == MODE_7525) {
    state.mode = MODE_STOCK;
    strip.setLedColorData(led_channel, led_brightness, 0, 0);
    strip.show();
    return;
  }
}

void setupTickers() {
  tickUpdateLED.start();
  tickBroadcastOpenHaldex.start();
  tickHaldexState.start();
  tickEEP.start();   // begin ticker for the EEPROM
  tickWiFi.start();  // begin ticker for the WiFi (to turn off after 60s)

  if (isGen1Standalone) {
    Gen1_tickFrames10.start();
    Gen1_tickFrames20.start();
    Gen1_tickFrames25.start();
    Gen1_tickFrames100.start();
    Gen1_tickFrames200.start();
    Gen1_tickFrames1000.start();
  }

  if (isGen2Standalone) {
    Gen2_tickFrames10.start();
    Gen2_tickFrames20.start();
    Gen2_tickFrames25.start();
    Gen2_tickFrames100.start();
    Gen2_tickFrames200.start();
    Gen2_tickFrames1000.start();
  }

  if (isGen4Standalone) {
    Gen4_tickFrames10.start();
    Gen4_tickFrames20.start();
    Gen4_tickFrames25.start();
    Gen4_tickFrames100.start();
    Gen4_tickFrames200.start();
    Gen4_tickFrames1000.start();
  }
}

void updateTickers() {
  tickUpdateLED.update();
  tickHaldexState.update();
  tickEEP.update();   // refresh the EEP ticker
  tickWiFi.update();  // refresh the WiFi ticker
  btnMode.tick();    // refresh the paddle up ticker
  btnModeExt.tick();    // refresh the paddle up ticker

  if (broadcastOpenHaldexOverCAN) { tickBroadcastOpenHaldex.update(); }

  if (isGen1Standalone) {
    Gen1_tickFrames10.update();
    Gen1_tickFrames20.update();
    Gen1_tickFrames25.update();
    Gen1_tickFrames100.update();
    Gen1_tickFrames200.update();
    Gen1_tickFrames1000.update();
  }

  if (isGen2Standalone) {
    Gen2_tickFrames10.update();
    Gen2_tickFrames20.update();
    Gen2_tickFrames25.update();
    Gen2_tickFrames100.update();
    Gen2_tickFrames200.update();
    Gen2_tickFrames1000.update();
  }

  if (isGen4Standalone) {
    Gen4_tickFrames10.update();
    Gen4_tickFrames20.update();
    Gen4_tickFrames25.update();
    Gen4_tickFrames100.update();
    Gen4_tickFrames200.update();
    Gen4_tickFrames1000.update();
  }
}

void showHaldexState() {
  DEBUG("Mode: %s", get_openhaldex_mode_string(state.mode));
  DEBUG("    haldexEngagement: %d", received_haldex_engagement);  // this is the lock %

  if (detailedDebug) {
    DEBUG("    Raw haldexEngagement: %d", received_haldex_state);
    DEBUG("    reportClutch1: %d", received_report_clutch1);  // this means it has a clutch issue
    DEBUG("    reportClutch2: %d", received_report_clutch2);  // this means it also has a clutch issue
    DEBUG("    couplingOpen: %d", received_coupling_open);    // clutch fully disengaged
    DEBUG("    speedLimit: %d", received_speed_limit);        // hit a speed limit...
    DEBUG("    tempCounter2: %d", rxtxcount);                 // incrememting value for checking the response to vars...
    if (isBusFailure) {
      DEBUG("    Bus Failure: True");
    }
  }

  if (detailedDebugCAN) {
    uint32_t alerts_triggered;
    twai_read_alerts(&alerts_triggered, pdMS_TO_TICKS(0));
    twai_status_info_t twaistatus;
    twai_get_status_info(&twaistatus);
    DEBUG("    CAN-BUS Details:");  // this is the lock %
    //Serial.printf("TX buffered: %lu\t", twaistatus.msgs_to_tx);
    DEBUG("    RX buffered: %lu\t", twaistatus.msgs_to_rx);
    DEBUG("    RX missed: %lu\t", twaistatus.rx_missed_count);
    DEBUG("    RX overrun %lu\n", twaistatus.rx_overrun_count);
    DEBUG("    %d", stackCHS);
    DEBUG("    %d", stackHDX);

    if (alerts_triggered & TWAI_ALERT_BUS_OFF) {
      DEBUG("    Bus Off: True");  // incrememting value for checking the response to vars...
    }
    if (alerts_triggered & TWAI_ALERT_TX_IDLE) {
      DEBUG("    TX Idle: True");  // incrememting value for checking the response to vars...
    }
    if (alerts_triggered & TWAI_ALERT_BUS_ERROR) {
      DEBUG("    Bus Error: True");  // incrememting value for checking the response to vars...
    }
    if (alerts_triggered & TWAI_ALERT_RX_QUEUE_FULL) {
      DEBUG("    RX Queue Full: True");  // incrememting value for checking the response to vars...
    }
  }
}
