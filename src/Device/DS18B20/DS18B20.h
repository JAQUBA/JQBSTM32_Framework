#ifndef __DS18B20_H__
#define __DS18B20_H__

#include "Core.h"
#include "Hardware/OneWire/OneWire.h"

class DS18B20 {
    public:
        DS18B20(OneWire *oneWire);
        void readTemperature();
    private:
        OneWire *oneWire;
};

#endif // __DS18B20_H__