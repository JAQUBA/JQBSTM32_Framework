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
				operationTimeout = millis()+2;
				if(currentOperation.operationType == EoperationType::TRANSMIT) {
                    if(currentOperation.isCSSet) HAL_GPIO_WritePin(currentOperation._CSPort, currentOperation._CSPin, GPIO_PIN_SET);
					if(HAL_SPI_Transmit_DMA(
						_pHandler, 
						currentOperation.pData_tx,
						currentOperation.Size
					) == HAL_OK) {
						operationState = WAITING;
					}
				}
				else if(currentOperation.operationType == EoperationType::RECEIVE) {
                    if(currentOperation.isCSSet) HAL_GPIO_WritePin(currentOperation._CSPort, currentOperation._CSPin, GPIO_PIN_SET);
					if(HAL_SPI_Receive_DMA(
						_pHandler, 
						currentOperation.pData_rx,
						currentOperation.Size
					) == HAL_OK) {
						operationState = WAITING;
					}
				}
				else if(currentOperation.operationType == EoperationType::TRANSMIT_RECEIVE) {
                    if(currentOperation.isCSSet) HAL_GPIO_WritePin(currentOperation._CSPort, currentOperation._CSPin, GPIO_PIN_SET);
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
					operationState = CLEAR;
					break;
				}
			}
			case CLEAR: {
				if(currentOperation.isCSSet) HAL_GPIO_WritePin(currentOperation._CSPort, currentOperation._CSPin, GPIO_PIN_SET);
				if(currentOperation.free) free(currentOperation.pData_tx);
				operations.pop();
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
	if(HAL_SPI_GetError(_pHandler) > HAL_SPI_ERROR_NONE) {
    	operationState = FINISH;
	}
}

void SPI::transmit(
	GPIO_TypeDef* CSPort, uint16_t CSPin, 
	uint8_t *pData, uint16_t Size, 
	dataCallback_f callbackFn
) {
	operation operation;
	operation.operationType = EoperationType::TRANSMIT;
	operation._CSPort = CSPort;
    operation._CSPin = CSPin;
	operation.pData_tx = (uint8_t*) malloc(Size);
	operation.free = true;
	memcpy(operation.pData_tx, pData, Size);
	operation.Size = Size;
	operation.callback_f = callbackFn;
	operations.push(operation);
}

void SPI::receive(
	GPIO_TypeDef* CSPort, uint16_t CSPin, 
	uint8_t *pData, uint16_t Size, 
	dataCallback_f callbackFn
) {
    operation operation;
	operation.operationType = EoperationType::RECEIVE;
    operation._CSPort = CSPort;
    operation._CSPin = CSPin;
	operation.pData_rx = pData;
	operation.Size = Size;
	operation.callback_f = callbackFn;
	operations.push(operation);
}
void SPI::transmitReceive(
	GPIO_TypeDef* CSPort, uint16_t CSPin,
	uint8_t *pDataTx, uint8_t *pDataRx, uint16_t Size,
	dataCallback_f callbackFn
) {
	operation operation;
	operation.operationType = EoperationType::TRANSMIT_RECEIVE;
	operation._CSPort = CSPort;
    operation._CSPin = CSPin;
	operation.pData_tx = (uint8_t*) malloc(Size);
	operation.free = true;
	memcpy(operation.pData_tx, pDataTx, Size);
	operation.pData_rx = pDataRx;
	operation.Size = Size;
	operation.callback_f = callbackFn;
	operations.push(operation);
}
void SPI::transmitThenReceive(
	GPIO_TypeDef* _CSPort, uint16_t _CSPin,
	uint8_t *pData_tx, uint16_t txSize,
	uint8_t *pData_rx, uint16_t rxSize,
	dataCallback_f callbackFn
) {
	operation operation;
	operation.operationType = EoperationType::TRANSMIT;
	operation._CSPort = _CSPort;
    operation._CSPin = _CSPin;
	operation.pData_tx = (uint8_t*) malloc(txSize);
	operation.free = true;
	memcpy(operation.pData_tx, pData_tx, txSize);
	operation.Size = txSize;
	operation.isCSSet = false;
	operation.callback_f = [&](uint8_t *data, uint16_t size) {
		receive(_CSPort, _CSPin, pData_rx, rxSize);
	};
	operations.push(operation);
}
uint16_t SPI::queueSize() {
	return operations.size();
}