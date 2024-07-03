#include "EEPROM.h"
EEPROM::EEPROM(I2C *pInstance, uint16_t DevAddress, uint16_t pages, uint16_t pageSize) {
	_pInstance = pInstance;
	_DevAddress = DevAddress;
	_pages = pages;
	_pageSize = pageSize;
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