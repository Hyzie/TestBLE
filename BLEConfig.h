#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <WiFi.h>
#include "StringEEPROM.h"

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;
bool shouldDeinitBLE = false;
unsigned long deinitTime = 0;

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      Serial.println("Device connected");
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
      Serial.println("Device disconnected");
    }
};



// Function to connect to Wi-Fi
void WriteIntoEEPROM(String ssid, String password) {
    EEPROM.begin(EEPROM_SIZE); 
    
    Serial.println("Writing SSID and Password into EEPROM...");

    WriteStringToEEPROM(0, ssid);
    WriteStringToEEPROM(20, password);

    Serial.println("All done.");
}

String Essid()
{
  EEPROM.begin(EEPROM_SIZE); 
  return ReadStringFromEEPROM(0);
}

String Epass()
{
  EEPROM.begin(EEPROM_SIZE); 
  return ReadStringFromEEPROM(20);
}

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      String rxValue = pCharacteristic->getValue();

      char* device_id = "IOT_RF";

      if (rxValue.length() > 0) {
        String receivedData = String(rxValue.c_str());
        int separatorIndex = rxValue.indexOf(',');
        
        if (separatorIndex != -1) {
          String ssid = receivedData.substring(0, separatorIndex);
          String password = receivedData.substring(separatorIndex + 1);
          
          Serial.print("SSID: ");
          Serial.println(ssid);
          Serial.print("Password: ");
          Serial.println(password);
          WriteIntoEEPROM(ssid, password);

          WiFi.begin(ssid, password);
          Serial.print("Connecting to WiFi");
          int attempts = 0;
          while (WiFi.status() != WL_CONNECTED && attempts < 5) {  
              delay(500);
              Serial.print(".");
              attempts++;
          }

          if (WiFi.status() == WL_CONNECTED) {
            Serial.print("Sent data to phone: ");
            pCharacteristic->setValue(device_id);
            pCharacteristic->notify();
            Serial.print("Connected to WiFi");

            WiFi.disconnect();

            delay(1000); 
            shouldDeinitBLE = true; 
            deinitTime = millis();
          } else {
              pCharacteristic->setValue("0");
              pCharacteristic->notify();
              Serial.println("Failed to connect to WiFi");
          }
        }
      }
    }
};

void BLEsetup() {
  Serial.println("Starting BLE work!");

  BLEDevice::init("ORC IOTRF");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);

  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );

  pCharacteristic->setCallbacks(new MyCallbacks());
  pCharacteristic->addDescriptor(new BLE2902());

  pService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0); 
  BLEDevice::startAdvertising();
  Serial.println("Characteristic defined! Now you can read it in your phone!");
}

void BLErepeat() {
  // Disconnecting
  if (!deviceConnected && oldDeviceConnected) {
    delay(500); 
    pServer->startAdvertising(); 
    Serial.println("Start advertising");
    oldDeviceConnected = deviceConnected;
  }
  // Connecting
  if (deviceConnected && !oldDeviceConnected) {
    oldDeviceConnected = deviceConnected;
  }
  if (shouldDeinitBLE && millis() - deinitTime > 1000) {
    BLEDevice::deinit(true);
    Serial.println("Bluetooth deinitialized");
    shouldDeinitBLE = false;
    esp_restart();
  }
}