#ifndef __EEPROM_H_
#define __EEPROM_H_
#include "../../Core.h"
class EEPROM {
    public:
        EEPROM(I2C_HandleTypeDef *instance, uint8_t address, uint16_t pages, uint16_t pagesize, uint16_t offset=0);
        void read16(uint16_t address, uint16_t *data, uint16_t size);
        void write16(uint16_t address, uint16_t *data, uint16_t size);
    private:
        I2C_HandleTypeDef *_instance;
		uint8_t _address;
        uint16_t _pages;
        uint16_t _pageSize;
        uint16_t _offset;
};
#endif