#include "EEP24C04.h"
#ifdef __EEP24C04_H_

EEP24C04::EEP24C04(
	I2C *pInstance,
 	uint16_t DevAddress,
	uint32_t BaseAddress
) {
	_pInstance = pInstance;
	_DevAddress = DevAddress;
	_BaseAddress = BaseAddress;
}
void EEP24C04::readFromMemory(
	uint32_t MemAddress,
	uint8_t *pData, 
	uint16_t Size
) {
	_pInstance->readFromMemory(_DevAddress, _BaseAddress + MemAddress, _MemAddSize, pData, Size);
}

void EEP24C04::writeToMemory(
	uint32_t MemAddress,
	uint8_t *pData, 
	uint16_t Size
) {
	_pInstance->writeToMemory(_DevAddress, _BaseAddress + MemAddress, _MemAddSize, pData, Size);
}
#endif