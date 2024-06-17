#include "NV_RAM.h"

NV_RAM::NV_RAM(I2C *pInstance, uint8_t address, uint16_t pages, uint16_t pagesize) {
	_pInstance = pInstance;

	// _address = address;
	// _pages = pages;
	// _pageSize = pagesize;
	// _offset = offset;
}
void NV_RAM::readFromMemory(uint16_t address, uint16_t *data, uint16_t size, bool blocking) {
	// HAL_I2C_Mem_Read(_pInstance, _address, ((address*2) + _offset), I2C_MEMADD_SIZE_8BIT, (uint8_t*)data, (size * 2), 1000);
}
void NV_RAM::writeToMemory(uint16_t address, uint16_t *data, uint16_t size, bool blocking) {
	// HAL_I2C_Mem_Write(_instance, _address, ((address*2) + _offset), I2C_MEMADD_SIZE_8BIT, (uint8_t*)data, (size * 2), 1000);
}