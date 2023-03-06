#include <Wire.h>
#include "SparkFunCCS811.h"
#include "BluetoothSerial.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define CCS811_ADDR 0x5A // CCS811 I2C ADDRESS
#define DHT_PIN 32
#define DHT_TYPE DHT22

CCS811 myCCS(CCS811_ADDR);
DHT_Unified myDHT(DHT_PIN, DHT_TYPE);

BluetoothSerial SerialBT;

uint16_t voc = 0;
uint16_t co2 = 0;
float temp = 0;
float hum = 0;

void getCCS();
void getDHT();

void setup()
{
  Serial.begin(115200);
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
    getDHT(); // we put it with myCCS so as not to spam the console
  }

  // handle bluetooth requests
  if (SerialBT.available()) {
    uint8_t cmd = SerialBT.read();
    switch(cmd) {
      case 48:
        SerialBT.write((uint8_t*)&co2, 2);
        SerialBT.write((uint8_t*)&voc, 2);
        SerialBT.write((uint8_t*)&temp, 4);
        SerialBT.write((uint8_t*)&hum, 4);
        break;
      default:
        break;
    }
  }

  delay(10); //Don't spam the I2C bus
}

void getCCS() {
  myCCS.readAlgorithmResults();

  co2 = myCCS.getCO2();
  voc = myCCS.getTVOC();

  // Serial port sensor debugging
  Serial.print("CO2: ");
  Serial.println(co2);
  Serial.print("tVOC: ");
  Serial.println(voc);
  // ----------------------------
}

void getDHT() {
  sensors_event_t tempEvent;
  sensors_event_t humEvent;

  myDHT.temperature().getEvent(&tempEvent);
  myDHT.humidity().getEvent(&humEvent);

  hum = humEvent.relative_humidity;
  temp = tempEvent.temperature;

  // Serial port sensor debugging
  Serial.print(F("Temperature: "));
  Serial.print(temp);
  Serial.println(F("°C"));

  Serial.print(F("Humidity: "));
  Serial.print(hum);
  Serial.println(F("%"));
  //------------------------------
}
