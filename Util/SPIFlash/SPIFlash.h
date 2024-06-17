#ifndef __SPIFLASH_H
#define __SPIFLASH_H

#include "Core.h"
#include "Interface/IExternalMemory.h"

class SPIFlash: public IExternalMemory {
    public:
        SPIFlash(IBus *_interface);
        void read16(uint16_t address, uint16_t *data, uint16_t size);
        void write16(uint16_t address, uint16_t *data, uint16_t size);
};

#endif