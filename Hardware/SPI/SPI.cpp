#include "SPI.h"

SPI *_SPI_instances[SPI_MAX_INSTANCES];
uint8_t _SPI_instancesNum = 0;

SPI *SPI::getInstance(SPI_HandleTypeDef *pHandler) {
    for (size_t i = 0; i < _SPI_instancesNum; i++) {
        if(_SPI_instances[i]->_pHandler->Instance == pHandler->Instance) return _SPI_instances[i];
    }
    return nullptr;
}
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi) {
    SPI::getInstance(hspi)->txInterrupt();
}
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi) {
    SPI::getInstance(hspi)->rxInterrupt();
}
void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi) {
    SPI::getInstance(hspi)->errorInterrupt();
}

SPI::SPI(SPI_HandleTypeDef *pHandler) {
     _pHandler = pHandler;
	_SPI_instances[_SPI_instancesNum++] = this;
	addTaskMain(taskCallback {
		switch(operationState) {
			case IDLE: {
				if(!operations.empty()) {
					currentOperation = operations.front();
					operationState = CHECK_FREE;
				}
				break;
			}
			case CHECK_FREE: {
				if(HAL_SPI_GetState(_pHandler) == HAL_SPI_STATE_READY) {
					operationState = WORK;
				}
				break;
			}
			case WORK: {
				operationTimeout = millis()+20;
				if(currentOperation.operationType == EoperationType::TRANSMIT) {
                    HAL_GPIO_WritePin(currentOperation.GPIOx, currentOperation.GPIO_Pin, GPIO_PIN_RESET);
					if(HAL_SPI_Transmit(
						_pHandler, 
						currentOperation.pData,
						currentOperation.Size, 20
					) == HAL_OK) {
						operationState = WAITING;
					}
				}
				else if(currentOperation.operationType == EoperationType::RECEIVE) {
                    HAL_GPIO_WritePin(currentOperation.GPIOx, currentOperation.GPIO_Pin, GPIO_PIN_RESET);
					if(HAL_SPI_Receive_DMA(
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
					operationState = FINISH;
				}
				break;
			}
			case FINISH: {
				if(currentOperation.callback_f != nullptr) currentOperation.callback_f(currentOperation.pData,
						currentOperation.Size);
				operationState = CLEAR;
				break;
			}
			case CLEAR: {
				if(currentOperation.free) free(currentOperation.pData);
				operations.pop();
                HAL_GPIO_WritePin(currentOperation.GPIOx, currentOperation.GPIO_Pin, GPIO_PIN_SET);
				operationState = IDLE;
				break;
			}
			default: {}
		}
	}, 0); 
}

void SPI::txInterrupt() {
	if(operationState == WAITING) {
		operationState = FINISH;
	}
}
void SPI::rxInterrupt() {
	if(operationState == WAITING) {
		operationState = FINISH;
	}
}
void SPI::errorInterrupt() {
	if (HAL_SPI_GetError(_pHandler) > HAL_SPI_ERROR_NONE) {
		operationState = FINISH;
	}
}

void SPI::receive(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint8_t *pData, uint16_t Size, dataCallback_f callbackFn) {
    operation operation;
	operation.operationType = EoperationType::RECEIVE;
    operation.GPIOx = GPIOx;
    operation.GPIO_Pin = GPIO_Pin;
	operation.pData = pData;
	operation.Size = Size;
	operation.callback_f = callbackFn;
	operation.free = false;
	operations.push(operation);
}
void SPI::transmit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint8_t *pData, uint16_t Size, dataCallback_f callbackFn) {
	operation operation;
	operation.operationType = EoperationType::TRANSMIT;
	operation.GPIOx = GPIOx;
    operation.GPIO_Pin = GPIO_Pin;
	operation.pData = (uint8_t*) malloc(Size);
	memcpy(operation.pData, pData, Size);
	operation.Size = Size;
	operation.callback_f = callbackFn;
	operations.push(operation);
}

void SPI::readFromMemory(uint16_t DevAddress, uint16_t MemAddress, uint8_t *pData, uint16_t Size, dataCallback_f callbackFn) {
	operation operation;
	operation.operationType = EoperationType::MEM_READ;
	//operation.DevAddress = DevAddress;
	//operation.MemAddress = MemAddress;
	operation.pData = pData;
	operation.Size = Size;
	operation.callback_f = callbackFn;
	operation.free = false;
	operations.push(operation);
}
void SPI::writeToMemory(uint16_t DevAddress, uint16_t MemAddress, uint8_t *pData, uint16_t Size, dataCallback_f callbackFn) {
	operation operation;
	operation.operationType = EoperationType::MEM_WRITE;
	//operation.DevAddress = DevAddress;
	//operation.MemAddress = MemAddress;
	operation.pData = (uint8_t*) malloc(Size);
	memcpy(operation.pData, pData, Size);
	operation.Size = Size;
	operation.callback_f = callbackFn;
	operations.push(operation);
}

uint16_t SPI::queueSize() {
	return operations.size();
}