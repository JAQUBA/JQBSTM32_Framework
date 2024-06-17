#ifndef __IEXTERNALMEMORY_H
#define __IEXTERNALMEMORY_H

#include "../Interface/IBus.h"

class IExternalMemory {
    public:
        virtual void read16(uint16_t address, uint16_t *data, uint16_t size);
        virtual void write16(uint16_t address, uint16_t *data, uint16_t size);
    protected:
        IBus *_instance;
		uint8_t _address;
        uint16_t _pages;
        uint16_t _pageSize;
        uint16_t _offset;
};

#endif