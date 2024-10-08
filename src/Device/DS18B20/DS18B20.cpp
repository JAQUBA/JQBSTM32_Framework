#include "DS18B20.h"

DS18B20::DS18B20(OneWire *oneWire) {
    this->oneWire = oneWire;
}
void DS18B20::readTemperature() {
    uint8_t data[9] = {0}; 

    data[0] = 0xCC; // Skip ROM
    data[1] = 0x44; // Convert T
    

    this->oneWire->transmit(data, 9);
    this->oneWire->receive(data, 9);

}