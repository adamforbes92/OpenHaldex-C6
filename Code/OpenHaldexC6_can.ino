void broadcastOpenHaldex() {
  twai_message_t broadcast_frame;
  broadcast_frame.identifier = OPENHALDEX_BROADCAST_ID;
  broadcast_frame.extd = 0;
  broadcast_frame.rtr = 0;
  broadcast_frame.data_length_code = 8;
  broadcast_frame.data[0] = 0;
  broadcast_frame.data[1] = isGen1Standalone + isGen2Standalone + isGen4Standalone;
  broadcast_frame.data[2] = (uint8_t)received_haldex_engagement;
  broadcast_frame.data[3] = (uint8_t)lock_target;
  broadcast_frame.data[4] = received_vehicle_speed;
  broadcast_frame.data[5] = state.mode_override;
  broadcast_frame.data[6] = (uint8_t)state.mode;
  broadcast_frame.data[7] = (uint8_t)received_pedal_value;
  twai_transmit_v2(twai_bus_0, &broadcast_frame, pdMS_TO_TICKS(20));
}

void parseCAN_chs(void *arg) {
  while (1) {
    //stackCHS = uxTaskGetStackHighWaterMark(NULL);
    if (twai_receive_v2(twai_bus_0, &rx_message_chs, pdMS_TO_TICKS(1000)) == ESP_OK) {
      tx_message_hdx.identifier = rx_message_chs.identifier;
      lastCANChassis = pdTICKS_TO_MS(xTaskGetTickCount()) - lastCANChassis;
      if (isGen1Standalone || isGen2Standalone || isGen4Standalone) {
        switch (rx_message_chs.identifier) {
          case diagnostics_1_ID:
          case diagnostics_2_ID:
          case diagnostics_3_ID:
          case diagnostics_4_ID:
          case diagnostics_5_ID:
            tx_message_hdx = rx_message_chs;
            tx_message_hdx.extd = rx_message_chs.extd;
            tx_message_hdx.rtr = rx_message_chs.rtr;
            tx_message_hdx.data_length_code = rx_message_chs.data_length_code;
            tx_message_hdx = rx_message_chs;
            twai_transmit_v2(twai_bus_1, &tx_message_hdx, pdMS_TO_TICKS(10));
            break;
        }
      }
      if (!isGen1Standalone && !isGen2Standalone && !isGen4Standalone) {
        switch (rx_message_chs.identifier) {
          case MOTOR1_ID:
            received_pedal_value = rx_message_chs.data[5] * 0.4;  // wpedv_w
            break;

          case MOTOR2_ID:
            received_vehicle_speed = rx_message_chs.data[3] * 128 / 100;  // vfzg
            break;

          case OPENHALDEX_EXTERNAL_CONTROL_ID:
            // If the requested mode is valid, apply it.
            if (rx_message_chs.data[0] < (uint8_t)openhaldex_mode_t_MAX && rx_message_chs.data[0] != (uint8_t)MODE_CUSTOM) {
              state.mode = (openhaldex_mode_t)rx_message_chs.data[0];
            }
            break;
        }

        // Edit the CAN frame, if not in STOCK mode (otherwise, the original frame is already copied in the new buffer).
        if (state.mode != MODE_STOCK) {
          if (haldexGeneration == 1 || haldexGeneration == 2 || haldexGeneration == 4) {
            get_lock_data(rx_message_chs);
          }
        }
        tx_message_hdx = rx_message_chs;
        tx_message_hdx.extd = rx_message_chs.extd;
        tx_message_hdx.rtr = rx_message_chs.rtr;
        tx_message_hdx.data_length_code = rx_message_chs.data_length_code;
        tx_message_hdx = rx_message_chs;
        twai_transmit_v2(twai_bus_1, &tx_message_hdx, pdMS_TO_TICKS(10));
        rxtxcount++;
      }
    }
  }
}

void parseCAN_hdx(void *arg) {
  while (1) {
    //stackHDX = uxTaskGetStackHighWaterMark(NULL);
    if (twai_receive_v2(twai_bus_1, &rx_message_hdx, pdMS_TO_TICKS(1000)) == ESP_OK) {
      lastCANHaldex = pdTICKS_TO_MS(xTaskGetTickCount()) - lastCANHaldex;

      //tx_message_chs.identifier = rx_message_hdx.identifier;
      //tx_message_chs.extd = rx_message_hdx.extd;
      //tx_message_chs.rtr = rx_message_hdx.rtr;
      //tx_message_chs.data_length_code = rx_message_hdx.data_length_code;
      //tx_message_chs = rx_message_hdx;
      twai_transmit_v2(twai_bus_0, &rx_message_hdx, pdMS_TO_TICKS(10));
      rxtxcount++;

      if (haldexGeneration == 1) {
        received_haldex_engagement = map(rx_message_hdx.data[1], 128, 198, 0, 100);
      }

      if (haldexGeneration == 2) {
        received_haldex_engagement = map(rx_message_hdx.data[1] + rx_message_hdx.data[4], 128, 255, 0, 100);
      }

      if (haldexGeneration == 4) {
        received_haldex_engagement = map(rx_message_hdx.data[1], 128, 255, 0, 100);
      }
      received_haldex_state = rx_message_hdx.data[0];

      // Decode the state byte.
      received_report_clutch1 = (received_haldex_state & (1 << 0));
      received_temp_protection = (received_haldex_state & (1 << 1));
      received_report_clutch2 = (received_haldex_state & (1 << 2));
      received_coupling_open = (received_haldex_state & (1 << 3));
      received_speed_limit = (received_haldex_state & (1 << 6));
    }
  }
}