#include "SPI.h"

#include "Application/Display/Display.h"
extern Display display;

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
				operationTimeout = millis()+2000;
				if(currentOperation.operationType == EoperationType::TRANSMIT) {
                    HAL_GPIO_WritePin(currentOperation.GPIOx, currentOperation.GPIO_Pin, GPIO_PIN_RESET);
					if(HAL_SPI_Transmit_DMA(
						_pHandler, 
						currentOperation.pData_tx,
						currentOperation.Size
					) == HAL_OK) {
						operationState = WAITING;
					}
				}
				else if(currentOperation.operationType == EoperationType::RECEIVE) {
                    HAL_GPIO_WritePin(currentOperation.GPIOx, currentOperation.GPIO_Pin, GPIO_PIN_RESET);
					if(HAL_SPI_Receive_DMA(
						_pHandler, 
						currentOperation.pData_rx,
						currentOperation.Size
					) == HAL_OK) {
						operationState = WAITING;
					}
				}
				else if(currentOperation.operationType == EoperationType::TRANSMIT_RECEIVE) {
                    HAL_GPIO_WritePin(currentOperation.GPIOx, currentOperation.GPIO_Pin, GPIO_PIN_RESET);
					if(HAL_SPI_TransmitReceive_DMA(
						_pHandler, 
						currentOperation.pData_tx,
						currentOperation.pData_rx,
						currentOperation.Size
					) == HAL_OK) {
						operationState = WAITING;
					}
				}
				break;
			}	
			case WAITING: {
				display.toggle(displayMap::U7);
				if(millis() >= operationTimeout) {
					operationState = FINISH;
				}
				break;
			}
			case FINISH: {
				if(currentOperation.callback_f != nullptr) {
					currentOperation.callback_f(
						currentOperation.operationType==EoperationType::TRANSMIT?
						currentOperation.pData_tx: currentOperation.pData_rx,
						currentOperation.Size
					);
				}
				operationState = CLEAR;
				break;
			}
			case CLEAR: {
				display.toggle(displayMap::S2);
				HAL_GPIO_WritePin(currentOperation.GPIOx, currentOperation.GPIO_Pin, GPIO_PIN_SET);
				if(currentOperation.free) free(currentOperation.pData_tx);
				operations.pop();
				operationState = IDLE;
				break;
			}
			default: {}
		}
	}, 50); 
}

void SPI::txInterrupt() {
	display.toggle(displayMap::U4);
	// if(operationState == WAITING) {
		operationState = FINISH;
	// }
}
void SPI::rxInterrupt() {
	display.toggle(displayMap::U5);
	if(operationState == WAITING) {
		operationState = FINISH;
	}
}

void SPI::errorInterrupt() {
	display.toggle(displayMap::U6);
	if(HAL_SPI_GetError(_pHandler) > HAL_SPI_ERROR_NONE) {
    	operationState = FINISH;
	}
}

void SPI::transmit(
	GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, 
	uint8_t *pData, uint16_t Size, 
	dataCallback_f callbackFn
) {
	operation operation;
	operation.operationType = EoperationType::TRANSMIT;
	operation.GPIOx = GPIOx;
    operation.GPIO_Pin = GPIO_Pin;
	operation.pData_tx = (uint8_t*) malloc(Size);
	operation.free = true;
	memcpy(operation.pData_tx, pData, Size);
	operation.Size = Size;
	operation.callback_f = callbackFn;
	operations.push(operation);
}

void SPI::receive(
	GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, 
	uint8_t *pData, uint16_t Size, 
	dataCallback_f callbackFn
) {
    operation operation;
	operation.operationType = EoperationType::RECEIVE;
    operation.GPIOx = GPIOx;
    operation.GPIO_Pin = GPIO_Pin;
	operation.pData_rx = pData;
	operation.Size = Size;
	operation.callback_f = callbackFn;
	operations.push(operation);
}
void SPI::transmitReceive(
	GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin,
	uint8_t *pData_tx,
	uint8_t *pData_rx,
	uint16_t Size,
	dataCallback_f callbackFn
) {
	operation operation;
	operation.operationType = EoperationType::TRANSMIT_RECEIVE;
	operation.GPIOx = GPIOx;
    operation.GPIO_Pin = GPIO_Pin;
	operation.pData_tx = (uint8_t*) malloc(Size);
	operation.free = true;
	memcpy(operation.pData_tx, pData_tx, Size);
	operation.pData_rx = pData_rx;
	operation.Size = Size;
	operation.callback_f = callbackFn;
	operations.push(operation);
}
uint16_t SPI::queueSize() {
	return operations.size();
}