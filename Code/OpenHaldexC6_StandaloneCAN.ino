void Gen1_frames10() {  // n/a
}

void Gen1_frames20() {
  twai_message_t frame;
  frame.identifier = MOTOR1_ID;  // 0x1A0
  frame.data_length_code = 8;    // DLC 8
  frame.data[0] = 0x00;          // ASR 0x04 sets bit 4.  0x08 removes set.  Coupling open/closed
  frame.data[1] = 0xFE;          // can use to disable (>130 dec).  Was 0x00; 0x41?  0x43?
  frame.data[2] = 0x21;          // was 0x00 no effect
  frame.data[3] = 0x4E;          // was 0xFE no effect
  frame.data[4] = 0x00;          // was 0xFE miasrl no effect
  frame.data[5] = 0x00;          // was 0xFE miasrs no effect
  frame.data[6] = 0x16;          // was 0x00
  frame.data[7] = 0x00;          // checksum
  twai_transmit_v2(twai_bus_1, &frame, 0);

  frame.identifier = BRAKES1_ID;    // 0x1A0
  frame.data_length_code = 8;       // DLC 8
  frame.data[0] = 0x80;             // ASR 0x04 sets bit 4.  0x08 removes set.  Coupling open/closed
  frame.data[1] = 0x00;             // can use to disable (>130 dec).  Was 0x00; 0x41?  0x43?
  frame.data[2] = 0x00;             // was 0x00 no effect
  frame.data[3] = 0x0A;             // was 0xFE no effect
  frame.data[4] = 0xFE;             // was 0xFE miasrl no effect
  frame.data[5] = 0xFE;             // was 0xFE miasrs no effect
  frame.data[6] = 0x00;             // was 0x00
  frame.data[7] = BRAKES1_counter;  // checksum
  if (++BRAKES1_counter > 0xF) {    // 0xF
    BRAKES1_counter = 0;            // 0
  }
  twai_transmit_v2(twai_bus_1, &frame, 0);

  /*
  Bremse 3 has massive effect - deviation between front/rear = block 011 'rpm changing'
  too high a speed causes 'pulsing'
  slip control changes from 0 to 1 IF rear > front
  */
  frame.identifier = BRAKES3_ID;  // 0x4A0 - deviation between front/rear = block 011 'rpm'
  frame.data_length_code = 8;     // DLC 8
  frame.data[0] = 0xFE;           // front left low
  frame.data[1] = 0x0A;           // front left high
  frame.data[2] = 0xFE;           // front right low
  frame.data[3] = 0x0A;           // front right high
  frame.data[4] = 0x00;           // rear left low
  frame.data[5] = 0x0A;           // rear left high
  frame.data[6] = 0x00;           // rear right low
  frame.data[7] = 0x0A;           // rear right high
  twai_transmit_v2(twai_bus_1, &frame, 0);
}

void Gen1_frames25() {  // n/a
}

void Gen1_frames100() {  // n/a
}

void Gen1_frames200() {  // n/a
}

void Gen1_frames1000() {  // n/a
}

