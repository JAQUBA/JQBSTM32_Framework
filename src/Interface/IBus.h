#ifndef __IBUS_H
#define __IBUS_H

#include <functional>

using dataCallback_f = std::function<void(uint8_t *pData, uint16_t Size)>;

#define MEM_ADR_INVALID 0

class IBus {
    public:
    protected:
};

#endif