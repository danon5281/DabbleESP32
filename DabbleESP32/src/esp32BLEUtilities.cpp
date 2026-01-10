#if defined(ESP32)
#include "esp32BLEUtilities.h"

bool BLE_status = 0;
bool isDeviceConnected = false;
bool prevDeviceConnected = false;
uint8_t tx_Value = 0;
uint8_t rxdatalength = 0;
uint8_t bytesremaining = 0;
uint8_t* rxdataBuffer = NULL;
bool newDataReceived = 0;

BLEServer *bleServer = NULL;
BLECharacteristic *bleTxCharacteristic;

// BLE Server Callbacks
class BleServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* bleServer) override {
        isDeviceConnected = true;
    }

    void onDisconnect(BLEServer* bleServer) override {
        isDeviceConnected = false;
    }
};

// BLE Characteristic Callbacks
class BleCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *bleCharacteristic) override {

        // Получаем данные как std::string
        std::string rx_Value = bleCharacteristic->getValue();

        if (newDataReceived == 1) {
            delete[] rxdataBuffer;
        }

        newDataReceived = 1;
        if (rx_Value.length() > 0) {
            rxdataBuffer = new uint8_t[rx_Value.length()];
            for (int i = 0; i < rx_Value.length(); i++) {
                rxdataBuffer[i] = (uint8_t)rx_Value[i];
                #ifdef DEBUG
                Serial.print(rxdataBuffer[i]);
                Serial.print(" ");
                #endif
            }
            #ifdef DEBUG
            Serial.println();
            #endif
            rxdatalength = rx_Value.length();
            bytesremaining = rx_Value.length();
        }
    }
};

void Esp32ble::begin(String a) {

    // Преобразование String → const char*
    BLEDevice::init(a.c_str());    

    bleServer = BLEDevice::createServer();
    bleServer->setCallbacks(new BleServerCallbacks());

    BLEService *bleService = bleServer->createService(UUID_Service);

    bleTxCharacteristic = bleService->createCharacteristic(
        UUID_Transmit,
        BLECharacteristic::PROPERTY_NOTIFY
    );

    bleTxCharacteristic->addDescriptor(new BLE2902());

    BLECharacteristic *bleRxCharacteristic = bleService->createCharacteristic(
        UUID_Receive,
        BLECharacteristic::PROPERTY_WRITE
    );
    bleRxCharacteristic->setCallbacks(new BleCallbacks());

    bleService->start();
    bleServer->getAdvertising()->start();

    #ifdef DEBUG
    Serial.println("Waiting for a client connection...");
    #endif
}

void Esp32ble::write(uint8_t a) {
    if (isDeviceConnected) {
        bleTxCharacteristic->setValue(&a, 1);
        bleTxCharacteristic->notify();
        delay(10);
    }

    if (!isDeviceConnected && prevDeviceConnected) {
        delay(500);
        bleServer->startAdvertising();
        prevDeviceConnected = isDeviceConnected;
    }
}

void Esp32ble::write(String x) {
    if (isDeviceConnected) {
        // String → std::string
        bleTxCharacteristic->setValue(std::string(x.c_str()));
        bleTxCharacteristic->notify();
        delay(10);
    }

    if (!isDeviceConnected && prevDeviceConnected) {
        delay(500);
        bleServer->startAdvertising();
        prevDeviceConnected = isDeviceConnected;
    }
}

void Esp32ble::write(int a) {
    if (isDeviceConnected) {
        bleTxCharacteristic->setValue(a);
        bleTxCharacteristic->notify();
        delay(10);
    }

    if (!isDeviceConnected && prevDeviceConnected) {
        delay(500);
        bleServer->startAdvertising();
        prevDeviceConnected = isDeviceConnected;
    }
}

void Esp32ble::write(float a) {
    if (isDeviceConnected) {
        bleTxCharacteristic->setValue(a);
        bleTxCharacteristic->notify();
        delay(10);
    }

    if (!isDeviceConnected && prevDeviceConnected) {
        delay(500);
        bleServer->startAdvertising();
        prevDeviceConnected = isDeviceConnected;
    }
}

uint8_t Esp32ble::available() {
    return bytesremaining;
}

uint8_t Esp32ble::read() {
    if (bytesremaining > 0) {
        uint8_t a = rxdataBuffer[rxdatalength - bytesremaining];
        bytesremaining--;
        return a;
    } else {
        return 0;
    }
}

void Esp32ble::stop() {
    btStop();
}

Esp32ble esp32ble;
#endif
