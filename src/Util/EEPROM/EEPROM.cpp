#include "EEPROM.h"
EEPROM::EEPROM(I2C *pInstance, uint16_t DevAddress, uint16_t pages, uint16_t pageSize) {
	_pInstance = pInstance;
	_DevAddress = DevAddress;
	_pages = pages;
	_pageSize = pageSize;
}
EEPROM::EEPROM(SPI *pInstance, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint16_t pages, uint16_t pagesize) {
	_pInstanceSPI = pInstance;
	_CSPort = GPIOx;
	_CSPin = GPIO_Pin;
	_pages = pages;
	_pageSize = pagesize;
}
EEPROM::EEPROM(SPI *pInstance, uint8_t CS_Addres, uint16_t pages, uint16_t pagesize) {
	_pInstanceSPI = pInstance;
	_CS_Addres = CS_Addres;
	_pages = pages;
	_pageSize = pagesize;
}
void EEPROM::readFromMemory(
	uint16_t MemAddress, 
	uint8_t *pData, 
	uint16_t Size
) {
	_pInstance->readFromMemory(_DevAddress, MemAddress, pData, Size);
}
void EEPROM::writeToMemory(
	uint16_t MemAddress, 
	uint8_t *pData, 
	uint16_t Size
) {
	_pInstance->writeToMemory(_DevAddress, MemAddress, pData, Size);
}