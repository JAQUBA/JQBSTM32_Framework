/*
 * JQBSTM32 Framework - FM25V05.cpp Implementation
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
#include "FM25V05.h"
#ifdef __FM25V05_H_

#define CMD_WREN 0x06 //0000 0110 Set Write Enable Latch
#define CMD_WRITE 0x02 //0000 0010 Write Memory Data
#define CMD_READ 0x03 //0000 0011 Read Memory Data

#define CMD_RDSR 0x05 //0000 0101 Read Status Register
#define CMD_WRSR 0x01 //0000 0001 Write Status Register
#define CMD_WRDI 0x04 //0000 0100 Write Disable
#define CMD_SLEEP 0xB9 // 1011 1001 Enter Sleep Mode
#define CMD_FASTREAD 0x0A // 0000 1011 Fast Read Memory Data
#define CMD_RDID 0x9F // 1001 1111 Read Device ID
#define CMD_SNR 0xC3 // 1100 0011 Read Device S/N


FM25V05::FM25V05(
	SPI *pInstance,
 	GPIO_TypeDef* GPIOx,
	uint16_t GPIO_Pin
) {
	_pInstance = pInstance;
	_CSPort = GPIOx;
	_CSPin = GPIO_Pin;
	HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_SET);
	
}

void FM25V05::readFromMemory(
	uint32_t MemAddress,
	uint8_t *pData, 
	uint16_t Size
) {
	uint8_t txBuf[] = {
		CMD_READ, 
		(uint8_t)((MemAddress & 0x0000FF00)>>8), 
		(uint8_t)((MemAddress & 0x000000FF))
	};
	_pInstance->transmitThenReceive(_CSPort, _CSPin, txBuf, sizeof(txBuf), pData, Size);
}

void FM25V05::writeToMemory(
	uint32_t MemAddress,
	uint8_t *pData, 
	uint16_t Size
) {
	// Enable write operation
	uint8_t wren = CMD_WREN;
	_pInstance->transmit(_CSPort, _CSPin, &wren, 1);

	// Use stack buffer for small writes, avoid malloc for better performance
	constexpr uint16_t STACK_BUFFER_SIZE = 128;
	
	if (Size <= STACK_BUFFER_SIZE - 3) {
		// Use stack buffer for small writes
		uint8_t txBuf[STACK_BUFFER_SIZE];
		
		txBuf[0] = CMD_WRITE;
		txBuf[1] = (uint8_t)((MemAddress & 0x0000FF00) >> 8);
		txBuf[2] = (uint8_t)(MemAddress & 0x000000FF);
		
		memcpy(txBuf + 3, pData, Size);
		_pInstance->transmit(_CSPort, _CSPin, txBuf, Size + 3);
	} else {
		// For large writes, send header first, then data
		uint8_t header[] = {
			CMD_WRITE, 
			(uint8_t)((MemAddress & 0x0000FF00) >> 8), 
			(uint8_t)(MemAddress & 0x000000FF)
		};
		
		// Manual CS control for continuous transaction
		HAL_GPIO_WritePin(_CSPort, _CSPin, GPIO_PIN_RESET);
				// Send header
		HAL_SPI_Transmit(_pInstance->getHandler(), header, 3, HAL_MAX_DELAY);
		
		// Send data
		HAL_SPI_Transmit(_pInstance->getHandler(), pData, Size, HAL_MAX_DELAY);
		
		HAL_GPIO_WritePin(_CSPort, _CSPin, GPIO_PIN_SET);
	}
}
#endif
