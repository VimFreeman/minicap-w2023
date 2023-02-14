#ifndef BLUETOOTH_H
#define BLUETOOTH_H

//blocking function for establishing bluetooth connection with app 
void bluetooth_init(); 

//function to send an arbitrary amount of data
void bluetooth_send(const uint8_t* const data, const uint8_t len);

//function to receieve up to maxLen bytes of data from bluetooth connection
uint8_t bluetooth_read(uint8_t* data, const uint8_t maxLen);

#endif