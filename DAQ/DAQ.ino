#include <Wire.h>
#include "SparkFunCCS811.h"
#include "BLEDevice.h"
#include "BLEServer.h"
#include "BLEUtils.h"
#include "BLE2902.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define CCS811_ADDR 0x5A // CCS811 I2C ADDRESS
#define DHT_PIN 2
#define DHT_TYPE DHT22
#define PM Serial2

#define SERVICE_UUID "87843d21-fd03-4307-8a95-bd3d76b49644"

CCS811 myCCS(CCS811_ADDR);
DHT_Unified myDHT(DHT_PIN, DHT_TYPE);

unsigned long perms = BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY;
BLECharacteristic ccsChar("5f551915-bdc8-4bac-b9df-12256620e1cf", perms);
BLEDescriptor ccsDesc(BLEUUID((uint16_t)0x2902));
BLECharacteristic dhtChar("f78ebbff-c8b7-4107-93de-889a6a06d408", perms);
BLEDescriptor dhtDesc(BLEUUID((uint16_t)0x2902));
BLECharacteristic pmChar("ca73b3ba-39f6-4ab3-91ae-186dc9577d99", perms);
BLEDescriptor pmDesc(BLEUUID((uint16_t)0x2902));

bool deviceConnected = false;
bool connecting = false;

uint16_t voc = 0;
uint16_t co2 = 0;

uint8_t tempInt = 0;
uint8_t tempFrac = 0;
uint8_t humInt = 0;
uint8_t humFrac = 0;

int pm2 = 0;

void getCCS();
void getDHT();
void getPM();
void initBLE();

class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;  
    connecting = false;
    Serial.println("Connected");
    digitalWrite(LED_BUILTIN, HIGH);  
  };
  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
    Serial.println("Disconnected");

    pServer->getAdvertising()->start();
    Serial.println("Advertising");

    digitalWrite(LED_BUILTIN, LOW);     
  }
};

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  delay(1000);
  PM.begin(9600); // enable Serial2 for PM sensor
  Wire.begin(); // Enable I2C Peripheral for CCS

  if (myCCS.begin() == false)
  {
    Serial.print("CCS811 error");
    while (1);
  }

  myDHT.begin(); // enable DHT singel wire protocol
  initBLE();
}

long timer = millis();
void loop()
{
  // get sensor readings and write to BLE
  if (deviceConnected) {
    if (myCCS.dataAvailable()) { getCCS(); }
    getDHT();
    getPM();
    Serial.println();  
    
    delay(2000);
  } 
}

void initBLE() {
  // Initialize server and service
  BLEDevice::init("Portable Air Quality");
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Add characteristics to service
  pService->addCharacteristic(&ccsChar);
  pService->addCharacteristic(&dhtChar);
  pService->addCharacteristic(&pmChar);
  
  ccsDesc.setValue("CCS811 CO2 and VOC");
  dhtDesc.setValue("DHT Temp and Humidity");
  pmDesc.setValue("PM2.5");

  ccsChar.addDescriptor(&ccsDesc);
  dhtChar.addDescriptor(&dhtDesc);
  pmChar.addDescriptor(&pmDesc);

  // write some initial values to the chars
  ccsChar.setValue("0");
  dhtChar.setValue("0");
  pmChar.setValue("0");

  pService->start();
  
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pServer->getAdvertising()->start();
  
  Serial.println("Advertising");
}

void getCCS() {
  // read and load values
  myCCS.readAlgorithmResults();

  co2 = myCCS.getCO2();
  voc = myCCS.getTVOC();

  int data = (co2 << 16 | voc);
  ccsChar.setValue(data);
  ccsChar.notify();

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

  int data = (tempInt << 24 | tempFrac << 16 | humInt << 8 | humFrac);
  dhtChar.setValue(data);
  dhtChar.notify();

  // Serial port debugging
  Serial.println("Temp: " + String(tempInt) + "." + String(tempFrac) + "°C");
  Serial.println("Humidity: " + String(humInt) + "." + String(humFrac) + "%");
  //----------------------
}

void getPM() {
  PM.write(0x11);
  PM.write(0x01);
  PM.write(0x02);
  PM.write(0xEC);
  
  uint8_t* dummy = new uint8_t[3];
  uint8_t* part1 = new uint8_t[4];
  uint8_t* part2 = new uint8_t[4];
  uint8_t* part3 = new uint8_t[4];
 
  if (PM.available() > 0) {
    PM.readBytes(dummy, 3);
    PM.readBytes(part1, 4);
    PM.readBytes(part2, 4);
    PM.readBytes(part3, 4); 
    PM.read(); // read last bit [CS]
  }

  int pm2 = (uint32_t)part1[2]*256 + ((uint32_t)part1[3]);

  pmChar.setValue(pm2);
  pmChar.notify();

  // Serial port debugging
  Serial.println("PM2.5: " + String(pm2) + "ug/m^3");
}