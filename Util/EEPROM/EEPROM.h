#ifndef __EEPROM_H_
#define __EEPROM_H_
#include "../../Core.h"
#include "../../Hardware/I2C/I2C.h"
#include "../../Interface/IExternalMemory.h"
#include "../../Interface/IBus.h"

class EEPROM : public IExternalMemory {
    public:
        EEPROM(IBus *instance, uint8_t address, uint16_t pages, uint16_t pagesize, uint16_t offset=0);
        void read16(uint16_t address, uint16_t *data, uint16_t size);
        void write16(uint16_t address, uint16_t *data, uint16_t size);

};
#endif