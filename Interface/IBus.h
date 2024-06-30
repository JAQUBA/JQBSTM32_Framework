#ifndef __IBUS_H
#define __IBUS_H

#include <stdio.h>

class IBus {
    public:
        virtual void send(uint8_t *data, uint16_t length) {};
        virtual void send(const char *data, uint16_t length) {};
    protected:
        
};

#endif