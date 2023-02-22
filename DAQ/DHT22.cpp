#include "DHT22.h"

void DHT22_init() {
  TIMG0_T0CONFIG_REG |= (1 << 31); //enalbe timer 0 0
  DHT22_start_count = 0;
  DHT22_end_count = 0;
  DHT_status = 0;
}

void DHT22_read_data(uint8_t* data, const uint8_t maxLen) {
  #if DATA_PIN < 32

  #elif DATA_PIN > 31 && DATA_PIN < 40

  #else 

  #endif
}
