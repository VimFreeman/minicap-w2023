#ifndef MQ135_H
#define MQ135_H

#define ANALOG_DATA_PIN
//function to read ppm, needs to use ADC
void MQ135_readppm(uint8_t* data, const uint8_t const maxLen);

#endif