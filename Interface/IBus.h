#ifndef __IBUS_H
#define __IBUS_H

#include <stdio.h>

class IBus {
    public:
        virtual void txInterrupt();
        virtual void rxInterrupt();

        virtual void send(uint16_t *data, uint16_t size);
    protected:
        
};

#endif