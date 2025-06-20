/*
 * JQBSTM32 Framework - UART.cpp Implementation
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
#include "UART.h"
#ifdef HAL_UART_MODULE_ENABLED

UART *_UART_instances[UART_MAX_INSTANCES];
uint8_t _UART_instancesNum;

UART *UART::getInstance(UART_HandleTypeDef *pHandler) {
    for (size_t i = 0; i < _UART_instancesNum; i++) {
        if(_UART_instances[i]->_pHandler->Instance == pHandler->Instance) return _UART_instances[i];
    }
    return nullptr;
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {UART::getInstance(huart)->rxInterrupt();}
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {UART::getInstance(huart)->txInterrupt();}
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {UART::getInstance(huart)->errorInterrupt();}

UART::UART(UART_HandleTypeDef *pHandler, GPIO_TypeDef *dirPort, uint16_t dirPin) 
    : _operationHead(0), _operationTail(0), _operationCount(0) {
    _pHandler = pHandler;
    _UART_instances[_UART_instancesNum++] = this;
	_dirPort = dirPort;
	_dirPin = dirPin;
    
    // Initialize operations array
    for (uint8_t i = 0; i < UART_MAX_OPERATIONS; i++) {
        _operations[i].active = false;
    }
    
    HAL_UART_Receive_IT(_pHandler, &Received_u1, 1);

    addTaskMain(taskCallback {
        if(received && millis() > lastReceivedByte + 2) {
            if(fpOnReceive) fpOnReceive(rx_buffer, rx_data_index);
            rx_data_index = 0;
            received = false;
        }

        switch(operationState) {
			case IDLE: {
				if(getNextOperation()) {
					operationState = CHECK_FREE;
				}
				break;
			}
			case CHECK_FREE: {
				if(HAL_UART_GetState(_pHandler) & ~HAL_UART_STATE_BUSY_TX) {
					operationState = WORK;
				}
				break;
			}
			case WORK: {
				operationTimeout = millis()+200;
				if(currentOperation.operationType == EoperationType::SEND) {
					if(_dirPort) HAL_GPIO_WritePin(_dirPort, _dirPin, GPIO_PIN_SET);
					if(HAL_UART_Transmit_DMA(
						_pHandler,
						currentOperation.pData,
						currentOperation.Size
					) == HAL_OK) {
						operationState = WAITING;
					}
				}
				break;
			}
			case WAITING: {
				if(millis() >= operationTimeout) {
					operationState = CLEAR;
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
				fpOnTransmit();
				operationState = CLEAR;
				break;
			}			case CLEAR: {
				if(_dirPort) HAL_GPIO_WritePin(_dirPort, _dirPin, GPIO_PIN_RESET);
				// No need to free memory - using stack buffers
				operationState = IDLE;
				break;
			}
			default: {}
		}
    });
}


void UART::rxInterrupt() {
    rx_buffer[rx_data_index++] = Received_u1;
    if(rx_data_index >= 256) {
		rx_data_index = 0;
	}
    lastReceivedByte = millis();
    received = true;
    HAL_UART_Receive_IT(_pHandler, &Received_u1, 1);
}
void UART::txInterrupt() {
    if(operationState == WAITING) {
		operationState = FINISH;
	}
}  
void UART::errorInterrupt() {
	if (HAL_UART_GetError(_pHandler) & HAL_UART_ERROR_DMA) {
		operationState = FINISH;
	}
}

void UART::onReceiveHandler(dataCallback_f onReceive) {fpOnReceive = onReceive;}
void UART::onTransmitHandler(voidCallback_f onTransmit) {fpOnTransmit = onTransmit;}

bool UART::getNextOperation() {
    if (_operationCount == 0) return false;
    
    currentOperation = _operations[_operationTail];
    _operationTail = (_operationTail + 1) % UART_MAX_OPERATIONS;
    _operationCount--;
    
    return true;
}

bool UART::enqueueOperation(const operation& op) {
    if (_operationCount >= UART_MAX_OPERATIONS) return false;
    
    _operations[_operationHead] = op;
    _operations[_operationHead].active = true;
    _operationHead = (_operationHead + 1) % UART_MAX_OPERATIONS;
    _operationCount++;
    
    return true;
}

bool UART::transmit(uint8_t *pData, uint16_t Size, dataCallback_f callbackFn) {
    if (Size > UART_MAX_DATA_SIZE) return false;
    
    operation op;
	op.operationType = EoperationType::SEND;
	op.Size = Size;
	op.callback_f = callbackFn;
	
	// Copy data to internal buffer
	memcpy(op.internalData, pData, Size);
	op.pData = op.internalData;
	op.useInternalBuffer = true;
	
	return enqueueOperation(op);
}

uint16_t UART::queueSize() {
    return _operationCount;
}

bool UART::isQueueFull() {
    return _operationCount >= UART_MAX_OPERATIONS;
}
#endif
