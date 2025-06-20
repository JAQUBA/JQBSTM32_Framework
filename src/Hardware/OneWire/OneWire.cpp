/*
 * JQBSTM32 Framework - OneWire Protocol Implementation
 * Copyright (C) 2024 JAQUBA (kjakubowski0492@gmail.com)
 * 
 * Implementation according to Dallas/Maxim 1-Wire specification.
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

#include "OneWire.h"

OneWire::OneWire(Timer* timer, GPIO_TypeDef* GPIO_Port, uint16_t GPIO_Pin) 
    : OW_Timer(timer), OW_Port(GPIO_Port), OW_Pin(GPIO_Pin), _operationHead(0), _operationTail(0), _operationCount(0) {
    
    // Initialize operations array
    for (uint8_t i = 0; i < ONEWIRE_MAX_OPERATIONS; i++) {
        _operations[i].active = false;
    }
	OW_Timer->attachInterrupt(Timer::PeriodElapsedCallback, voidCallback {
		switch(operationProgress) {
			case OPERATION_PROGRESS_IDLE: {
				if(operationState == WAITING) operationState = FINISH;
				break;
			}
			case OPERATION_PROGRESS_RESET: {
				HAL_GPIO_WritePin(OW_Port, OW_Pin, GPIO_PIN_RESET);
				OW_Timer->setPeriod(480);
				operationProgress = OPERATION_PROGRESS_RESET_WAIT_LOW;
				break;
			}
			case OPERATION_PROGRESS_RESET_WAIT_LOW: {
				HAL_GPIO_WritePin(OW_Port, OW_Pin, GPIO_PIN_SET);
				OW_Timer->setPeriod(70);
				operationProgress=OPERATION_PROGRESS_RESET_WAIT_HIGH;
				break;
			}
			case OPERATION_PROGRESS_RESET_WAIT_HIGH: {
				is_device_presence = HAL_GPIO_ReadPin(OW_Port, OW_Pin);
				OW_Timer->setPeriod(410);
				operationProgress=OPERATION_PROGRESS_IDLE;
				break;
			}

			case OPERATION_PROGRESS_WRITE_START: {
				ow_byte = *(currentOperation.pData + ow_byte_index);
				ow_bit_index = 0;
			}
			case OPERATION_PROGRESS_WRITE_BIT: {
				HAL_GPIO_WritePin(OW_Port, OW_Pin, GPIO_PIN_RESET);
				OW_Timer->setPeriod(ow_byte & 0x01?6:60);
				operationProgress=OPERATION_PROGRESS_WRITE_END;
				break;
			}
			case OPERATION_PROGRESS_WRITE_END: {
				HAL_GPIO_WritePin(OW_Port, OW_Pin, GPIO_PIN_SET);
				OW_Timer->setPeriod(ow_byte & 0x01?64:10);
				if (++ow_bit_index < 8) {
					ow_byte >>= 1;
					operationProgress = OPERATION_PROGRESS_WRITE_BIT;
				} else {
					if (++ow_byte_index<currentOperation.Size) {
						operationProgress = OPERATION_PROGRESS_WRITE_START;
						break;
					}
					operationProgress = OPERATION_PROGRESS_IDLE;
				}
				break;
			}

			case OPERATION_PROGRESS_READ_START: {
				ow_bit_index = 0;
				ow_byte = 0;
			}
			case OPERATION_PROGRESS_READ_BIT: {
				HAL_GPIO_WritePin(OW_Port, OW_Pin, GPIO_PIN_RESET);
				OW_Timer->setPeriod(10);
				operationProgress=OPERATION_PROGRESS_READ_WAIT;
				break;
			}
			case OPERATION_PROGRESS_READ_WAIT: {
				HAL_GPIO_WritePin(OW_Port, OW_Pin, GPIO_PIN_SET);
				OW_Timer->setPeriod(9);
				operationProgress=OPERATION_PROGRESS_READ_END;
				break;
			}
			case OPERATION_PROGRESS_READ_END: {
				if (HAL_GPIO_ReadPin(OW_Port, OW_Pin))	ow_byte |= 0x80;
				OW_Timer->setPeriod(55);
				if (++ow_bit_index < 8) {
					ow_byte >>= 1;
					operationProgress = OPERATION_PROGRESS_READ_BIT;
				} else {
					*(currentOperation.pData + ow_byte_index) = ow_byte;
					if(++ow_byte_index<currentOperation.Size) {
						operationProgress = OPERATION_PROGRESS_READ_START;
						break;
					}
					operationProgress = OPERATION_PROGRESS_IDLE;
				}
				break;
			}
		}
	});
	addTaskMain(taskCallback {
		switch(operationState) {
			case IDLE: {
				if(getNextOperation()) {
					operationState = CHECK_FREE;
					operationState = CHECK_FREE;
				}
				break;
			}
			case CHECK_FREE: {
				if(operationProgress == OPERATION_PROGRESS_IDLE) {
					operationState = WORK;
				}
				break;
			}
			case WORK: {
				operationTimeout = millis()+40;
				if(currentOperation.operationType == EoperationType::RESET) {
					is_device_presence = false;
					operationProgress = OPERATION_PROGRESS_RESET;
				} else if(currentOperation.operationType == EoperationType::TRANSMIT) {
					ow_byte_index = 0;
					operationProgress = OPERATION_PROGRESS_WRITE_START;

				} else if(currentOperation.operationType == EoperationType::RECEIVE) {
					ow_byte_index = 0;
					operationProgress = OPERATION_PROGRESS_READ_START;
				}
				operationState = WAITING;
				break;
			}
			case WAITING: {
	   			if (millis() >= operationTimeout) {
					operationState = FINISH;
				}
				break;
			}
			case FINISH: {
				if(currentOperation.callback_f != nullptr) {
					currentOperation.callback_f(
						currentOperation.pData,
						currentOperation.Size
					);
				}
				operationState = CLEAR;
				break;
			}			case CLEAR: {
				// No need to free memory - using stack buffers or external pointers
				operationState = IDLE;
				break;
			}
		}
	});
}

bool OneWire::getNextOperation() {
    if (_operationCount == 0) return false;
    
    currentOperation = _operations[_operationTail];
    _operationTail = (_operationTail + 1) % ONEWIRE_MAX_OPERATIONS;
    _operationCount--;
    
    return true;
}

bool OneWire::enqueueOperation(const operation& op) {
    if (_operationCount >= ONEWIRE_MAX_OPERATIONS) return false;
    
    _operations[_operationHead] = op;
    _operations[_operationHead].active = true;
    _operationHead = (_operationHead + 1) % ONEWIRE_MAX_OPERATIONS;
    _operationCount++;
    
    return true;
}

void OneWire::reset() {
	operation op;
	op.operationType = EoperationType::RESET;
	op.useInternalBuffer = false;
	enqueueOperation(op);
}

void OneWire::transmit(
	const uint8_t* pData, uint16_t size,
	dataCallback_f callbackFn
){
	if (size > ONEWIRE_MAX_DATA_SIZE) return;
	
	operation op;
	op.operationType = EoperationType::TRANSMIT;
	op.Size = size;
	op.callback_f = callbackFn;
	
	// Copy data to internal buffer
	memcpy(op.internalData, pData, size);
	op.pData = op.internalData;
	op.useInternalBuffer = true;
	
	enqueueOperation(op);
}

void OneWire::receive(
	uint8_t* pData, uint16_t size,
	dataCallback_f callbackFn
){
	operation op;
	op.operationType = EoperationType::RECEIVE;
	op.pData = pData;
	op.Size = size;
	op.useInternalBuffer = false;
	op.callback_f = callbackFn;
	enqueueOperation(op);
}

void OneWire::transmitThenReceive(
	const uint8_t* pData_tx, uint16_t txSize,
	uint8_t* pData_rx, uint16_t rxSize,
	dataCallback_f callbackFn
){
	if (txSize > ONEWIRE_MAX_DATA_SIZE) return;
	
	operation op;
	op.operationType = EoperationType::TRANSMIT;
	op.Size = txSize;
	
	// Copy data to internal buffer
	memcpy(op.internalData, pData_tx, txSize);
	op.pData = op.internalData;
	op.useInternalBuffer = true;
	
	enqueueOperation(op); // transmit
	receive(pData_rx, rxSize, callbackFn);
}

void OneWire::transaction(
	uint8_t romCommand,
	const uint8_t* address,
	uint8_t functionCommand,
	const uint8_t* pData_tx,
	uint16_t txSize,
	uint8_t* pData_rx,
	uint16_t rxSize,
	dataCallback_f callbackFn,
	bool resetAfterTransaction
){
	uint8_t Size;
	operation op;

	reset();

	op.operationType = EoperationType::TRANSMIT;
	if (address==NULL){
		Size = 2;
		if (txSize>0) Size += txSize;
		
		if (Size > ONEWIRE_MAX_DATA_SIZE) return;
		
		op.internalData[0] = romCommand;
		op.internalData[1] = functionCommand;
		if (txSize>0) memcpy(op.internalData+2, pData_tx, txSize);
		op.pData = op.internalData;
	} else {
		Size=10;
		if (txSize>0) Size += txSize;
		
		if (Size > ONEWIRE_MAX_DATA_SIZE) return;
		
		op.internalData[0] = romCommand;
		memcpy(op.internalData+1, address, 8);
		op.internalData[9] = functionCommand;		if (txSize>0) memcpy(op.internalData+10, pData_tx, txSize);
		op.pData = op.internalData;
	}
	op.Size = Size;
	op.useInternalBuffer = true;
	enqueueOperation(op);

	if (rxSize>0) receive(pData_rx, rxSize, callbackFn);
	if (resetAfterTransaction) reset();
}       

uint16_t OneWire::queueSize() {
	return _operationCount;
}

bool OneWire::isDevicePresent() {
	return !is_device_presence; // Inverted logic - LOW means device present
}

bool OneWire::isBusy() {
	return operationState != IDLE || operationProgress != OPERATION_PROGRESS_IDLE;
}

void OneWire::clearQueue() {
	// Reset circular buffer
	_operationHead = 0;
	_operationTail = 0;
	_operationCount = 0;
	
	// Mark all operations as inactive
	for (uint8_t i = 0; i < ONEWIRE_MAX_OPERATIONS; i++) {
		_operations[i].active = false;
	}
}

uint8_t OneWire::calculateCRC8(const uint8_t* data, uint8_t length) {
	uint8_t crc = 0;
	
	for(uint8_t i = 0; i < length; i++) {
		uint8_t inbyte = data[i];
		for(uint8_t j = 0; j < 8; j++) {
			uint8_t mix = (crc ^ inbyte) & 0x01;
			crc >>= 1;
			if(mix) crc ^= 0x8C; // Dallas/Maxim CRC8 polynomial
			inbyte >>= 1;
		}
	}
	
	return crc;
}

bool OneWire::validateCRC(const uint8_t* data, uint8_t length, uint8_t expectedCRC) {
	return calculateCRC8(data, length) == expectedCRC;
}