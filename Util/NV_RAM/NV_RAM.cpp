#include "NV_RAM.h"

NV_RAM::NV_RAM(I2C *pInstance,
 	uint16_t DevAddress,
	uint32_t BaseAddress) {

	_pInstanceI2C = pInstance;
	_DevAddress = DevAddress;
	_MemAdress = BaseAddress;
}

void NV_RAM::readFromMemory(
	uint32_t MemAddress, 
	uint8_t *pData, 
	uint16_t Size
) {
	_pInstanceI2C->readFromMemory(_DevAddress, MemAddress, pData, Size);
}
void NV_RAM::writeToMemory(
	uint32_t MemAddress, 
	uint8_t *pData, 
	uint16_t Size
) {
	_pInstanceI2C->writeToMemory(_DevAddress, MemAddress, pData, Size);
}