#include "EEP24C256.h"
#ifdef __EEP24C256_H_

EEP24C256::EEP24C256(
	I2C *pInstance,
 	uint16_t DevAddress,
	uint32_t BaseAddress
) {
	_pInstance = pInstance;
	_DevAddress = DevAddress;
	_BaseAddress = BaseAddress;
}
void EEP24C256::readFromMemory(
	uint32_t MemAddress,
	uint8_t *pData, 
	uint16_t Size
) {
	_pInstance->readFromMemory(_DevAddress, _BaseAddress + MemAddress, _MemAddSize, pData, Size);
}

void EEP24C256::writeToMemory(
	uint32_t MemAddress,
	uint8_t *pData, 
	uint16_t Size
) {
	_pInstance->writeToMemory(_DevAddress, _BaseAddress + MemAddress, _MemAddSize, pData, Size);
}
#endif