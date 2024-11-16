#include "DS18B20.h"
#ifdef __DS18B20_H_

bool readT = false;

DS18B20::DS18B20(OneWire *oneWire) {
    
    this->oneWire = oneWire;

    addTaskMain(taskCallback {

        if (readT) {
            if (!sensors.empty()) {
                currentSensor = sensors.front();
                oneWire->sesja(0x55, currentSensor.pDataRom,
                0xBE, 0, 0, currentSensor.rd, 9); //odczyt temperatury
            }
            readT = false;
		} else {
		    readT = true;
			oneWire->sesja(0xCC, 0, 0x44);//start konwer. temp.
		}
	}, 1000);
}

void DS18B20::addSensor(uint8_t *romCode, uint8_t nr) {

    sensor sensor;

    sensor.pDataRom = romCode;
    sensor.nr = nr;
    sensors.push(sensor);
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