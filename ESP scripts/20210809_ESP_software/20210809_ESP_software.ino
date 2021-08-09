// This Sketch programs the ESP32 for Climate observation

// Low energy Bluetooth Packages
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// General Packages
#include <Wire.h>
#include <SPI.h>

// Sensor Packages
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <BH1750.h>
#include "SparkFunCCS811.h"
// #include <Multichannel_Gas_GMXXX.h>

Adafruit_BME280 bme;

// Set up BLE Characteristics
BLECharacteristic *temp_characteristic;
BLECharacteristic *pres_characteristic;
BLECharacteristic *aHum_characteristic;
BLECharacteristic *ligh_characteristic;
BLECharacteristic *CO2_characteristic;
BLECharacteristic *tVOC_characteristic;

BLECharacteristic *NO2_characteristic;
BLECharacteristic *C2H5OH_characteristic;
BLECharacteristic *VOC_characteristic;
BLECharacteristic *CO_characteristic;


BLECharacteristic *sHum_characteristic;
BLECharacteristic *aFan_characteristic;
BLECharacteristic *pump_characteristic;

// Defining important pins on the ESP Board
int sHumPin   = 33;
int pumpPin   = 4;
int fanPin    = 16;

// Defining Values for measurements
float tempVal, presVal, aHumVal, lighVal, CO2Val, tVOCVal;
float NO2Val, C2H5OHVal, VOCVal, COVal ;
bool  fanBool , pumpBool, deviceConnected;
int   sHumVal ;

// Setting I2C addresses
BH1750 lightMeter(0x23);
CCS811 myCCS811(0x5A);
//#ifdef SOFTWAREWIRE
//    #include <SoftwareWire.h>
//    SoftwareWire myWireM(3, 2);
//    GAS_GMXXX<SoftwareWire> gas;
//#else
//    GAS_GMXXX<TwoWire> gas;
//#endif

// Setting BLE addresses
#define SENSORS_UUID              "b035d8ca-1d3f-4b01-800c-91a1519992ad" 
#define CONTROL_UUID              "e5b43dfc-88f2-4c01-b4f9-45046cded961" 
#define CHARACTERISTIC_UUID_temp  "39a02f19-d507-45cb-bd67-a328f6c63caa"
#define CHARACTERISTIC_UUID_pres  "39a02f20-d507-45cb-bd67-a328f6c63caa"
#define CHARACTERISTIC_UUID_aHum  "39a02f21-d507-45cb-bd67-a328f6c63caa"
#define CHARACTERISTIC_UUID_ligh  "39a02f22-d507-45cb-bd67-a328f6c63caa"
#define CHARACTERISTIC_UUID_CO2   "39a02f23-d507-45cb-bd67-a328f6c63caa"
#define CHARACTERISTIC_UUID_tVOC  "39a02f24-d507-45cb-bd67-a328f6c63caa"

#define CHARACTERISTIC_UUID_NO2    "39a03f19-d507-45cb-bd67-a328f6c63caa"
#define CHARACTERISTIC_UUID_C2H5OH "39a03f20-d507-45cb-bd67-a328f6c63caa"
#define CHARACTERISTIC_UUID_VOC    "39a03f21-d507-45cb-bd67-a328f6c63caa"
#define CHARACTERISTIC_UUID_CO     "39a03f22-d507-45cb-bd67-a328f6c63caa"


#define CHARACTERISTIC_UUID_sHum  "468253b4-75f0-4804-be54-077a5b1aefa6"
#define CHARACTERISTIC_UUID_fan   "659552a3-05c7-41ae-87c3-91eb5b8c3c12"
#define CHARACTERISTIC_UUID_pump  "e21267bb-5259-400a-b55a-3f33f4bdd37d"

// Setting BLE callbacks for connections and turning on and off pumps
// 1. Do something when a device connects
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };
    
    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
  };
  
class MyCallbacksFan: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *aFan_characteristic) {
      std::string rxVal = aFan_characteristic->getValue();
      if (rxVal.length() > 0) {
        if (rxVal[0] == 1)    digitalWrite (fanPin, LOW);
        if (rxVal[0] == 0)    digitalWrite (fanPin, HIGH);
        }
      }
  };

class MyCallbacksPump: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pump_characteristic) {
      std::string rxVal = pump_characteristic->getValue();
      if (rxVal.length() > 0) {
        if (rxVal[0] == 1)    digitalWrite (pumpPin, HIGH);
        if (rxVal[0] == 0)    digitalWrite (pumpPin, LOW);
      }
    }
  };

  
