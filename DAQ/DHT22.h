#ifndef DHT22_H
#define DHT22_H

#define DATA_PIN

//init function for timer
void DHT22_init();

//function to get reading from DHT22, reads data serially using time of high pulses
void DHT22_read_data(uint8_t* data, const uint8_t maxLen);

#endif