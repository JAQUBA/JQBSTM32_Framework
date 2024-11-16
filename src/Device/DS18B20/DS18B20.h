#include "../../Hardware/OneWire/OneWire.h"
#ifdef ONEWIRE_H

#ifndef __DS18B20_H_
#define __DS18B20_H_

class DS18B20 {
    public:
        DS18B20(OneWire *oneWire);
        void addSensor(uint8_t *romCode, uint8_t nr);
        uint16_t getTemperature(uint8_t id);
    private:
        OneWire *oneWire;

        

        struct sensor {
           // EoperationType  operationType;
            uint8_t         *pDataRom;
            uint8_t         nr;
            uint8_t         rd[9]={0};
            //dataCallback_f  callback_f = nullptr;
        } currentSensor;
        
        std::queue<sensor> sensors;
};

#endif // __DS18B20_H__
#endif // __ONEWIRE_H__