#include "EEPROM.h"

const uint8_t CMD_WREN = 0x06; //0000 0110 Set Write Enable Latch
const uint8_t CMD_WRITE = 0x02; //0000 0010 Write Memory Data
const uint8_t CMD_READ = 0x03; //0000 0011 Read Memory Data
const uint8_t CMD_RDSR = 0x05; //0000 0101 Read Status Register
const uint8_t CMD_WRSR = 0x01; //0000 0001 Write Status Register
const uint8_t CMD_WRDI = 0x04; //0000 0100 Write Disable
const uint8_t CMD_SLEEP = 0xB9; // 1011 1001 Enter Sleep Mode
const uint8_t CMD_FASTREAD = 0x0A; // 0000 1011 Fast Read Memory Data
const uint8_t CMD_RDID = 0x9F ; // 1001 1111 Read Device ID
const uint8_t CMD_SNR= 0xC3 ; // 1100 0011 Read Device S/N

EEPROM::EEPROM(
	I2C *pInstance,
 	uint16_t DevAddress,
	uint32_t BaseAddress,
	uint16_t MemAddSize
) {
	_pInstanceI2C = pInstance;
	_DevAddress = DevAddress;
	_BaseAddress = BaseAddress;
	_MemAddSize = MemAddSize;
    isSPI = false;
}
uint8_t rxBuf[300];

EEPROM::EEPROM(
	SPI *pInstance,
 	GPIO_TypeDef* GPIOx,
	uint16_t GPIO_Pin,
	uint32_t BaseAddress,
	uint16_t MemAddSize
) {
	_pInstance = pInstance;
	_CSPort = GPIOx;
	_CSPin = GPIO_Pin;
	_BaseAddress = BaseAddress;
	_MemAddSize = MemAddSize;
    isSPI = true;

	uint8_t txBuf[1] = {CMD_WREN};
	// _pInstanceSPI->transmitReceive(_CSPort, _CSPin, txBuf, rxBuf, 1);

}

void EEPROM::readFromMemory(
	uint32_t MemAddress,
	uint8_t *pData, 
	uint16_t Size
) {
	

	if(isSPI) {
		/*
		operation operation;
	operation.operationType = EoperationType::MEM_READ;

	operation.GPIOx = GPIOx;
    operation.GPIO_Pin = GPIO_Pin;
	operation.MemAddress = MemAddress;
	operation.MemAddSize = MemAddSize;
	operation.Size = Size + MemAddSize + 1;

	operation.pData_tx = (uint8_t*) malloc(operation.Size);
	memset(operation.pData_tx, 0, operation.Size);

	*(operation.pData_tx+0) = CMD_READ;

	if (MemAddSize==3) {
		*(operation.pData_tx+1) = (uint8_t)((MemAddress & 0x00FF0000)>>16);
		*(operation.pData_tx+2) = (uint8_t)((MemAddress & 0x0000FF00)>>8);
		*(operation.pData_tx+3) = (uint8_t)((MemAddress & 0x000000FF));
	} else if (MemAddSize==2) {
		*(operation.pData_tx+1) = (uint8_t)((MemAddress & 0x0000FF00)>>8);
		*(operation.pData_tx+2) = (uint8_t)((MemAddress & 0x000000FF));
	} else if (MemAddSize==1) {
		*(operation.pData_tx+1) = (uint8_t)((MemAddress & 0x000000FF));
	} else {
		//nieporawny add
	}
	operation.pData_rx = pData;
	operation.callback_f = callbackFn;
	operation.free = true;
	operations.push(operation);
	*/
		

	} else {
		uint16_t mem_address = _BaseAddress + MemAddress;
		_pInstanceI2C->readFromMemory(_DevAddress, mem_address, _MemAddSize, pData, Size);
	}

}

void EEPROM::writeToMemory(
	uint32_t MemAddress,
	uint8_t *pData, 
	uint16_t Size
) {
	// uint32_t mem_address = _BaseAddress + MemAddress;

	if(isSPI) {
		// _pInstanceSPI->writeToMemory(_CSPort, _CSPin, mem_address, MemAddSize, pData, Size);
		// uint8_t txBuf[32];
		// txBuf[0] = CMD_WRITE;
		/*
		operation operation;
	operation.operationType = EoperationType::MEM_WRITE;

	operation.GPIOx = GPIOx;
    operation.GPIO_Pin = GPIO_Pin;
	operation.MemAddress = MemAddress;
	operation.MemAddSize = MemAddSize;
	operation.Size = Size + MemAddSize + 1;

	operation.pData_tx = (uint8_t*) malloc(operation.Size);
	memset(operation.pData_tx, 0, operation.Size);

	*(operation.pData_tx+0) = CMD_WRITE;

	if (MemAddSize==3) {
		*(operation.pData_tx+1) = (uint8_t)((MemAddress & 0x00FF0000)>>16);
		*(operation.pData_tx+2) = (uint8_t)((MemAddress & 0x0000FF00)>>8);
		*(operation.pData_tx+3) = (uint8_t)((MemAddress & 0x000000FF));
	} else if (MemAddSize==2) {
		*(operation.pData_tx+1) = (uint8_t)((MemAddress & 0x0000FF00)>>8);
		*(operation.pData_tx+2) = (uint8_t)((MemAddress & 0x000000FF));
	} else if (MemAddSize==1) {
		*(operation.pData_tx+1) = (uint8_t)((MemAddress & 0x000000FF));
	} else {
		//nieporawny add
	}
	
	memcpy(operation.pData_tx+1+MemAddSize, pData, Size);

	operation.pData_rx = rdBuf;
	operation.callback_f = callbackFn;
	operation.free = true;
	operations.push(operation);*/


		
		// _pInstanceSPI->transmit(_CSPort, _CSPin, txBuf, Size);
	}else{
		uint16_t mem_address = _BaseAddress + MemAddress;
		_pInstanceI2C->writeToMemory(_DevAddress, mem_address, _MemAddSize, pData, Size);
	}
}