void Gen2_frames10() {
  twai_message_t frame;
  frame.identifier = BRAKES1_ID;    // 0x1A0
  frame.data_length_code = 8;       // DLC 8
  frame.data[0] = 0x00;             // ASR 0x04 sets bit 4.  0x08 removes set.  Coupling open/closed
  frame.data[1] = 0x41;             // can use to disable (>130 dec).  Was 0x00; 0x41?  0x43?
  frame.data[2] = 0x00;             // was 0x00 no effect
  frame.data[3] = 0xFE;             // was 0xFE no effect
  frame.data[4] = 0xFE;             // was 0xFE miasrl no effect
  frame.data[5] = 0xFE;             // was 0xFE miasrs no effect
  frame.data[6] = 0x00;             // was 0x00
  frame.data[7] = BRAKES1_counter;  // checksum
  if (++BRAKES1_counter > 0xF) {    // 0xF
    BRAKES1_counter = 0;            // 0
  }
  twai_transmit_v2(twai_bus_1, &frame, 0);

  frame.identifier = BRAKES2_ID;    // 0x1A0
  frame.data_length_code = 8;       // DLC 8
  frame.data[0] = 0x7F;             // ASR 0x04 sets bit 4.  0x08 removes set.  Coupling open/closed
  frame.data[1] = 0xAE;             // can use to disable (>130 dec).  Was 0x00; 0x41?  0x43?
  frame.data[2] = 0x3D;             // was 0x00 no effect
  frame.data[3] = BRAKES2_counter;  // was 0xFE no effect
  frame.data[4] = 0x7F;             // was 0xFE miasrl no effect
  frame.data[5] = 0xFE;             // was 0xFE miasrs no effect
  frame.data[6] = 0x5E;             // was 0x00
  frame.data[7] = 0x2B;             // checksum
  BRAKES2_counter = BRAKES2_counter + 10;
  if (BRAKES2_counter > 0xF7) {
    BRAKES2_counter = 7;
  }
  twai_transmit_v2(twai_bus_1, &frame, 0);

  /*
  Bremse 3 has massive effect - deviation between front/rear = block 011 'rpm changing'
  too high a speed causes 'pulsing'
  slip control changes from 0 to 1 IF rear > front
  */
  frame.identifier = BRAKES3_ID;  // 0x4A0 - deviation between front/rear = block 011 'rpm'
  frame.data_length_code = 8;     // DLC 8
  frame.data[0] = 0xFE;           // front left low
  frame.data[1] = 0x0A;           // front left high
  frame.data[2] = 0xFE;           // front right low
  frame.data[3] = 0x0A;           // front right high
  frame.data[4] = 0x00;           // rear left low
  frame.data[5] = 0x0A;           // rear left high
  frame.data[6] = 0x00;           // rear right low
  frame.data[7] = 0x0A;           // rear right high
  twai_transmit_v2(twai_bus_1, &frame, 0);

  frame.identifier = BRAKES4_ID;    // 0x2A0 includes coupling moment? Affects vehicle mode(!) do not have!
  frame.data_length_code = 8;       // DLC 8 (/4)
  frame.data[0] = 0x00;             // affects estimated torque AND vehicle mode(!)
  frame.data[1] = 0x00;             //
  frame.data[2] = 0x00;             //
  frame.data[3] = 0x00;             // 32605
  frame.data[4] = 0x00;             //
  frame.data[5] = 0x00;             //
  frame.data[6] = BRAKES4_counter;  // checksum
  frame.data[7] = BRAKES4_counter;  // checksum
  BRAKES4_counter = BRAKES4_counter + 10;
  if (BRAKES4_counter > 0xF0) {
    BRAKES4_counter = 0;
  }
  twai_transmit_v2(twai_bus_1, &frame, 0);

  frame.identifier = BRAKES5_ID;     // 0x2A0 includes coupling moment? Affects vehicle mode(!) do not have!
  frame.data_length_code = 8;        // DLC 8 (/4)
  frame.data[0] = 0x00;              // affects estimated torque AND vehicle mode(!)
  frame.data[1] = 0x00;              //
  frame.data[2] = 0x00;              //
  frame.data[3] = 0x00;              // 32605
  frame.data[4] = 0x00;              //
  frame.data[5] = 0x00;              //
  frame.data[6] = BRAKES5_counter;   // checksum
  frame.data[7] = BRAKES5_counter2;  // checksum
  BRAKES5_counter = BRAKES5_counter + 10;
  if (BRAKES5_counter > 0xF0) {
    BRAKES5_counter = 0;
  }
  BRAKES5_counter2 = BRAKES5_counter2 + 10;
  if (BRAKES5_counter2 > 0xF3) {
    BRAKES5_counter2 = 3;
  }
  twai_transmit_v2(twai_bus_1, &frame, 0);

  frame.identifier = BRAKES9_ID;     // 0x0AE do not have!
  frame.data_length_code = 8;        // DLC 8
  frame.data[0] = BRAKES9_counter;   // checksum
  frame.data[1] = BRAKES9_counter2;  // checksum
  frame.data[2] = 0x00;              // no effect
  frame.data[3] = 0x00;              // no effect
  frame.data[4] = 0x00;              // no effect
  frame.data[5] = 0x00;              // no effect
  frame.data[6] = 0x02;              // 0x01
  frame.data[7] = 0x00;              // no effect
  twai_transmit_v2(twai_bus_1, &frame, 0);
  BRAKES9_counter = BRAKES9_counter + 10;
  if (BRAKES9_counter > 0xF1) {
    BRAKES9_counter = 0x11;
  }
  BRAKES9_counter2 = BRAKES9_counter2 + 10;
  if (BRAKES9_counter2 > 0xF0) {
    BRAKES9_counter2 = 0x00;
  }

  frame.identifier = MOTOR1_ID;  // 0x280 - needs this
  frame.data_length_code = 8;
  frame.data[0] = 0x01;  // various bits no effect 0x08
  frame.data[1] = 0xFE;  // MDNORM no effect x
  frame.data[2] = 0x20;  // RPM low byte no effect was 0x20
  frame.data[3] = 0x4E;  // RPM high byte.  Will disable pre-charge pump if 0x00.  Sets raw = 8, coupling open
  frame.data[4] = 0xFE;  // MDNORM no effect
  frame.data[5] = 0xFE;  // Pedal no effect
  frame.data[6] = 0x20;  // idle adaptation?  Was slippage?
  frame.data[7] = 0xFE;  // Fahrerwunschmoment req. torque?
  twai_transmit_v2(twai_bus_1, &frame, 0);

  frame.identifier = mLW_1;  // electronic power steering 0x0C2
  frame.extd = 0;
  frame.rtr = 0;
  frame.data_length_code = 8;
  frame.data[0] = 0x20;           // various bits no effect 0x08
  frame.data[1] = 0x00;           // MDNORM no effect x
  frame.data[2] = 0x00;           // RPM low byte no effect was 0x20
  frame.data[3] = 0x00;           // RPM high byte.  Will disable pre-charge pump if 0x00.  Sets raw = 8, coupling open
  frame.data[4] = 0x80;           // MDNORM no effect
  frame.data[5] = mLW_1_counter;  // Pedal no effect
  frame.data[6] = 0x00;           // idle adaptation?  Was slippage?
  // now calculate checksum (0xFF - bytes 0, 1, 2, 3 + 5)
  mLW_1_crc = 255 - (frame.data[0] + frame.data[1] + frame.data[2] + frame.data[3] + frame.data[5]);
  frame.data[7] = mLW_1_crc;  //0x8F;          // no effect H
  mLW_1_counter = mLW_1_counter + 16;
  if (mLW_1_counter >= 0xF0) {
    mLW_1_counter = 0;
  }
  twai_transmit_v2(twai_bus_1, &frame, 0);
}

