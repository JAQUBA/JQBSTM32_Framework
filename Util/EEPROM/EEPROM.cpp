#include "EEPROM.h"

EEPROM::EEPROM(I2C *pInstance,
 	uint16_t DevAddress,
	uint32_t BaseAddress,
	uint16_t MemAddSize) {
	
	_pInstanceI2C = pInstance;
	_DevAddress = DevAddress;
	_BaseAddress = BaseAddress;
	_MemAddSize = MemAddSize;
    isSPI = false;
}

EEPROM::EEPROM(SPI *pInstance,
 	GPIO_TypeDef* GPIOx,
	uint16_t GPIO_Pin,
	uint32_t BaseAddress,
	uint16_t MemAddSize) {

	_pInstance = pInstance;
	_CSPort = GPIOx;
	_CSPin = GPIO_Pin;
	_BaseAddress = BaseAddress;
	_MemAddSize = MemAddSize;
    isSPI = true;
}

void EEPROM::readFromMemory(
	uint32_t MemAddress,
	uint16_t MemAddSize,
	uint8_t *pData, 
	uint16_t Size
) {
	uint32_t mem_address = _BaseAddress + MemAddress;

	if(isSPI) {
		_pInstanceSPI->readFromMemory(_CSPort, _CSPin, mem_address, MemAddSize, pData, Size);
	}else{
		_pInstanceI2C->readFromMemory(_DevAddress, mem_address, pData, Size);
	}

}

void EEPROM::writeToMemory(
	uint32_t MemAddress,
	uint16_t MemAddSize, 
	uint8_t *pData, 
	uint16_t Size
) {
	uint32_t mem_address = _BaseAddress + MemAddress;

	if(isSPI) {
		_pInstanceSPI->writeToMemory(_CSPort, _CSPin, mem_address, MemAddSize, pData, Size);
	}else{
		_pInstanceI2C->writeToMemory(_DevAddress, mem_address, pData, Size);
	}
}