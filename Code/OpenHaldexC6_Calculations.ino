// Only executed when in MODE_FWD/MODE_5050/MODE_CUSTOM
float get_lock_target_adjustment() {
  // Handle FWD and 5050 modes.
  switch (state.mode) {
    case MODE_FWD:
      return 0;

    case MODE_5050:
      if (state.pedal_threshold == 0 && disableSpeed == 0) {
        return 100;
      }

      if (int(received_pedal_value) >= state.pedal_threshold || state.pedal_threshold == 0 || received_vehicle_speed < disableSpeed || state.mode_override) {
        return 100;
      }
      return 0;

    case MODE_6040:
      if (int(received_pedal_value) >= state.pedal_threshold || state.pedal_threshold == 0 || received_vehicle_speed < disableSpeed || state.mode_override) {
        return 40;
      }
      return 0;

    case MODE_7525:
      if (int(received_pedal_value) >= state.pedal_threshold || state.pedal_threshold == 0 || received_vehicle_speed < disableSpeed || state.mode_override) {
        return 30;
      }
      return 0;

    case MODE_CUSTOM:
      if (int(received_pedal_value) >= state.pedal_threshold || state.pedal_threshold == 0 || received_vehicle_speed < disableSpeed || state.mode_override) {
        return 30;
      }
      return 0;

    default:
      return 0;
  }

  // Getting here means it's in not FWD or 5050/7525.

  // Check if locking is necessary.
  if (!(int(received_pedal_value) >= state.pedal_threshold || state.pedal_threshold == 0 || received_vehicle_speed < disableSpeed || state.mode_override)) {
    return 0;
  }

  // Find the pair of lockpoints between which the vehicle speed falls.
  lockpoint_t lp_lower = state.custom_mode.lockpoints[0];
  lockpoint_t lp_upper = state.custom_mode.lockpoints[state.custom_mode.lockpoint_count - 1];

  // Look for the lockpoint above the current vehicle speed.
  for (uint8_t i = 0; i < state.custom_mode.lockpoint_count; i++) {
    if (received_vehicle_speed <= state.custom_mode.lockpoints[i].speed) {
      lp_upper = state.custom_mode.lockpoints[i];
      lp_lower = state.custom_mode.lockpoints[(i == 0) ? 0 : (i - 1)];
      break;
    }
  }

  // Handle the case where the vehicle speed is lower than the lowest lockpoint.
  if (received_vehicle_speed <= lp_lower.speed) {
    return lp_lower.lock;
  }

  // Handle the case where the vehicle speed is higher than the highest lockpoint.
  if (received_vehicle_speed >= lp_upper.speed) {
    return lp_upper.lock;
  }

  // In all other cases, interpolation is necessary.
  float inter = (float)(lp_upper.speed - lp_lower.speed) / (float)(received_vehicle_speed - lp_lower.speed);

  // Calculate the target.
  float target = lp_lower.lock + ((float)(lp_upper.lock - lp_lower.lock) / inter);
  //DEBUG("lp_upper:%d@%d lp_lower:%d@%d speed:%d target:%0.2f", lp_upper.lock, lp_upper.speed, lp_lower.lock, lp_lower.speed, received_vehicle_speed, target);
  return target;
}

// Only executed when in MODE_FWD/MODE_5050/MODE_CUSTOM
uint8_t get_lock_target_adjusted_value(uint8_t value, bool invert) {
  // Handle 5050 mode.
  if (lock_target == 100) {
    // is this needed?  Should be caught in get_lock_target_adjustment
    if (int(received_pedal_value) >= state.pedal_threshold || received_vehicle_speed < disableSpeed || state.pedal_threshold == 0) {
      return (invert ? (0xFE - value) : value);
    }
    return (invert ? 0xFE : 0x00);
  }

  // Handle FWD and CUSTOM modes.
  // No correction is necessary if the target is already 0.
  if (lock_target == 0) {
    return (invert ? 0xFE : 0x00);
  }

  float correction_factor = ((float)lock_target / 2) + 20;
  uint8_t corrected_value = value * (correction_factor / 100);
  if (int(received_pedal_value) >= state.pedal_threshold || received_vehicle_speed < disableSpeed || state.pedal_threshold == 0) {
    return (invert ? (0xFE - corrected_value) : corrected_value);
  }
  return (invert ? 0xFE : 0x00);
}