void Gen2_frames20() {
  twai_message_t frame;
  frame.identifier = MOTOR1_ID;  // 0x5A0
  frame.data_length_code = 8;    // DLC 8
  frame.data[0] = 0x08;          // various bits // was 7E
  frame.data[1] = 0xFA;          // outside temp no effect
  frame.data[2] = 0x20;          // Pedal no effect
  frame.data[3] = 0x4E;          // checksum changing
  frame.data[4] = 0xFA;          // big affect(!) 0x7F is max
  frame.data[5] = 0xFA;          // no effect.  Was 0x6E
  frame.data[6] = 0x20;          // no effect.  Was 0x70 - can cause 'Operating Mode Malfunction'(!) Includes Emergency Mode and Control Module Error
  frame.data[7] = 0xFA;          // no effect.  gen1 is FE, Gen4 is 01 was AB - can cause 'Operating Mode Malfunction'(!)
  twai_transmit_v2(twai_bus_1, &frame, 0);

  frame.identifier = MOTOR2_ID;  // 0x5A0
  frame.data_length_code = 8;    // DLC 8
  frame.data[0] = 0x00;          // various bits // was 7E
  frame.data[1] = 0x30;          // outside temp no effect
  frame.data[2] = 0x00;          // Pedal no effect
  frame.data[3] = 0xA;           // checksum changing
  frame.data[4] = 0xA;           // big affect(!) 0x7F is max
  frame.data[5] = 0x10;          // no effect.  Was 0x6E
  frame.data[6] = 0xFE;          // no effect.  Was 0x70 - can cause 'Operating Mode Malfunction'(!) Includes Emergency Mode and Control Module Error
  frame.data[7] = 0xFE;          // no effect.  gen1 is FE, Gen4 is 01 was AB - can cause 'Operating Mode Malfunction'(!)
  twai_transmit_v2(twai_bus_1, &frame, 0);

  frame.identifier = MOTOR5_ID;    // 0x1A0
  frame.data_length_code = 8;      // DLC 8
  frame.data[0] = 0xFE;            // ASR 0x04 sets bit 4.  0x08 removes set.  Coupling open/closed
  frame.data[1] = 0x00;            // can use to disable (>130 dec).  Was 0x00; 0x41?  0x43?
  frame.data[2] = 0x00;            // was 0x00 no effect
  frame.data[3] = 0x00;            // was 0xFE no effect
  frame.data[4] = 0x00;            // was 0xFE miasrl no effect
  frame.data[5] = 0x00;            // was 0xFE miasrs no effect
  frame.data[6] = 0x00;            // was 0x00
  frame.data[7] = MOTOR5_counter;  // checksum
  if (++BRAKES1_counter > 255) {   // 0xF
    BRAKES1_counter = 0;           // 0
  }
  twai_transmit_v2(twai_bus_1, &frame, 0);

  frame.identifier = BRAKES10_ID;    // 0x1A0
  frame.data_length_code = 8;        // DLC 8
  frame.data[0] = 0xA6;              // ASR 0x04 sets bit 4.  0x08 removes set.  Coupling open/closed
  frame.data[1] = BRAKES10_counter;  // can use to disable (>130 dec).  Was 0x00; 0x41?  0x43?
  frame.data[2] = 0x75;              // was 0x00 no effect
  frame.data[3] = 0xD4;              // was 0xFE no effect
  frame.data[4] = 0x51;              // was 0xFE miasrl no effect
  frame.data[5] = 0x47;              // was 0xFE miasrs no effect
  frame.data[6] = 0x1D;              // was 0x00
  frame.data[7] = 0x0F;              // checksum
  BRAKES10_counter = BRAKES10_counter + 1;
  if (BRAKES10_counter > 0xF) {
    BRAKES10_counter = 0;
  }
  twai_transmit_v2(twai_bus_1, &frame, 0);
}

