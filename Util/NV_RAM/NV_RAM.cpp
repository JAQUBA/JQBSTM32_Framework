#include "NV_RAM.h"

NV_RAM::NV_RAM(I2C *pInstance, uint16_t DevAddress) : EEPROM(pInstance, DevAddress) {

}
// NV_RAM::NV_RAM(I2C *pInstance, uint16_t DevAddress, uint16_t pages, uint16_t pageSize) {
// 	_pInstance = pInstance;
// 	_DevAddress = DevAddress;
// }
