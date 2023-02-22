#include "CCS811.h"

void ccs881_init() {
    pinMode(WAKE_PIN, OUTPUT);
    digitalWrite(WAKE_PIN, HIGH);
    delayMicroseconds(50);
    uint8_t temp = MODE | INT_DATARDY | INT_THRESH;
    ccs881_write_reg(&temp, MEAS_MODE, 1);

}

void ccs881_read_reg(uint8_t* data, const uint8_t regAddr, const uint8_t len) {
    Wire.beginTransaction(CCS881_ADDR);
    Wire.write(regAddr);
    Wire.endTransaction(true);
    for (uint8_t i = 0; i < len; i+=2) {
        Wire.requestFrom(CCS881_ADDR, 2, true);
        while(Wire.available()) {
            data[i] = Wire.read();
        }
    }
}

void ccs881_write_reg(const uint8_t* const data, const uint8_t regAddr, const uint8_t len) {
    Wire.beginTransaction(CCS881_ADDR);
    Wire.write(regAddr);
    for (uint8_t i = 0; i < len; i++) {
        Wire.write(data[i]);
    }
    Wire.endTransaction(true);
}

void ccs881_read_results(uint16_t* co2, uint16_t* voc) {
    digitalWrite(WAKE_PIN, HIGH); //unsure if we have to leave it on all the time
    delayMicroseconds(50);
    uint8_t temp[4] = {0};
    ccs881_read_reg(temp, ALG_RESULT_DATA, 6);
    *co2 = temp[0] << 8 | temp[1];
    *voc = temp[2] << 8 | temp[3]; 
    uint8_t status = temp[4];
    uint8_t error_id = temp[5];
    digitalWrite(WAKE_PIN, LOW);
}
