#ifndef __NV_RAM_H_
#define __NV_RAM_H_
#include "../../Core.h"
class NV_RAM {
    public:
        NV_RAM(I2C_HandleTypeDef *instance, uint8_t address, uint16_t pages, uint16_t pagesize, uint16_t offset=0);
        void read16(uint16_t address, uint16_t *data, uint16_t size);
        void write16(uint16_t address, uint16_t *data, uint16_t size);
        
    private:
       
};
#endif