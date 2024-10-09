#include "DS18B20.h"

DS18B20::DS18B20(OneWire *oneWire) {
    this->oneWire = oneWire;
}
uint16_t DS18B20::getTemperature(uint64_t romCode) {
    
}