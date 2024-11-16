#include "DS18B20.h"
#ifdef __DS18B20_H_

bool readT = false;

DS18B20::DS18B20(OneWire *oneWire) {

    this->oneWire = oneWire;

    addTaskMain(taskCallback {
        if (!sensors.empty()) {
            if (readT) {
            
                currentSensor = sensors.front();
                oneWire->sesja(0x55, currentSensor.pDataRom,
                0xBE, 0, 0, currentSensor.rd, 9); //odczyt temperatury
                readT = false;
		    } else {
		        readT = true;
			oneWire->sesja(0xCC, 0, 0x44);//start konwer. temp.
		    }
        }
	}, 1000);
}

void DS18B20::addSensor(uint8_t *romCode, uint8_t nr) {

    sensor sensor;

    sensor.pDataRom = romCode;
    sensor.nr = nr;
    sensors.push(sensor);
}

uint16_t DS18B20::getTemperature(uint8_t id) {

    return ((*(currentSensor.rd+1)) << 8) | (*(currentSensor.rd));
    
   // return (recv[1] << 8) | recv[0];
}
#endif // __DS18B20_H__