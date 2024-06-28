#include "I2C.h"
#include "Hardware/GPIO/GPIO.h"
#include "Application/portMap.h"

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

I2C::I2C(I2C_HandleTypeDef* pHandler) {
    _pHandler = pHandler;
	_I2C_instances[_I2C_instancesNum++] = this;
	addTaskMain([&](taskStruct *task) {
		switch(i2cState) {
			case IDLE: {
				if(!operations.empty()) {
					currentOperation = operations.front();
					i2cState = CHECK_FREE;
				}
				break;
			}
			case CHECK_FREE: {
				if(HAL_I2C_GetState(_pHandler) == HAL_I2C_STATE_READY) {
					i2cState = WORK;
				}
				break;
			}
			case WORK: {
				if(currentOperation.operationType == EoperationType::SEND) {
					if(HAL_I2C_Master_Transmit_DMA(
						_pHandler, 
						currentOperation.DevAddress,
						currentOperation.pData,
						currentOperation.Size
					) == HAL_OK) {
						i2cState = WAITING;
					}
				}
				// if(currentOperation.operationType == EoperationType::MEM_WRITE) {
				// 	if(HAL_I2C_Master_Transmit_DMA(
				// 		_pHandler, 
				// 		currentOperation.DevAddress,
				// 		currentOperation.pData,
				// 		currentOperation.Size
				// 	) == HAL_OK) {
				// 		i2cState = WAITING;
				// 	}
				// }
				break;
			}
			case FINISH: {
				///

				i2cState = CLEAR;
				break;
			}
			case CLEAR: {
				free(currentOperation.pData);
				operations.pop();
				i2cState = IDLE;
				break;
			}
			default: {}
		}
	}, 0);
}

void I2C::txInterrupt() {
	if(i2cState == WAITING) {
		i2cState = FINISH;
	}
}
void I2C::rxInterrupt() {
}

void I2C::send(uint16_t DevAddress, uint8_t *pData, uint16_t Size, std::function<void()> callbackFn) {
	i2cOperation operation;
	operation.operationType = EoperationType::SEND;
	operation.DevAddress = DevAddress;
	operation.pData = (uint8_t*)malloc(Size);
	memcpy(operation.pData, pData, Size);
	operation.Size = Size;
	operations.push(operation);
}
void I2C::receive(uint16_t DevAddress, uint8_t *pData, uint16_t Size, std::function<void()> callbackFn) {
    i2cOperation operation;
	operation.operationType = EoperationType::RECEIVE;
	operation.DevAddress = DevAddress;
	operation.pData = (uint8_t*)malloc(Size);
	memcpy(operation.pData, pData, Size);
	operation.Size = Size;
	operations.push(operation);
}

void I2C::readFromMemory(uint16_t DevAddress, uint16_t MemAddress, uint8_t *pData, uint16_t Size, std::function<void()> callbackFn) {
	i2cOperation operation;
	operation.operationType = EoperationType::MEM_READ;
	operation.DevAddress = DevAddress;
	operation.MemAddress = MemAddress;
	operation.pData = (uint8_t*)malloc(Size);
	memcpy(operation.pData, pData, Size);
	operation.Size = Size;
	operations.push(operation);
}
void I2C::writeToMemory(uint16_t DevAddress, uint16_t MemAddress, uint8_t *pData, uint16_t Size, std::function<void()> callbackFn) {
	i2cOperation operation;
	operation.operationType = EoperationType::MEM_WRITE;
	operation.DevAddress = DevAddress;
	operation.MemAddress = MemAddress;
	operation.pData = (uint8_t*)malloc(Size);
	memcpy(operation.pData, pData, Size);
	operation.Size = Size;
	operations.push(operation);
}