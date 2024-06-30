#include "NV_RAM.h"
NV_RAM::NV_RAM(I2C *pInstance, uint16_t DevAddress) {
	_pInstance = pInstance;
	_DevAddress = DevAddress;
}

NV_RAM::NV_RAM(SPI *pInstance, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin) {
    _pInstance2 = pInstance;
}

void NV_RAM::readFromMemory(
	uint16_t MemAddress, 
	uint8_t *pData, 
	uint16_t Size, 
	std::function<void(memOperation *operation)> functionPointer
) {
	_pInstance->readFromMemory(_DevAddress, MemAddress, pData, Size);
}
void NV_RAM::writeToMemory(
	uint16_t MemAddress, 
	uint8_t *pData, 
	uint16_t Size, 
	std::function<void(memOperation *operation)> functionPointer
) {
	_pInstance->writeToMemory(_DevAddress, MemAddress, pData, Size);
}