void Gen2_frames25() {
  twai_message_t frame;
  frame.identifier = mKombi_1;  // kombi 1
  frame.data_length_code = 8;   // DLC 8
  frame.data[0] = 0x00;         // angle of turn (block 011) low byte
  frame.data[1] = 0x02;         // no effect B high byte
  frame.data[2] = 0x00;         // no effect C
  frame.data[3] = 0x00;         // no effect D
  frame.data[4] = 0x36;         // rate of change (block 010)
  frame.data[5] = 0x00;         // rate of change (block 010) checksum?
  frame.data[6] = 0x00;         // rate of change (block 010)
  frame.data[7] = 0x00;         // rate of change (block 010)
  twai_transmit_v2(twai_bus_1, &frame, 0);
}

void Gen2_frames100() {  // n/a
}

void Gen2_frames200() {  // n/a
}

void Gen2_frames1000() {  // n/a
}

void Gen4_frames10() {
  twai_message_t frame;
  frame.identifier = mLW_1;  // electronic power steering 0x0C2
  frame.extd = 0;
  frame.rtr = 0;
  frame.data_length_code = 8;
  frame.data[0] = lws_2[mLW_1_counter][0];  // angle of turn (block 011) low byte
  frame.data[1] = lws_2[mLW_1_counter][1];  // no effect B high byte
  frame.data[2] = lws_2[mLW_1_counter][2];  // no effect C
  frame.data[3] = lws_2[mLW_1_counter][3];  // no effect D
  frame.data[4] = lws_2[mLW_1_counter][4];  // rate of change (block 010) was 0x00
  frame.data[5] = lws_2[mLW_1_counter][5];  // no effect F
  frame.data[6] = lws_2[mLW_1_counter][6];  // no effect F
  frame.data[7] = lws_2[mLW_1_counter][7];  // no effect F

  mLW_1_counter++;
  if (mLW_1_counter > 15) {
    mLW_1_counter = 0;
  }
  twai_transmit_v2(twai_bus_1, &frame, 0);

  frame.identifier = BRAKES1_ID;                                // 0x1A0
  frame.data_length_code = 8;                                   // DLC 8
  frame.data[0] = 0x20;                                         // ASR 0x04 sets bit 4.  0x08 removes set.  Coupling open/closed
  frame.data[1] = 0x40;                                         // can use to disable (>130 dec).  Was 0x00; 0x41?  0x43?
  frame.data[2] = 0xF0;                                         // was 0x00 no effect
  frame.data[3] = 0x07;                                         // was 0xFE no effect
  frame.data[4] = get_lock_target_adjusted_value(0xFE, false);  // was 0xFE miasrl no effect
  frame.data[5] = get_lock_target_adjusted_value(0xFE, false);  // was 0xFE miasrs no effect
  frame.data[6] = 0x00;                                         // was 0x00
  frame.data[7] = BRAKES1_counter;                              // checksum
  if (++BRAKES1_counter > 0x1F) {                               // 0xF
    BRAKES1_counter = 10;                                       // 0
  }
  twai_transmit_v2(twai_bus_1, &frame, 0);

  /*
  Bremse 3 has massive effect - deviation between front/rear = block 011 'rpm changing'
  too high a speed causes 'pulsing'
  slip control changes from 0 to 1 IF rear > front
  */
  frame.identifier = BRAKES3_ID;                                // 0x4A0 - deviation between front/rear = block 011 'rpm'
  frame.data_length_code = 8;                                   // DLC 8
  frame.data[0] = get_lock_target_adjusted_value(0xB6, false);  // front left low
  frame.data[1] = 0x07;                                         // front left high
  frame.data[2] = get_lock_target_adjusted_value(0xCC, false);  // front right low
  frame.data[3] = 0x07;                                         // front right high
  frame.data[4] = get_lock_target_adjusted_value(0xD2, false);  // rear left low
  frame.data[5] = 0x07;                                         // rear left high
  frame.data[6] = get_lock_target_adjusted_value(0xD2, false);  // rear right low
  frame.data[7] = 0x07;                                         // rear right high
  twai_transmit_v2(twai_bus_1, &frame, 0);

  frame.identifier = BRAKES4_ID;                                // 0x2A0 includes coupling moment? Affects vehicle mode(!) do not have!
  frame.data_length_code = 8;                                   // DLC 8 (/4)
  frame.data[0] = get_lock_target_adjusted_value(0xFE, false);  // affects estimated torque AND vehicle mode(!)
  frame.data[1] = 0x00;                                         //
  frame.data[2] = 0x00;                                         //
  frame.data[3] = 0x64;                                         // 32605
  frame.data[4] = 0x00;                                         //
  frame.data[5] = 0x00;                                         //
  frame.data[6] = BRAKES4_counter;                              // checksum
  BRAKES4_crc = 0;
  for (uint8_t i = 0; i < 7; i++) {
    BRAKES4_crc ^= frame.data[i];
  }
  frame.data[7] = BRAKES4_crc;

  BRAKES4_counter = BRAKES4_counter + 16;
  if (BRAKES4_counter > 0xF0) {
    BRAKES4_counter = 0x00;
  }
  twai_transmit_v2(twai_bus_1, &frame, 0);

  frame.identifier = BRAKES9_ID;     // 0x0AE do not have!
  frame.data_length_code = 8;        // DLC 8
  frame.data[0] = BRAKES9_counter;   // checksum
  frame.data[1] = BRAKES9_counter2;  // checksum
  frame.data[2] = 0x00;              // no effect
  frame.data[3] = 0x00;              // no effect
  frame.data[4] = 0x00;              // no effect
  frame.data[5] = 0x00;              // no effect
  frame.data[6] = 0x03;              // 0x01
  frame.data[7] = 0x00;              // no effect
  twai_transmit_v2(twai_bus_1, &frame, 0);
  BRAKES9_counter = BRAKES9_counter + 16;
  if (BRAKES9_counter > 0xF3) {
    BRAKES9_counter = 0x03;
  }
  BRAKES9_counter2 = BRAKES9_counter2 + 16;
  if (BRAKES9_counter2 > 0xF0) {
    BRAKES9_counter2 = 0x00;
  }

  frame.identifier = MOTOR1_ID;  // 0x280 - needs this
  frame.data_length_code = 8;
  frame.data[1] = get_lock_target_adjusted_value(0xFE, false);  // has effect
  frame.data[2] = get_lock_target_adjusted_value(0x20, false);  // RPM low byte no effect was 0x20
  frame.data[3] = get_lock_target_adjusted_value(0x4E, false);  // RPM high byte.  Will disable pre-charge pump if 0x00.  Sets raw = 8, coupling open
  frame.data[4] = get_lock_target_adjusted_value(0xFE, false);  // MDNORM no effect
  frame.data[5] = get_lock_target_adjusted_value(0xFE, false);  // Pedal no effect
  frame.data[6] = get_lock_target_adjusted_value(0x16, false);  // idle adaptation?  Was slippage?
  frame.data[7] = get_lock_target_adjusted_value(0xFE, false);  // Fahrerwunschmoment req. torque?
  twai_transmit_v2(twai_bus_1, &frame, 0);
}

