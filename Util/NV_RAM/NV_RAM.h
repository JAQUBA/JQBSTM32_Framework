#ifndef __NV_RAM_H_
#define __NV_RAM_H_
#include "../../Core.h"
#include "../../Hardware/I2C/I2C.h"
#include "../../Util/EEPROM/EEPROM.h"

class NV_RAM : public EEPROM {
    public:
        NV_RAM(I2C *pInstance, uint16_t DevAddress);
};
#endif