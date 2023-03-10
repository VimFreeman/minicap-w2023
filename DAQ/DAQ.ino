#include <Wire.h>
#include "SparkFunCCS811.h"
#include "BluetoothSerial.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define CCS811_ADDR 0x5A // CCS811 I2C ADDRESS
#define DHT_PIN 32
#define DHT_TYPE DHT22
#define PM Serial2

CCS811 myCCS(CCS811_ADDR);
DHT_Unified myDHT(DHT_PIN, DHT_TYPE);

BluetoothSerial SerialBT;

uint16_t voc = 0;
uint16_t co2 = 0;

uint8_t tempInt = 0;
uint8_t tempFrac = 0;
uint8_t humInt = 0;
uint8_t humFrac = 0;

int pm1 = 0;
int pm2 = 0;
int pm10 = 0;

void getCCS();
void getDHT();
void getPM();

void setup()
{
  Serial.begin(115200);
  PM.begin(9600);
  SerialBT.begin("PAQ-3000");

  Wire.begin(); // Enable I2C Peripheral

  if (myCCS.begin() == false)
  {
    Serial.print("CCS811 error");
    while (1);
  }

  myDHT.begin();

  delay(1000);
}

void loop()
{
  if (myCCS.dataAvailable())
  {
    getCCS();
    getDHT(); // we put it with myCCS so as not to spam the serial console
    getPM();
  }

  //handle bluetooth requests
  if (SerialBT.available()) {
    uint8_t cmd = SerialBT.read();
    switch(cmd) {
      case 48:
        SerialBT.write((uint8_t*)&co2, 2);
        SerialBT.write((uint8_t*)&voc, 2);
        SerialBT.write(tempInt);
        SerialBT.write(tempFrac);
        SerialBT.write(humInt);
        SerialBT.write(humFrac);
        SerialBT.write((uint8_t*)&pm1, 4);
        SerialBT.write((uint8_t*)&pm2, 4);
        SerialBT.write((uint8_t*)&pm10, 4);
        SerialBT.write(10);
        break;
      default:
        break;
    }
  }

  // check each reading and update the corresponding LED
  
  delay(10); // We don't want to spam the I2C bus
}

void getCCS() {
  myCCS.readAlgorithmResults();

  co2 = myCCS.getCO2();
  voc = myCCS.getTVOC();

  // Serial port sensor debugging
  Serial.print("CO2: ");
  Serial.print(co2);
  Serial.println(" ppm");
  Serial.print("tVOC: ");
  Serial.print(voc);
  Serial.println(" ppb");
  // ----------------------------
}

void getDHT() {
  sensors_event_t tempEvent;
  sensors_event_t humEvent;

  myDHT.temperature().getEvent(&tempEvent);
  myDHT.humidity().getEvent(&humEvent);

  float temp = tempEvent.temperature;
  float hum = humEvent.relative_humidity;

  float temptempInt;
  float temphumInt;

  tempFrac = (uint8_t)(modf(temp, &temptempInt)*100);
  humFrac = (uint8_t)(modf(hum, &temphumInt)*100);

  tempInt = (uint8_t)temptempInt;
  humInt = (uint8_t)temphumInt;

  // Serial port debugging
  Serial.println("Temp: " + String(tempInt) + "." + String(tempFrac) + "°C");
  Serial.println("Humidity: " + String(humInt) + "." + String(humFrac) + "%");
  //----------------------
}

void getPM() {
  // need to figure out what a "DF" is. My assumption is that it's a uart data frame (i.e. 8 bits)
  PM.write(0x110102EC);
  int dummy = PM.read(3);
  int part1 = PM.read(4);
  int part2 = PM.read(4);
  int part3 = PM.read(4);

  // probably not necessary. Need to test with and without
  // could be that we just need to read one more byte
  // can test by making a loop after line 130 that reads until empty and count the number of bytes read.
  while(PM.available()>0) {
    PM.read(); // flush input buffer
  }
  // check if read is little or big endian

  // here we assume DF4-3-2-1
  pm2 = (part1 & 0x0F00)*256 + (part1 & 0xF000);
  pm1 = (part2 & (0x0F00)*256^1 + (part2 & 0xF000);
  pm10 = (part2 & (0x0F00)*256^1 + (part2 & 0xF000);
}