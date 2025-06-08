/*
 * JQBSTM32 Framework - EEP24C256.cpp Implementation
 * Copyright (C) 2024 JAQUBA (kjakubowski0492@gmail.com)
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
#include "EEP24C256.h"
#ifdef __EEP24C256_H_

EEP24C256::EEP24C256(
	I2C *pInstance,
 	uint16_t DevAddress,
	uint32_t BaseAddress
) {
	_pInstance = pInstance;
	_DevAddress = DevAddress;
	_BaseAddress = BaseAddress;
}
void EEP24C256::readFromMemory(
	uint32_t MemAddress,
	uint8_t *pData, 
	uint16_t Size
) {
	_pInstance->readFromMemory(_DevAddress, _BaseAddress + MemAddress, _MemAddSize, pData, Size);
}

void EEP24C256::writeToMemory(
	uint32_t MemAddress,
	uint8_t *pData, 
	uint16_t Size
) {
	_pInstance->writeToMemory(_DevAddress, _BaseAddress + MemAddress, _MemAddSize, pData, Size);
}
#endif
