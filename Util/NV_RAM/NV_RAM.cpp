#include "NV_RAM.h"

NV_RAM::NV_RAM(I2C *instance, uint8_t address, uint16_t pages, uint16_t pagesize, uint16_t offset) {
	_instance = instance;
	_address = address;
	_pages = pages;
	_pageSize = pagesize;
	_offset = offset;
}
void NV_RAM::read16(uint16_t address, uint16_t *data, uint16_t size) {
	// HAL_I2C_Mem_Read(_instance, _address, ((address*2) + _offset), I2C_MEMADD_SIZE_8BIT, (uint8_t*)data, (size * 2), 1000);
}
void NV_RAM::write16(uint16_t address, uint16_t *data, uint16_t size) {
	// HAL_I2C_Mem_Write(_instance, _address, ((address*2) + _offset), I2C_MEMADD_SIZE_8BIT, (uint8_t*)data, (size * 2), 1000);
}