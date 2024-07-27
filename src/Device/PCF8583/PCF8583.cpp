#include "PCF8583.h"

PCF8583::PCF8583(
	I2C *pInstance,
 	uint16_t DevAddress,
	uint32_t BaseAddress
) {
	_pInstance = pInstance;
	_DevAddress = DevAddress;
	_BaseAddress = BaseAddress;
}
void PCF8583::readFromMemory(
	uint32_t MemAddress,
	uint8_t *pData, 
	uint16_t Size
) {
	_pInstance->readFromMemory(_DevAddress, _BaseAddress + MemAddress, _MemAddSize, pData, Size);
}

void PCF8583::writeToMemory(
	uint32_t MemAddress,
	uint8_t *pData, 
	uint16_t Size
) {
	_pInstance->writeToMemory(_DevAddress, _BaseAddress + MemAddress, _MemAddSize, pData, Size);
}