#ifndef __IEXTERNALMEMORY_H
#define __IEXTERNALMEMORY_H

#include "../Interface/IBus.h"

class IExternalMemory {
    public:
        virtual void readFromMemory(uint16_t address, uint16_t *data, uint16_t size, bool blocking = false);
        virtual void writeToMemory(uint16_t address, uint16_t *data, uint16_t size, bool blocking = false);
    protected:
        IBus *_pInstance;

        
        // struct _memOperation {
        //     enum {READ, WRITE} operation;
        // };

		// uint8_t _address;
        // uint16_t _pages;
        // uint16_t _pageSize;
};

#endif