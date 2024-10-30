#include "DS18B20.h"
#ifdef __DS18B20_H_

DS18B20::DS18B20(OneWire *oneWire) {
    this->oneWire = oneWire;
}
uint16_t DS18B20::getTemperature(uint64_t romCode) {
    uint8_t data[11];
    uint8_t recv[2] = {0, 0};

    oneWire->reset();
    data[0] = 0x55;
    for (uint8_t i = 0; i < 8; i++) {data[i + 1] = (romCode >> (i * 8)) & 0xff;}
    data[10] = 0x44;
    oneWire->transmit(data, 10);
    data[10] = 0xbe;
    oneWire->transmitThenReceive(
        data, 10,
        recv, 2
    );

    
    return (recv[1] << 8) | recv[0];
}
#endif // __DS18B20_H__