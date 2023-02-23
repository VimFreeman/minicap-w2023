#include "DHT22.h"

void DHT22_init() {
  TIMG0_T0CONFIG_REG |= (1 << 31); //enalbe timer 0 0
  DHT22_start_count = 0;
  DHT22_end_count = 0;
  DHT_status = 0;
}

void DHT22_read_data(uint8_t* data) {
  #if DATA_PIN < 32
    GPIO_ENABLE_W1TS_REG = 1 << DATA_PIN;
    GPIO_OUT_W1TC_REG = 1 << DATA_PIN; 
  #elif DATA_PIN > 31 && DATA_PIN < 40
    GPIO_ENABLE1_W1TS_REG = 1 << (DATA_REG - 32);
    GPIO_OUT1_W1TC_REG = 1 << (DATA_REG - 32);
  #endif
  /* above section sets the pin as output and then sets it low*/
  delay(1); /*wait one second to assert wake signal*/

  #if DATA_PIN < 32
    GPIO_OUT_W1TS_REG = 1 << DATA_PIN; 
  #elif DATA_PIN > 31 && DATA_PIN < 40
    GPIO_OUT1_W1TS_REG = 1 << (DATA_REG - 32);
  #endif
  /*set signal high*/
  delayMicroseconds(40);

  #if DATA_PIN < 32
    GPIO_ENABLE_W1TC_REG = 1 << DATA_PIN;
  #elif DATA_PIN > 31 && DATA_PIN < 40
    GPIO_ENABLE1_W1TC_REG = 1 << (DATA_REG - 32);
  #endif
  /*set pin as input*/

  uint8_t index = 0;
  uint8_t temp = 0;
  for( uint8_t i = 0; i < 40; i++ ) {
      temp = read_bit() << index;
      index++;
      if (index == 8) {
        data[(i/8) - 1] = temp;
        temp = index = 0;
      }
  }
}


uint8_t read_bit() {
  #if DATA_PIN < 32
    return (GPIO_IN_REG & DATA_PIN) >> DATA_PIN;
  #else
     return (GPIO_IN1_REG & (DATA_PIN - 32)) >> (DATA_PIN - 32);
  #endif
}