void Gen4_frames20() {
  twai_message_t frame;
  frame.identifier = BRAKES2_ID;    // 0x5A0
  frame.data_length_code = 8;       // DLC 8
  frame.data[0] = 0x80;             // various bits // was 7E
  frame.data[1] = 0x7A;             // outside temp no effect
  frame.data[2] = 0x05;             // Pedal no effect
  frame.data[3] = BRAKES2_counter;  // checksum changing
  frame.data[4] = get_lock_target_adjusted_value(0x7F, false);             // big affect(!) 0x7F is max
  frame.data[5] = 0xCA;             // no effect.  Was 0x6E
  frame.data[6] = 0x1B;             // no effect.  Was 0x70 - can cause 'Operating Mode Malfunction'(!) Includes Emergency Mode and Control Module Error
  frame.data[7] = 0xAB;             // no effect.  gen1 is FE, Gen4 is 01 was AB - can cause 'Operating Mode Malfunction'(!)
  twai_transmit_v2(twai_bus_1, &frame, 0);
  BRAKES2_counter = BRAKES2_counter + 16;
  if (BRAKES2_counter > 0xF0) {
    BRAKES2_counter = 0;
  }
}

void Gen4_frames25() {
  twai_message_t frame;
  frame.identifier = mKombi_1;  // kombi 1
  frame.data_length_code = 8;   // DLC 8
  frame.data[0] = 0x24;         // angle of turn (block 011) low byte
  frame.data[1] = 0x00;         // no effect B high byte
  frame.data[2] = 0x1D;         // no effect C
  frame.data[3] = 0xB9;         // no effect D
  frame.data[4] = 0x07;         // rate of change (block 010)
  frame.data[5] = 0x42;         // rate of change (block 010) checksum?
  frame.data[6] = 0x09;         // rate of change (block 010)
  frame.data[7] = 0x81;         // rate of change (block 010)
  twai_transmit_v2(twai_bus_1, &frame, 0);

  frame.identifier = mKombi_3;  // 0x520 - kombi 3
  frame.data_length_code = 8;   // DLC 8
  frame.data[0] = 0x60;         // angle of turn (block 011) low byte
  frame.data[1] = 0x43;         // no effect B high byte
  frame.data[2] = 0x01;         // no effect C
  frame.data[3] = 0x10;         // no effect D
  frame.data[4] = 0x66;         // rate of change (block 010)
  frame.data[5] = 0xF1;         // rate of change (block 010) checksum?
  frame.data[6] = 0x03;         // rate of change (block 010)
  frame.data[7] = 0x02;         // rate of change (block 010)
  twai_transmit_v2(twai_bus_1, &frame, 0);
}

