#ifndef __SPI_H
#define __SPI_H
#include "Core.h"
#include "Interface/IBus.h"

class SPI : public IBus {
    public:
        SPI(SPI_HandleTypeDef *_instance);

};

#endif