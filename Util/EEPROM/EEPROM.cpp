#include "EEPROM.h"

EEPROM::EEPROM(I2C *pInstance,
 	uint16_t DevAddress,
	uint16_t MemAdress,
	uint16_t size) {

	_pInstanceI2C = pInstance;
	_DevAddress = DevAddress;
	_MemAdress = MemAdress;
    _size = size;
}
EEPROM::EEPROM(SPI *pInstance,
 	GPIO_TypeDef* GPIOx,
	uint16_t GPIO_Pin,
	uint16_t MemAdress,
	uint16_t size) {

	_pInstance = pInstance;
	_CSPort = GPIOx;
	_CSPin = GPIO_Pin;
	_MemAdress = MemAdress;
    _size = size;
}

EEPROM::EEPROM(SPI *pInstance,
 	GPIO_TypeDef* GPIOx,
	uint16_t GPIO_Pin,
	uint32_t MemAdress,
	uint16_t size) {

	_pInstance = pInstance;
	_CSPort = GPIOx;
	_CSPin = GPIO_Pin;
	_MemAdress = MemAdress;
    _size = size;
}


void EEPROM::readFromMemory(
	uint32_t MemAddress, 
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
	uint32_t MemAddress, 
	uint8_t *pData, 
	uint16_t Size
) {
	//_pInstanceI2C->writeToMemory(_DevAddress, MemAddress, pData, Size);
}