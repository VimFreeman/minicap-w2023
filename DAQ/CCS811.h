#ifndef CCS881_H
#define CCS881_H

#include <Wire.h>

#define WAKE_PIN 1
#define RESET_PIN 1
#define CCS881_ADDR 1

#define MEAS_MODE 1
#define ALG_RESULT_DATA 2

#define MODE 3
#define INT_DATARDY 0
#define INT_THRESH 0


//blocking init function
void ccs881_init();

//function to read regAddr of len bytes
void ccs881_read_reg(uint8_t* data, const uint8_t regAddr, const uint8_t len);

//function to write data to to regaddr of size len bytes
void ccs881_write_reg(const uint8_t* const data, const uint8_t regAddr, const uint8_t len);

//function to get co2 and voc results
void ccs881_read_results(uint16_t* co2, uint16_t* voc);

#endif
