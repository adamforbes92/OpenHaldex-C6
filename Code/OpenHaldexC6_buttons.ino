void modeChange(void) {
#if enableDebug
  DEBUG("Internal mode button pressed!");
#endif
  // Determine the next mode in the sequence.
  uint8_t next_mode = (uint8_t)state.mode + 1;

  // If the next mode is valid, change the current mode to it.
  if (next_mode < (uint8_t)openhaldex_mode_t_MAX - 1) {
    state.mode = (openhaldex_mode_t)next_mode;
  }
  // On overflow, start over from MODE_STOCK.
  else {
    state.mode = MODE_STOCK;
  }
}

void modeChangeExt(void) {
#if enableDebug
  DEBUG("External mode button pressed!");
#endif

  // Determine the next mode in the sequence.
  uint8_t next_mode = (uint8_t)state.mode + 1;

  // If the next mode is valid, change the current mode to it.
  if (next_mode < (uint8_t)openhaldex_mode_t_MAX - 1) {
    state.mode = (openhaldex_mode_t)next_mode;
  }
  // On overflow, start over from MODE_STOCK.
  else {
    state.mode = MODE_STOCK;
  }
}