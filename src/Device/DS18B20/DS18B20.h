#include "../../Hardware/OneWire/OneWire.h"
#ifdef __ONEWIRE_H_

#ifndef __DS18B20_H_
#define __DS18B20_H_

class DS18B20 {
    public:
        DS18B20(OneWire *oneWire);
        uint16_t getTemperature(uint64_t romCode);
    private:
        OneWire *oneWire;
};

#endif // __DS18B20_H__
#endif // __ONEWIRE_H__