#include <Wire.h>
#include "SparkFunCCS811.h"
#include "BluetoothSerial.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define CCS811_ADDR 0x5A
#define DHT_PIN 32
#define DHT_TYPE DHT22

CCS811 myCCS(CCS811_ADDR);
DHT_Unified myDHT(DHT_PIN, DHT_TYPE);

BluetoothSerial SerialBT;

uint8_t voc = 0;
uint8_t co2 = 0;
float temp = 0;
float hum = 0;

void setup()
{
  Serial.begin(115200);
  SerialBT.begin("PAQ-3000");

  Wire.begin();

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
    myCCS.readAlgorithmResults();

    co2 = myCCS.getCO2();
    voc = myCCS.getTVOC();

    Serial.print("CO2[");
    //Returns calculated CO2 reading
    Serial.print(co2);
    Serial.print("] tVOC[");
    //Returns calculated TVOC reading
    Serial.print(voc);
    Serial.print("]");
    //Display the time since program start
    Serial.println();
  }

  sensors_event_t event;
  myDHT.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println(F("Error reading temperature!"));
  }
  else {
    temp = event.temperature;
    Serial.print(F("Temperature: "));
    Serial.print(temp);
    Serial.println(F("°C"));
  }
  
  myDHT.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println(F("Error reading humidity!"));
  }
  else {
    hum = event.relative_humidity;
    Serial.print(F("Humidity: "));
    Serial.print(hum);
    Serial.println(F("%"));
  }  

  if (SerialBT.available()) {
    uint8_t cmd = SerialBT.read();
    switch(cmd) {
      case 48:
        SerialBT.write(&co2, 2);
        SerialBT.write(10);
        SerialBT.write(&voc, 2);
        SerialBT.write(10);
        SerialBT.write(10);
        break;
      case 49:
        SerialBT.write((uint8_t)temp);
        SerialBT.write(10);
        SerialBT.write((uint8_t)hum);
        SerialBT.write(10);
        SerialBT.write(10);
        break;
      default:
        break;
    }
  }

  delay(1000); //Don't spam the I2C bus
}