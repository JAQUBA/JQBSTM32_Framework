#include "EEPROM.h"

EEPROM::EEPROM(I2C *pInstance, uint16_t DevAddress, uint16_t pages, uint16_t pageSize) {
	_pInstanceI2C = pInstance;
	_DevAddress = DevAddress;
	_pages = pages;
	_pageSize = pageSize;
}
EEPROM::EEPROM(SPI *pInstance, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint16_t pages, uint16_t pagesize) {
	//_pInstance = pInstance;
	_CSPort = GPIOx;
	_CSPin = GPIO_Pin;
	_pages = pages;
	_pageSize = pagesize;
}

void EEPROM::readFromMemory(
	uint16_t MemAddress, 
	uint8_t *pData, 
	uint16_t Size
) {
	if(isSPI) {
		_pInstanceSPI->readFromMemory(_DevAddress, MemAddress, pData, Size);	// _pInstanceSPI;
	}else{
		_pInstanceI2C->readFromMemory(_DevAddress, MemAddress, pData, Size);
	}



	
}

void EEPROM::writeToMemory(
	uint16_t MemAddress, 
	uint8_t *pData, 
	uint16_t Size
) {
	_pInstanceI2C->writeToMemory(_DevAddress, MemAddress, pData, Size);
}