// Startup
void setup() {
  // Opening Serial Port
  Serial.begin(115200);
  while (!Serial) {
      delay(10);
  }
  
  //Setting the Fan and pump pins.
  pinMode (fanPin,  OUTPUT);
  pinMode (pumpPin, OUTPUT);
  digitalWrite (pumpPin, LOW);

  //  Create the bluetooth low energy device
  BLEDevice::init("Louie"); // Give it a name

  // Create the BLE Server
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Services
  BLEService *pService = pServer->createService(SENSORS_UUID);
  BLEService *ControlService = pServer->createService(CONTROL_UUID);

  // Create all BLE characteristics
  temp_characteristic = pService->createCharacteristic(
                          CHARACTERISTIC_UUID_temp,
                          BLECharacteristic::PROPERTY_READ
                        );
  temp_characteristic -> addDescriptor(new BLE2902());

  pres_characteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_pres,
                        BLECharacteristic::PROPERTY_READ);
  pres_characteristic -> addDescriptor(new BLE2902());

  aHum_characteristic = pService->createCharacteristic( CHARACTERISTIC_UUID_aHum,
                        BLECharacteristic::PROPERTY_READ
                                                      );
  aHum_characteristic -> addDescriptor(new BLE2902());

  ligh_characteristic = pService->createCharacteristic(
                          CHARACTERISTIC_UUID_ligh,
                          BLECharacteristic::PROPERTY_READ
                        );
  ligh_characteristic -> addDescriptor(new BLE2902());

  CO2_characteristic = pService->createCharacteristic(
                         CHARACTERISTIC_UUID_CO2,
                         BLECharacteristic::PROPERTY_READ
                       );
  CO2_characteristic -> addDescriptor(new BLE2902());

  tVOC_characteristic = ControlService->createCharacteristic(
                          CHARACTERISTIC_UUID_tVOC,
                          BLECharacteristic::PROPERTY_READ
                        );
  tVOC_characteristic -> addDescriptor(new BLE2902());

  sHum_characteristic = ControlService->createCharacteristic(
                          CHARACTERISTIC_UUID_sHum,
                          BLECharacteristic::PROPERTY_READ
                        );
  sHum_characteristic -> addDescriptor(new BLE2902());

  
  NO2_characteristic = ControlService->createCharacteristic(
                          CHARACTERISTIC_UUID_NO2,
                          BLECharacteristic::PROPERTY_READ
                        );
  NO2_characteristic -> addDescriptor(new BLE2902());
  C2H5OH_characteristic = ControlService->createCharacteristic(
                          CHARACTERISTIC_UUID_C2H5OH,
                          BLECharacteristic::PROPERTY_READ
                        );
  C2H5OH_characteristic -> addDescriptor(new BLE2902());
  VOC_characteristic = ControlService->createCharacteristic(
                          CHARACTERISTIC_UUID_VOC,
                          BLECharacteristic::PROPERTY_READ
                        );
  VOC_characteristic -> addDescriptor(new BLE2902());
  CO_characteristic = ControlService->createCharacteristic(
                          CHARACTERISTIC_UUID_CO,
                          BLECharacteristic::PROPERTY_READ
                        );
  CO_characteristic -> addDescriptor(new BLE2902());

    aFan_characteristic = ControlService->createCharacteristic(
                        CHARACTERISTIC_UUID_fan,
                        BLECharacteristic::PROPERTY_READ|
                        BLECharacteristic::PROPERTY_WRITE
                        );
    aFan_characteristic -> addDescriptor(new BLE2902());
    aFan_characteristic -> setCallbacks(new MyCallbacksFan);
  
    pump_characteristic = ControlService->createCharacteristic(
                        CHARACTERISTIC_UUID_pump,
                        BLECharacteristic::PROPERTY_READ|
                        BLECharacteristic::PROPERTY_WRITE
                        );
  
    pump_characteristic -> addDescriptor(new BLE2902());
    pump_characteristic ->setCallbacks(new MyCallbacksFan);


  // Start the service
  pService->start();
  ControlService->start();

  BLEAdvertising *pAdvertising = pServer->getAdvertising();

  pAdvertising->addServiceUUID(pService->getUUID());
  pAdvertising->addServiceUUID(ControlService->getUUID());

  // Start advertising
  pServer->getAdvertising()->start();

 // Starting I2C communications
  Wire.begin();
  unsigned status;
  status = bme.begin(0x76);