// Only executed when in MODE_FWD/MODE_5050/MODE_CUSTOM
void getLockData(twai_message_t& rx_message_chs) {
  // Get the initial lock target.
  lock_target = get_lock_target_adjustment();

  // Edit the frames if configured as Gen1...
  if (haldexGeneration == 1) {
    switch (rx_message_chs.identifier) {
      case MOTOR1_ID:
        rx_message_chs.data[0] = 0x00;                                         // various individual bits ('space gas', driving pedal, kick down, clutch, timeout brake, brake intervention, drinks-torque intervention?) was 0x01 - ignored
        rx_message_chs.data[1] = get_lock_target_adjusted_value(0xFE, false);  // rpm low byte
        rx_message_chs.data[2] = 0x21;                                         // rpm high byte
        rx_message_chs.data[3] = get_lock_target_adjusted_value(0x4E, false);  // set RPM to a value so the pre-charge pump runs
        rx_message_chs.data[4] = get_lock_target_adjusted_value(0xFE, false);  // inner moment (%): 0.39*(0xF0) = 93.6%  (make FE?) - ignored
        rx_message_chs.data[5] = get_lock_target_adjusted_value(0xFE, false);  // driving pedal (%): 0.39*(0xF0) = 93.6%  (make FE?) - ignored
        appliedTorque = rx_message_chs.data[6];
        // rx_message_chs.data[6] = get_lock_target_adjusted_value(0x16, false);  // set to a low value to control the req. transfer torque.  Main control value for Gen1
        switch (state.mode) {
          case MODE_FWD:
            appliedTorque = get_lock_target_adjusted_value(0xFE, true);  // return 0xFE to disable
            break;
          case MODE_5050:
            appliedTorque = get_lock_target_adjusted_value(0x16, false);  // return 0x16 to fully lock
            break;
          case MODE_6040:
            appliedTorque = get_lock_target_adjusted_value(0x22, false);  // set to ~30% lock (0x96 = 15%, 0x56 = 27%)
            break;
          case MODE_7525:
            appliedTorque = get_lock_target_adjusted_value(0x50, false);  // set to ~30% lock (0x96 = 15%, 0x56 = 27%)
            break;
        }

        rx_message_chs.data[6] = appliedTorque;  // was 0x00
        rx_message_chs.data[7] = 0x00;           // these must play a factor - achieves ~169 without
        break;
      case MOTOR3_ID:
        rx_message_chs.data[2] = get_lock_target_adjusted_value(0xFE, false);  // pedal - ignored
        rx_message_chs.data[7] = get_lock_target_adjusted_value(0xFE, false);  // throttle angle (100%), ignored
        break;
      case BRAKES1_ID:
        rx_message_chs.data[1] = get_lock_target_adjusted_value(0x00, false);  // also controlling slippage.  Brake force can add 20%
        rx_message_chs.data[2] = 0x00;                                         //  ignored
        rx_message_chs.data[3] = get_lock_target_adjusted_value(0x0A, false);  // 0xA ignored?
        break;
      case BRAKES3_ID:
        rx_message_chs.data[0] = get_lock_target_adjusted_value(0xFE, false);  // low byte, LEFT Front // affects slightly +2
        rx_message_chs.data[1] = 0x0A;                                         // high byte, LEFT Front big effect
        rx_message_chs.data[2] = get_lock_target_adjusted_value(0xFE, false);  // low byte, RIGHT Front// affects slightly +2
        rx_message_chs.data[3] = 0x0A;                                         // high byte, RIGHT Front big effect
        rx_message_chs.data[4] = 0x00;                                         // low byte, LEFT Rear
        rx_message_chs.data[5] = 0x0A;                                         // high byte, LEFT Rear // 254+10? (5050 returns 0xA)
        rx_message_chs.data[6] = 0x00;                                         // low byte, RIGHT Rear
        rx_message_chs.data[7] = 0x0A;                                         // high byte, RIGHT Rear  // 254+10?
        break;
    }
  }

  // Edit the frames if configured as Gen2...
  if (haldexGeneration == 2) {  // Edit the frames if configured as Gen2.  Currently copied from Gen4...
    switch (rx_message_chs.identifier) {
      case MOTOR1_ID:
        rx_message_chs.data[1] = get_lock_target_adjusted_value(0xFE, false);
        rx_message_chs.data[2] = 0x21;
        rx_message_chs.data[3] = get_lock_target_adjusted_value(0x4E, false);
        rx_message_chs.data[6] = get_lock_target_adjusted_value(0xFE, false);
        break;
      case MOTOR3_ID:
        rx_message_chs.data[2] = get_lock_target_adjusted_value(0xFE, false);
        rx_message_chs.data[7] = get_lock_target_adjusted_value(0x01, false);  // gen1 is 0xFE, gen4 is 0x01
        break;
      case MOTOR6_ID:
        break;
      case BRAKES1_ID:
        rx_message_chs.data[0] = get_lock_target_adjusted_value(0x80, false);
        rx_message_chs.data[1] = get_lock_target_adjusted_value(0x41, false);
        rx_message_chs.data[2] = get_lock_target_adjusted_value(0xFE, false);  // gen1 is 0x00, gen4 is 0xFE
        rx_message_chs.data[3] = 0x0A;
        break;
      case BRAKES2_ID:
        rx_message_chs.data[4] = get_lock_target_adjusted_value(0x7F, false);  // big affect(!) 0x7F is max
        rx_message_chs.data[5] = get_lock_target_adjusted_value(0xFE, false);  // no effect.  Was 0x6E
        break;
      case BRAKES3_ID:
        rx_message_chs.data[0] = get_lock_target_adjusted_value(0xFE, false);
        rx_message_chs.data[1] = 0x0A;
        rx_message_chs.data[2] = get_lock_target_adjusted_value(0xFE, false);
        rx_message_chs.data[3] = 0x0A;
        rx_message_chs.data[4] = 0x00;
        rx_message_chs.data[5] = 0x0A;
        rx_message_chs.data[6] = 0x00;
        rx_message_chs.data[7] = 0x0A;
        break;
    }
  }
  // Edit the frames if configured as Gen4...
  if (haldexGeneration == 4) {
    switch (rx_message_chs.identifier) {
      case mLW_1:
        rx_message_chs.data[0] = lws_2[mLW_1_counter][0];  // angle of turn (block 011) low byte
        rx_message_chs.data[1] = lws_2[mLW_1_counter][1];  // no effect B high byte
        rx_message_chs.data[2] = lws_2[mLW_1_counter][2];  // no effect C
        rx_message_chs.data[3] = lws_2[mLW_1_counter][3];  // no effect D
        rx_message_chs.data[4] = lws_2[mLW_1_counter][4];  // rate of change (block 010) was 0x00
        rx_message_chs.data[5] = lws_2[mLW_1_counter][5];  // no effect F
        rx_message_chs.data[6] = lws_2[mLW_1_counter][6];  // no effect F
        rx_message_chs.data[7] = lws_2[mLW_1_counter][7];  // no effect F
        mLW_1_counter++;
        if (mLW_1_counter > 15) {
          mLW_1_counter = 0;
        }
        break;
      case MOTOR1_ID:
        rx_message_chs.data[1] = get_lock_target_adjusted_value(0xFE, false);  // has effect
        rx_message_chs.data[2] = get_lock_target_adjusted_value(0x20, false);  // RPM low byte no effect was 0x20
        rx_message_chs.data[3] = get_lock_target_adjusted_value(0x4E, false);  // RPM high byte.  Will disable pre-charge pump if 0x00.  Sets raw = 8, coupling open
        rx_message_chs.data[4] = get_lock_target_adjusted_value(0xFE, false);  // MDNORM no effect
        rx_message_chs.data[5] = get_lock_target_adjusted_value(0xFE, false);  // Pedal no effect
        rx_message_chs.data[6] = get_lock_target_adjusted_value(0x16, false);  // idle adaptation?  Was slippage?
        rx_message_chs.data[7] = get_lock_target_adjusted_value(0xFE, false);  // Fahrerwunschmoment req. torque?
        break;
      case MOTOR3_ID:
        //frame.data[2] = get_lock_target_adjusted_value(0xFE, false);
        //frame.data[7] = get_lock_target_adjusted_value(0x01, false);
        break;
      case BRAKES1_ID:
        rx_message_chs.data[0] = 0x20;                                         // ASR 0x04 sets bit 4.  0x08 removes set.  Coupling open/closed
        rx_message_chs.data[1] = 0x40;                                         // can use to disable (>130 dec).  Was 0x00; 0x41?  0x43?
        rx_message_chs.data[4] = get_lock_target_adjusted_value(0xFE, false);  // was 0xFE miasrl no effect
        rx_message_chs.data[5] = get_lock_target_adjusted_value(0xFE, false);  // was 0xFE miasrs no effect
        break;
      case BRAKES2_ID:
        rx_message_chs.data[4] = get_lock_target_adjusted_value(0x7F, false);  // big affect(!) 0x7F is max
        break;
      case BRAKES3_ID:
        rx_message_chs.data[0] = get_lock_target_adjusted_value(0xB6, false);  // front left low
        rx_message_chs.data[1] = 0x07;                                         // front left high
        rx_message_chs.data[2] = get_lock_target_adjusted_value(0xCC, false);  // front right low
        rx_message_chs.data[3] = 0x07;                                         // front right high
        rx_message_chs.data[4] = get_lock_target_adjusted_value(0xD2, false);  // rear left low
        rx_message_chs.data[5] = 0x07;                                         // rear left high
        rx_message_chs.data[6] = get_lock_target_adjusted_value(0xD2, false);  // rear right low
        rx_message_chs.data[7] = 0x07;                                         // rear right high
        break;

      case BRAKES4_ID:
        rx_message_chs.data[0] = get_lock_target_adjusted_value(0xFE, false);  // affects estimated torque AND vehicle mode(!)
        rx_message_chs.data[1] = 0x00;                                         //
        rx_message_chs.data[2] = 0x00;                                         //
        rx_message_chs.data[3] = 0x64;                                         // 32605
        rx_message_chs.data[4] = 0x00;                                         //
        rx_message_chs.data[5] = 0x00;                                         //
        rx_message_chs.data[6] = BRAKES4_counter;                              // checksum
        BRAKES4_crc = 0;
        for (uint8_t i = 0; i < 7; i++) {
          BRAKES4_crc ^= rx_message_chs.data[i];
        }
        rx_message_chs.data[7] = BRAKES4_crc;

        BRAKES4_counter = BRAKES4_counter + 16;
        if (BRAKES4_counter > 0xF0) {
          BRAKES4_counter = 0x00;
        }
        break;
    }
  }
}