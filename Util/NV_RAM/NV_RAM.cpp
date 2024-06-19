#include "NV_RAM.h"
NV_RAM::NV_RAM(I2C *pInstance, uint16_t DevAddress) : EEPROM(pInstance, DevAddress) {}