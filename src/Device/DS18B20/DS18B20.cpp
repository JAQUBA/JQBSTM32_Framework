#include "Hardware/OneWire/OneWire.h"

class DS18B20 {
public:
    DS18B20(OneWire &oneWire);
    bool begin();
    float getTemperature();

private:
    OneWire &oneWire;
    uint8_t address[8];
    bool searchAddress();
    void writeScratchpad(uint8_t th, uint8_t tl, uint8_t config);
    void startConversion();
    bool readScratchpad(uint8_t *data, uint8_t length);
};

DS18B20::DS18B20(OneWire &oneWire) : oneWire(oneWire) {}

bool DS18B20::begin() {
    if (!oneWire.search(address)) {
        return false;
    }
    if (OneWire::crc8(address, 7) != address[7]) {
        return false;
    }
    return true;
}

float DS18B20::getTemperature() {
    startConversion();
    delay(750); // Wait for conversion to complete

    uint8_t data[9];
    if (!readScratchpad(data, 9)) {
        return NAN;
    }

    int16_t rawTemperature = (data[1] << 8) | data[0];
    return rawTemperature / 16.0;
}

bool DS18B20::searchAddress() {
    oneWire.reset_search();
    return oneWire.search(address);
}

void DS18B20::writeScratchpad(uint8_t th, uint8_t tl, uint8_t config) {
    oneWire.reset();
    oneWire.select(address);
    oneWire.write(0x4E); // Write Scratchpad command
    oneWire.write(th);
    oneWire.write(tl);
    oneWire.write(config);
}

void DS18B20::startConversion() {
    oneWire.reset();
    oneWire.select(address);
    oneWire.write(0x44); // Start conversion command
}

bool DS18B20::readScratchpad(uint8_t *data, uint8_t length) {
    oneWire.reset();
    oneWire.select(address);
    oneWire.write(0xBE); // Read Scratchpad command

    for (uint8_t i = 0; i < length; i++) {
        data[i] = oneWire.read();
    }

    return (OneWire::crc8(data, 8) == data[8]);
}