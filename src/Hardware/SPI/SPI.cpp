/*
 * JQBSTM32 Framework - SPI.cpp Implementation
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
#include "SPI.h"
#ifdef HAL_SPI_MODULE_ENABLED

SPI *_SPI_instances[SPI_MAX_INSTANCES];
uint8_t _SPI_instancesNum = 0;

SPI *SPI::getInstance(SPI_HandleTypeDef *pHandler) {
    for (size_t i = 0; i < _SPI_instancesNum; i++) {
        if(_SPI_instances[i]->_pHandler->Instance == pHandler->Instance) return _SPI_instances[i];
    }
    return nullptr;
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi) {SPI::getInstance(hspi)->txInterrupt();}
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi) {SPI::getInstance(hspi)->rxInterrupt();}
void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi) {SPI::getInstance(hspi)->errorInterrupt();}

void SPI::txInterrupt() {if(operationState == WAITING) { operationState = FINISH; }}
void SPI::rxInterrupt() {if(operationState == WAITING) { operationState = FINISH; } }
void SPI::errorInterrupt() {if(HAL_SPI_GetError(_pHandler) > HAL_SPI_ERROR_NONE) { operationState = FINISH; }}

SPI::SPI(SPI_HandleTypeDef *pHandler) : _operationHead(0), _operationTail(0), _operationCount(0) {
     _pHandler = pHandler;
	_SPI_instances[_SPI_instancesNum++] = this;
	
	// Initialize operations array
	for (uint8_t i = 0; i < SPI_MAX_OPERATIONS; i++) {
	    _operations[i].active = false;
	}
	
	addTaskMain(taskCallback {
		switch(operationState) {
			case IDLE: {
				if(getNextOperation()) {
					operationState = CHECK_FREE;
				} else break;
			}
			case CHECK_FREE: {
				if(HAL_SPI_GetState(_pHandler) == HAL_SPI_STATE_READY) {
					operationState = WORK;
				} else break;
			}
			case WORK: {
				operationTimeout = millis()+4;
				HAL_GPIO_WritePin(currentOperation._CSPort, currentOperation._CSPin, GPIO_PIN_RESET);
				operationState = WAITING;
				if(currentOperation.operationType == EoperationType::TRANSMIT) {
					if(HAL_SPI_Transmit_DMA(
						_pHandler, 
						currentOperation.pData,
						currentOperation.Size
					) == HAL_OK) {
					}
				}
				else if(currentOperation.operationType == EoperationType::RECEIVE) {
					if(HAL_SPI_Receive_DMA(
						_pHandler, 
						currentOperation.pData,
						currentOperation.Size
					) == HAL_OK) {
					}
				}
				// else if(currentOperation.operationType == EoperationType::TRANSMIT_RECEIVE) {
					// if(HAL_SPI_TransmitReceive_DMA(
					// 	_pHandler, 
					// 	currentOperation.pData_tx,
					// 	currentOperation.pData_rx,
					// 	currentOperation.Size
					// ) == HAL_OK) {
						// operationState = WAITING;
					// }
				// }
				break;
			}	
			case WAITING: {
				if(millis() >= operationTimeout) {
					operationState = FINISH;
				} else break;
			}
			case FINISH: {
				if(currentOperation.callback_f != nullptr) {
					currentOperation.callback_f(
						currentOperation.pData,
						currentOperation.Size
					);
					operationState = CLEAR;
					break;
				}
			}			case CLEAR: {
				if(currentOperation._pinReset) HAL_GPIO_WritePin(currentOperation._CSPort, currentOperation._CSPin, GPIO_PIN_SET);
				// No need to free memory - using stack buffers
				operationState = IDLE;
				break;
			}
		}
	}); 
}

bool SPI::getNextOperation() {
    if (_operationCount == 0) return false;
    
    currentOperation = _operations[_operationTail];
    _operationTail = (_operationTail + 1) % SPI_MAX_OPERATIONS;
    _operationCount--;
    
    return true;
}

bool SPI::enqueueOperation(const operation& op) {
    if (_operationCount >= SPI_MAX_OPERATIONS) return false;
    
    _operations[_operationHead] = op;
    _operations[_operationHead].active = true;
    _operationHead = (_operationHead + 1) % SPI_MAX_OPERATIONS;
    _operationCount++;
    
    return true;
}

bool SPI::transmit(
	GPIO_TypeDef* CSPort, uint16_t CSPin, 
	uint8_t *pData, uint16_t Size, 
	dataCallback_f callbackFn
) {
	if (Size > SPI_MAX_DATA_SIZE) return false;
	
	operation op;
	op.operationType = EoperationType::TRANSMIT;
	op._CSPort = CSPort;
    op._CSPin = CSPin;
	op.Size = Size;
	op.callback_f = callbackFn;
	
	// Copy data to internal buffer
	memcpy(op.internalData, pData, Size);
	op.pData = op.internalData;
	op.useInternalBuffer = true;
	
	return enqueueOperation(op);
}

bool SPI::receive(
	GPIO_TypeDef* CSPort, uint16_t CSPin, 
	uint8_t *pData, uint16_t Size, 
	dataCallback_f callbackFn
) {
    operation op;
	op.operationType = EoperationType::RECEIVE;
    op._CSPort = CSPort;
    op._CSPin = CSPin;
	op.pData = pData;
	op.Size = Size;
	op.useInternalBuffer = false;
	op.callback_f = callbackFn;
	
	return enqueueOperation(op);
}
bool SPI::transmitThenReceive(
	GPIO_TypeDef* CSPort, uint16_t CSPin,
	uint8_t *pData_tx, uint16_t txSize,
	uint8_t *pData_rx, uint16_t rxSize,
	dataCallback_f callbackFn
) {
	if (isQueueFull() || txSize > SPI_MAX_DATA_SIZE) {
		return false;
	}
	
	operation op;
	op.operationType = EoperationType::TRANSMIT;
	op._CSPort = CSPort;
	op._CSPin = CSPin;
	op._pinReset = false;
	op.Size = txSize;
	op.callback_f = nullptr; // No callback for intermediate operation
	
	// Copy transmit data to internal buffer
	memcpy(op.internalData, pData_tx, txSize);
	op.pData = op.internalData;
	op.useInternalBuffer = true;
	
	enqueueOperation(op);
	
	// Then queue receive operation with the original callback
	return receive(CSPort, CSPin, pData_rx, rxSize, callbackFn);
}

bool SPI::transmitReceive(
	GPIO_TypeDef* CSPort, uint16_t CSPin,
	uint8_t *pDataTx, uint8_t *pDataRx, uint16_t Size,
	dataCallback_f callbackFn
) {
	// This would require full-duplex operation - not implemented yet
	// For now, perform sequential transmit then receive
	return transmitThenReceive(CSPort, CSPin, pDataTx, Size, pDataRx, Size, callbackFn);
}

uint16_t SPI::queueSize() {
	return _operationCount;
}

bool SPI::isQueueFull() {
    return _operationCount >= SPI_MAX_OPERATIONS;
}

uint16_t SPI::getMaxQueueSize() {
    return SPI_MAX_OPERATIONS;
}

SPI_HandleTypeDef* SPI::getHandler() {
    return _pHandler;
}

#endif
