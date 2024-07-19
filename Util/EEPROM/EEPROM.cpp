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
	uint32_t BaseAddress) {

	_pInstance = pInstance;
	_CSPort = GPIOx;
	_CSPin = GPIO_Pin;
	_BaseAddress = BaseAddress;
    isSPI = true;
}

void EEPROM::readFromMemory(
	uint32_t MemAddress, 
	uint8_t *pData, 
	uint16_t Size
) {
	uint32_t mem_address = _BaseAddress + MemAddress;

	if(isSPI) {
		
		_pInstanceSPI->readFromMemory(mem_address, pData, Size);
	}else{
		_pInstanceI2C->readFromMemory(_DevAddress, mem_address, pData, Size);
	}

}

void EEPROM::writeToMemory(
	uint32_t MemAddress, 
	uint8_t *pData, 
	uint16_t Size
) {
	uint32_t mem_address = _BaseAddress + MemAddress;

	if(isSPI) {
		_pInstanceSPI->writeToMemory(mem_address, pData, Size);
	}else{
		_pInstanceI2C->writeToMemory(_DevAddress, mem_address, pData, Size);
	}
}