void Gen4_frames100() {
  twai_message_t frame;
  frame.identifier = mGate_Komf_1;  // electronic power steering 0x0C2
  frame.data_length_code = 8;       // DLC 8
  frame.data[0] = 0x03;             // angle of turn (block 011) low byte
  frame.data[1] = 0x11;             // no effect B high byte
  frame.data[2] = 0x58;             // no effect C
  frame.data[3] = 0x00;             // no effect D
  frame.data[4] = 0x40;             // rate of change (block 010)
  frame.data[5] = 0x00;             // rate of change (block 010)
  frame.data[6] = 0x01;             // rate of change (block 010)
  frame.data[7] = 0x08;             // rate of change (block 010)
  twai_transmit_v2(twai_bus_1, &frame, 0);

  frame.identifier = mGate_Komf_2;  // electronic power steering 0x0C2
  frame.data_length_code = 8;       // DLC 8
  frame.data[0] = 0x09;             // angle of turn (block 011) low byte
  frame.data[1] = 0x01;             // no effect B high byte
  frame.data[2] = 0x00;             // no effect C
  frame.data[3] = 0xA1;             // no effect D
  frame.data[4] = 0x00;             // rate of change (block 010)
  frame.data[5] = 0x00;             // rate of change (block 010)
  frame.data[6] = 0x00;             // rate of change (block 010)
  frame.data[7] = 0x00;             // rate of change (block 010)
  twai_transmit_v2(twai_bus_1, &frame, 0);

  frame.identifier = mSysteminfo_1;  // electronic power steering 0x0C2
  frame.data_length_code = 6;        // DLC 8
  frame.data[0] = 0xC0;              // angle of turn (block 011) low byte
  frame.data[1] = 0x03;              // no effect B high byte
  frame.data[2] = 0x50;              // no effect C
  frame.data[3] = 0xBF;              // no effect D
  frame.data[4] = 0x37;              // rate of change (block 010)
  frame.data[5] = 0x56;              // rate of change (block 010)
  frame.data[6] = 0xC0;              // rate of change (block 010)
  frame.data[7] = 0x00;              // rate of change (block 010)
  twai_transmit_v2(twai_bus_1, &frame, 0);

  frame.identifier = mSoll_Verbauliste_neu;  // electronic power steering 0x0C2
  frame.data_length_code = 8;                // DLC 8
  frame.data[0] = 0xF7;                      // angle of turn (block 011) low byte
  frame.data[1] = 0x42;                      // no effect B high byte
  frame.data[2] = 0x70;                      // no effect C
  frame.data[3] = 0x3F;                      // no effect D
  frame.data[4] = 0x1C;                      // rate of change (block 010)
  frame.data[5] = 0x08;                      // rate of change (block 010)
  frame.data[6] = 0x00;                      // rate of change (block 010)
  frame.data[7] = 0xC8;                      // rate of change (block 010)
  twai_transmit_v2(twai_bus_1, &frame, 0);

  frame.identifier = BRAKES11_ID;  // 0x3A0
  frame.data_length_code = 8;      // DLC 8
  frame.data[0] = 0x00;            // no effect
  frame.data[1] = 0XC0;            // checksum
  frame.data[2] = 0x00;            // no effect
  frame.data[3] = 0x00;            // no effect
  frame.data[4] = 0x00;            // no effect
  frame.data[5] = 0x00;            // no effect
  frame.data[6] = 0x00;            // no effect
  frame.data[7] = 0x00;            // no effect
  twai_transmit_v2(twai_bus_1, &frame, 0);
}

