#include "NV_RAM.h"
NV_RAM::NV_RAM(I2C *pInstance, uint16_t DevAddress, uint16_t pages, uint16_t pagesize) {
	_pInstanceI2C = pInstance;
	_DevAddress = DevAddress;
}

NV_RAM::NV_RAM(SPI *pInstance, GPIO_TypeDef* CSPort, uint16_t CSPin) {
    _pInstanceSPI = pInstance;
	_CSPort = CSPort;
	_CSPin = CSPin;
	isSPI = true;
}

void NV_RAM::readFromMemory(
	uint16_t MemAddress, 
	uint8_t *pData, 
	uint16_t Size
) {
	if(isSPI) {
		// _pInstanceSPI;
		//obsługa spi! TODO
		return;
	}
	_pInstanceI2C->readFromMemory(_DevAddress, MemAddress, pData, Size);
}
void NV_RAM::writeToMemory(
	uint16_t MemAddress, 
	uint8_t *pData, 
	uint16_t Size
) {
	if(isSPI) {
		// _pInstanceSPI;
		//obsługa spi! TODO
		return;
	}
	_pInstanceI2C->writeToMemory(_DevAddress, MemAddress, pData, Size);
}