//  gas.begin(Wire, 0x08);
//  if (gas.begin(Wire, 0x08)) {
//     Serial.println("Grove multichannel Sensor started")
//  }
//  else{
//    Serial.println("Grove Multichannel sensor Failed")
//  }
  if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println(F("BH1750 Advanced begin"));
  }
  else {
    Serial.println(F("Error initialising BH1750"));
  }

  CCS811Core::status returnCode = myCCS811.begin();
  if (returnCode != CCS811Core::SENSOR_SUCCESS)
  {
    Serial.println("CSS811.begin() returned with an error.");
  }
}


void loop() {

  tempVal = bme.readTemperature(); // This could be an actual sensor reading!
  presVal = bme.readPressure() / 100.00 ;
  aHumVal = bme.readHumidity();
  sHumVal = analogRead(sHumPin);
  NO2Val  = 0; // gas.measure_NO2() ; 
  C2H5OHVal = 0; // gas.measure_C2H5OH();
  VOCVal  = 0; // gas.measure_VOC();
  COVal   = 0; // gas.measure_CO();
  float lighVal = lightMeter.readLightLevel();
  if (myCCS811.dataAvailable()) {
    myCCS811.readAlgorithmResults();
    CO2Val = myCCS811.getCO2();
    tVOCVal = myCCS811.getTVOC();
  }
  else {
    CO2Val  = 0 ;
    tVOCVal = 0;
  }
  char tempString[8];                               // make sure this is big enuff
  dtostrf(tempVal, 1, 2, tempString);             // float_val, min_width, digits_after_decimal, char_buffer
  temp_characteristic->setValue(tempString);

  char aHumString[8];
  dtostrf(aHumVal, 1, 2, aHumString);
  aHum_characteristic->setValue(aHumString);

  char presString[8];
  dtostrf(presVal, 1, 2, presString);
  pres_characteristic->setValue(presString);

  char lighString[8];
  dtostrf(lighVal, 1, 2, lighString);
  ligh_characteristic->setValue(lighString);

  char CO2String[8];
  dtostrf(CO2Val, 1, 2, CO2String);
  CO2_characteristic->setValue(CO2String);

  char tVOCString[8];
  dtostrf(tVOCVal, 1, 2, tVOCString);
  tVOC_characteristic->setValue(tVOCString);

  char sHumString[8];
  dtostrf(sHumVal, 1, 2, sHumString);
  sHum_characteristic->setValue(sHumString);

  char NO2String[8];
  dtostrf(NO2Val, 1, 2, NO2String);
  NO2_characteristic->setValue(NO2String);
  char C2H5OHString[8];
  dtostrf(C2H5OHVal, 1, 2, C2H5OHString);
  C2H5OH_characteristic->setValue(C2H5OHString);
  char VOCString[8];
  dtostrf(VOCVal, 1, 2, VOCString);
  VOC_characteristic->setValue(VOCString);
  char COString[8];
  dtostrf(COVal, 1, 2, COString);
  CO_characteristic->setValue(COString);

  Serial.print("Temp: ");
  Serial.print(tempVal);
  Serial.print("\t");
  Serial.print("aHum: ");
  Serial.print(aHumVal);
  Serial.print("\t");
  Serial.print("Pres: ");
  Serial.print(presVal);
  Serial.print("\t");
  Serial.print("sHum: ");
  Serial.print(sHumVal);
  Serial.print("\t");
  Serial.print("Light: ");
  Serial.print(lighVal);
  Serial.print("\t");
  Serial.print("CO2: ");
  Serial.print(CO2Val);
  Serial.print("\t");
  Serial.print("tVOC: ");
  Serial.print(tVOCVal);
  Serial.println("\t");

  Serial.print("NO2: ");
  Serial.print(NO2Val);
  Serial.println("\t");

  Serial.print("C2H5OH: ");
  Serial.print(C2H5OHVal);

  Serial.print("VOC: ");
  Serial.print(VOCVal);
  Serial.println("\t");

  Serial.print("CO: ");
  Serial.print(COVal);
  Serial.println("\t");


//  if (sHumVal > 3500) digitalWrite (pumpPin, HIGH);
//  if (sHumVal < 3500) digitalWrite (pumpPin, LOW);
//  digitalWrite (fanPin, LOW);
  delay(1000);
  if (sHumVal > 3500) digitalWrite (pumpPin, LOW);
  if (sHumVal < 2500) digitalWrite (pumpPin, HIGH);
//  digitalWrite (fanPin, HIGH);
}