void Gen4_frames200() {
  twai_message_t frame;
  frame.identifier = mKombi_2;  // electronic power steering 0x0C2
  frame.data_length_code = 8;   // DLC 8
  frame.data[0] = 0x4C;         // angle of turn (block 011) low byte
  frame.data[1] = 0x86;         // no effect B high byte
  frame.data[2] = 0x85;         // no effect C
  frame.data[3] = 0x00;         // no effect D
  frame.data[4] = 0x00;         // rate of change (block 010)
  frame.data[5] = 0x30;         // rate of change (block 010)
  frame.data[6] = 0xFF;         // rate of change (block 010)
  frame.data[7] = 0x04;         // rate of change (block 010)
  twai_transmit_v2(twai_bus_1, &frame, 0);

  frame.identifier = mKombi_3;  // electronic power steering 0x0C2
  frame.data_length_code = 8;   // DLC 8
  frame.data[0] = 0xA6;         // angle of turn (block 011) low byte checksum?
  frame.data[1] = 0x87;         // no effect B high bytechecksum?
  frame.data[2] = 0x01;         // no effect C
  frame.data[3] = 0x10;         // no effect D
  frame.data[4] = 0x66;         // rate of change (block 010)
  frame.data[5] = 0xF2;         // rate of change (block 010)
  frame.data[6] = 0x03;         // rate of change (block 010)
  frame.data[7] = 0x02;         // rate of change (block 010)
  twai_transmit_v2(twai_bus_1, &frame, 0);

  frame.identifier = NMH_Gateway;  // electronic power steering 0x0C2
  frame.data_length_code = 7;      // DLC 8
  frame.data[0] = 0x04;            // angle of turn (block 011) low byte
  frame.data[1] = 0x03;            // no effect B high byte
  frame.data[2] = 0x01;            // no effect C
  frame.data[3] = 0x00;            // no effect D
  frame.data[4] = 0x02;            // rate of change (block 010)
  frame.data[5] = 0x00;            // rate of change (block 010)
  frame.data[6] = 0x00;            // rate of change (block 010)
  twai_transmit_v2(twai_bus_1, &frame, 0);
}

void Gen4_frames1000() {
  twai_message_t frame;
  frame.identifier = mDiagnose_1;       // electronic power steering 0x0C2
  frame.data_length_code = 8;           // DLC 8
  frame.data[0] = 0x26;                 // angle of turn (block 011) low byte
  frame.data[1] = 0xF2;                 // no effect B high byte
  frame.data[2] = 0x03;                 // no effect C
  frame.data[3] = 0x12;                 // no effect D
  frame.data[4] = 0x70;                 // rate of change (block 010)
  frame.data[5] = 0x19;                 // rate of change (block 010)
  frame.data[6] = 0x25;                 // rate of change (block 010)
  frame.data[7] = mDiagnose_1_counter;  // rate of change (block 010) checksum?
  twai_transmit_v2(twai_bus_1, &frame, 0);
  mDiagnose_1_counter++;
  if (mDiagnose_1_counter > 0x1F) {
    mDiagnose_1_counter = 0;
  }
}