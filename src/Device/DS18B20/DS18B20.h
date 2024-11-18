#include "../../Hardware/OneWire/OneWire.h"
#ifdef ONEWIRE_H

#ifndef __DS18B20_H_
#define __DS18B20_H_

#ifndef DS_MAX_SENSORS
#define DS_MAX_SENSORS 10
#endif

class DS18B20 {
    public:
        DS18B20(OneWire *oneWire);
        void addSensor(uint64_t romCode, uint8_t nr);
        uint16_t getTemperature(uint8_t id);
    private:
        OneWire *oneWire;
        uint8_t b_rom[8] ={0};
        uint8_t id=0;
        uint8_t b_rd[DS_MAX_SENSORS * 9]={0};
};

#endif // __DS18B20_H__
#endif // __ONEWIRE_H__