#include "DS18B20.h"
#ifdef __DS18B20_H_

uint64_t sensors[DS_MAX_SENSORS] = {0};

bool readT = false;

void unpack_rom(uint64_t number, uint8_t *result) {
    result[7] = number & 0x00000000000000FF ; number = number >> 8 ;
    result[6] = number & 0x00000000000000FF ; number = number >> 8 ;
    result[5] = number & 0x00000000000000FF ; number = number >> 8 ;
    result[4] = number & 0x00000000000000FF ; number = number >> 8 ;
    result[3] = number & 0x00000000000000FF ; number = number >> 8 ;
    result[2] = number & 0x00000000000000FF ; number = number >> 8 ;
    result[1] = number & 0x00000000000000FF ; number = number >> 8 ;
    result[0] = number & 0x00000000000000FF ;
}

DS18B20::DS18B20(OneWire *a_oneWire) {
    this->oneWire = a_oneWire;
    addTaskMain(taskCallback {
        if (readT) {
            for (id=0; id<DS_MAX_SENSORS; id++) {
                unpack_rom(sensors[id], b_rom);
                if (b_rom[id]>0) {
                    oneWire->sesja(0x55, b_rom,
                    0xBE, 0, 0, b_rd+(id*9), 9); //odczyt temperatury
                }
            }
            readT = false;
        } else {
            readT = true;
            oneWire->sesja(0xCC, 0, 0x44);//start konwer. temp.
        }
	}, 1000);
}

void DS18B20::addSensor(uint64_t romCode, uint8_t id) {
    sensors[id]=romCode;
}

uint16_t DS18B20::getTemperature(uint8_t id) {
    return    (*(b_rd+(id*9)+1))<<8 | *(b_rd+(id*9));
}

#endif // __DS18B20_H__