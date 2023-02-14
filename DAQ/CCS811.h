#ifndef CCS881_H
#define CCS881_H

#define WAKE_PIN
#define RESET_PIN
#define CCS881_ADDR

//blocking init function
void ccs881_init();

//function to read regAddr of len bytes
void ccs881_read_reg(uint8_t* data, const uint8_t regAddr, const uint8_t len);

//function to write data to to regaddr of size len bytes
void ccs881_write_reg(const uint8_t* const data, const uint8_t regAddr, const uint8_t len);

//function to get co2 and voc results
void ccs881_read_results(uint16_t* c02, uint16_t* voc);

#endif