/*
 * JQBSTM32 Framework - I2C.cpp Implementation
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
#include "I2C.h"
#ifdef HAL_I2C_MODULE_ENABLED

I2C *_I2C_instances[I2C_MAX_INSTANCES];
uint8_t _I2C_instancesNum = 0;

I2C *I2C::getInstance(I2C_HandleTypeDef *pHandler) {
    for (size_t i = 0; i < _I2C_instancesNum; i++) {
        if(_I2C_instances[i]->_pHandler->Instance == pHandler->Instance) return _I2C_instances[i];
    }
    return nullptr;
}
void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c) {I2C::getInstance(hi2c)->rxInterrupt();}
void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c) {I2C::getInstance(hi2c)->txInterrupt();}
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c) {I2C::getInstance(hi2c)->errorInterrupt();}

void I2C::txInterrupt() {if(operationState == WAITING) {operationState = FINISH;}}
void I2C::rxInterrupt() {if(operationState == WAITING) {operationState = FINISH;}}
void I2C::errorInterrupt() {if (HAL_I2C_GetError(_pHandler) > HAL_I2C_ERROR_NONE) {operationState = FINISH;}}

I2C::I2C(I2C_HandleTypeDef* pHandler) {
    _pHandler = pHandler;
	_I2C_instances[_I2C_instancesNum++] = this;
	addTaskMain(taskCallback {
		switch(operationState) {
			case IDLE: {
				if(!operations.empty()) {
					currentOperation = operations.front();
					operationState = CHECK_FREE;
				} else break;
			}
			case CHECK_FREE: {
				if(HAL_I2C_GetState(_pHandler) == HAL_I2C_STATE_READY) {
					operationState = WORK;
				} else break;
			}
			case WORK: {
				operationTimeout = millis()+4;
				operationState = WAITING;
				if(currentOperation.operationType == EoperationType::TRANSMIT) {
					if(HAL_I2C_Master_Transmit_DMA(
						_pHandler, 
						currentOperation.DevAddress,
						currentOperation.pData,
						currentOperation.Size
					) == HAL_OK) {
					}
				}
				else if(currentOperation.operationType == EoperationType::RECEIVE) {
					if(HAL_I2C_Master_Receive_DMA(
						_pHandler, 
						currentOperation.DevAddress,
						currentOperation.pData,
						currentOperation.Size
					) == HAL_OK) { 
					}
				}
				else if(currentOperation.operationType == EoperationType::MEM_READ) {
					if(HAL_I2C_Mem_Read_DMA(
						_pHandler, 
						currentOperation.DevAddress,
						currentOperation.MemAddress,
						currentOperation.MemAddSize,
						currentOperation.pData,
						currentOperation.Size
					) == HAL_OK) {
					}
				}
				else if(currentOperation.operationType == EoperationType::MEM_WRITE) {
					if(HAL_I2C_Mem_Write_DMA(
						_pHandler, 
						currentOperation.DevAddress,
						currentOperation.MemAddress,
						currentOperation.MemAddSize,
						currentOperation.pData,
						currentOperation.Size
					) == HAL_OK) {
					}
				}
				break;
			}
			case WAITING: {
				if(millis() >= operationTimeout) {
					operationState = FINISH;
				} else break;
			}
			case FINISH: {
				if(currentOperation.callback_f != nullptr){
				   currentOperation.callback_f(
						currentOperation.pData,
						currentOperation.Size
					);
					operationState = CLEAR;
					break;
				}
			}
			case CLEAR: {
				if(currentOperation.free) free(currentOperation.pData);
				operations.pop();
				operationState = IDLE;
				break;
			}
		}
	});
}
void I2C::transmit(uint16_t DevAddress, uint8_t *pData, uint16_t Size, dataCallback_f callbackFn) {
	operation operation;
	operation.operationType = EoperationType::TRANSMIT;
	operation.DevAddress = DevAddress;
	operation.pData = (uint8_t*) malloc(Size);
	operation.free = true;
	memcpy(operation.pData, pData, Size);
	operation.Size = Size;
	operation.callback_f = callbackFn;
	operations.push(operation);
}
void I2C::receive(uint16_t DevAddress, uint8_t *pData, uint16_t Size, dataCallback_f callbackFn) {
    operation operation;
	operation.operationType = EoperationType::RECEIVE;
	operation.DevAddress = DevAddress;
	operation.pData = pData;
	operation.Size = Size;
	operation.callback_f = callbackFn;
	operation.free = false;
	operations.push(operation);
}
void I2C::readFromMemory(uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, dataCallback_f callbackFn) {
	operation operation;
	operation.operationType = EoperationType::MEM_READ;
	operation.DevAddress = DevAddress;
	operation.MemAddress = MemAddress;
	operation.MemAddSize = MemAddSize;
	operation.pData = pData;
	operation.Size = Size;
	operation.callback_f = callbackFn;
	operation.free = false;
	operations.push(operation);
}
void I2C::writeToMemory(uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, dataCallback_f callbackFn) {
	operation operation;
	operation.operationType = EoperationType::MEM_WRITE;
	operation.DevAddress = DevAddress;
	operation.MemAddress = MemAddress;
	operation.MemAddSize = MemAddSize;
	operation.pData = (uint8_t*) malloc(Size);
	operation.free = true;
	memcpy(operation.pData, pData, Size);
	operation.Size = Size;
	operation.callback_f = callbackFn;
	operations.push(operation);
}
uint16_t I2C::queueSize() {
	return operations.size();
}
#endif
