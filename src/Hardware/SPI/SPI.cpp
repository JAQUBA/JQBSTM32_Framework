#include "SPI.h"

SPI *_SPI_instances[SPI_MAX_INSTANCES];
uint8_t _SPI_instancesNum = 0;

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

uint8_t rdBuf[300]= {0};
uint8_t wrBuf[300]= {0};

SPI *SPI::getInstance(SPI_HandleTypeDef *pHandler) {
    for (size_t i = 0; i < _SPI_instancesNum; i++) {
        if(_SPI_instances[i]->_pHandler->Instance == pHandler->Instance) return _SPI_instances[i];
    }
    return nullptr;
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi) {
    err=100;
    SPI::getInstance(hspi)->txInterrupt();
}
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi) {
	err=200;
    SPI::getInstance(hspi)->rxInterrupt();
}
void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi) {
	err=300;
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
				operationTimeout = millis()+3;
				if(currentOperation.operationType == EoperationType::TRANSMIT) {
                    HAL_GPIO_WritePin(currentOperation.GPIOx, currentOperation.GPIO_Pin, GPIO_PIN_RESET);
					if(HAL_SPI_TransmitReceive_DMA(
						_pHandler, 
						currentOperation.pData_tx,
						currentOperation.pData_rx,
						currentOperation.Size
					) == HAL_OK) {
						operationState = WAITING;
					}// else {//HAL ERR
						//operationState = FINISH;
					//}
				}
				else if(currentOperation.operationType == EoperationType::RECEIVE) {
                    HAL_GPIO_WritePin(currentOperation.GPIOx, currentOperation.GPIO_Pin, GPIO_PIN_RESET);
					if(HAL_SPI_TransmitReceive_DMA(
						_pHandler, 
						currentOperation.pData_tx,
						currentOperation.pData_rx,
						currentOperation.Size
					) == HAL_OK) {
						operationState = WAITING;
					} //else {
					//	operationState = FINISH;
					//}
				}
				else if(currentOperation.operationType == EoperationType::MEM_READ) {
					HAL_GPIO_WritePin(currentOperation.GPIOx, currentOperation.GPIO_Pin, GPIO_PIN_RESET);
					if(HAL_SPI_TransmitReceive_DMA(
						_pHandler, 
						currentOperation.pData_tx,
						currentOperation.pData_rx,
						currentOperation.Size
					) == HAL_OK) {
						operationState = WAITING;//WAIT_READ_END;
					}
				}
				else if(currentOperation.operationType == EoperationType::MEM_WRITE) {
					HAL_GPIO_WritePin(currentOperation.GPIOx, currentOperation.GPIO_Pin, GPIO_PIN_RESET);
					if(HAL_SPI_TransmitReceive_DMA(
						_pHandler, 
						currentOperation.pData_tx,
						currentOperation.pData_rx,
						currentOperation.Size
					) == HAL_OK) {
						operationState = WAITING;//WAIT_READ_END;
					}
					}
				}
				break;
		
			/*case WAIT_CMD_WREN_END: {
				if(millis() >= operationTimeout) {
					operationState = FINISH;
				}
				break;
			}
			case WRITE: {
				if(millis() >= operationTimeout) {
					operationState = FINISH;
				}
				else {
					if(HAL_SPI_TransmitReceive_DMA(_pHandler, currentOperation.pData_tx,//WRITE
						currentOperation.pData_rx, currentOperation.Size) == HAL_OK) {
						operationState = WAIT_WRITE_END;
					}
				}
				break;
			}

			case WAIT_WRITE_END: {
				if(millis() >= operationTimeout) {
					operationState = FINISH;
				}else{
					uint8_t wrcmd[1]={CMD_WRDI};
					if(HAL_SPI_TransmitReceive_DMA(_pHandler, wrcmd,
						currentOperation.pData_rx, 1) == HAL_OK) {
						operationState = WAIT_CMD_WRDI_END;
					}
				}
				break;
			}

			case WAIT_CMD_WRDI_END: {
				if(millis() >= operationTimeout) {
					operationState = FINISH;
				}else {
					operationState = FINISH;
				}
				break;
			}


			case WAIT_READ_END: {
				if(millis() >= operationTimeout) {
					operationState = FINISH;
				}
				break;
			}

			case READ_END: {
				currentOperation.pData_rx+=(currentOperation.MemAddSize+1);
				operationState = FINISH_READ_MEM;
				break;
			}

			case FINISH_READ_MEM: {
				if(currentOperation.callback_f != nullptr){
				   currentOperation.callback_f(currentOperation.pData_rx,
				   currentOperation.Size-=(currentOperation.MemAddSize+1));
				}   
				operationState = CLEAR;
				break;
			}*/

			case WAITING: {
				if(millis() >= operationTimeout) {
					HAL_GPIO_WritePin(currentOperation.GPIOx, currentOperation.GPIO_Pin, GPIO_PIN_SET);
					operationState = FINISH;
				}
				break;
			}
			case FINISH: {
				if(currentOperation.callback_f != nullptr){
				   currentOperation.callback_f(currentOperation.pData_tx,
				   currentOperation.Size);
				}   
				operationState = CLEAR;
				break;
			}
			case CLEAR: {
				if(currentOperation.free) free(currentOperation.pData_tx);
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
	operationState = FINISH;
	//if(operationState == WAITING) {
		//operationState = FINISH;
	//}/* else if(operationState == WAIT_CMD_WREN_END) {
	/*	operationState = WRITE;
	} else if(operationState == WAIT_WRITE_END) {
		operationState = CMD_WRDI;
	} else if(operationState == WAIT_CMD_WRDI_END) {
		operationState = FINISH;
	} else {
		operationState = FINISH;
	}*/
}
void SPI::rxInterrupt() {
	operationState = FINISH;
	//if(operationState == WAITING) {
	//	operationState = FINISH;
	//}/*else if(operationState == WAIT_READ_END) {
	/*	operationState = READ_END;
	} else{
		operationState = FINISH;
	}*/
}

void SPI::errorInterrupt() {
	if (HAL_SPI_GetError(_pHandler) > HAL_SPI_ERROR_NONE) {
		//HAL_GPIO_WritePin(currentOperation.GPIOx, currentOperation.GPIO_Pin, GPIO_PIN_SET);
    	operationState = FINISH;
	}
}

void SPI::receive(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint8_t *pData, uint16_t Size, dataCallback_f callbackFn) {
    operation operation;
	operation.operationType = EoperationType::RECEIVE;
    operation.GPIOx = GPIOx;
    operation.GPIO_Pin = GPIO_Pin;
	operation.pData_tx = wrBuf;
	operation.pData_rx = pData;
	operation.Size = Size;
	operation.callback_f = callbackFn;
	operation.free = false;
	operations.push(operation);
}

void SPI::transmit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint8_t *pData, uint16_t Size, dataCallback_f callbackFn) {
	operation operation;
	operation.operationType = EoperationType::TRANSMIT;

    operation.pData_tx = pData;
	operation.pData_rx = rdBuf;

	operation.GPIOx = GPIOx;
    operation.GPIO_Pin = GPIO_Pin;

	operation.pData_tx = (uint8_t*) malloc(Size);
	memcpy(operation.pData_tx, pData, Size);
	operation.Size = Size;
	operation.callback_f = callbackFn;
	operations.push(operation);
}

void SPI::readFromMemory(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin,
uint32_t MemAddress, uint16_t MemAddSize,
uint8_t *pData, uint16_t Size, dataCallback_f callbackFn) {

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
}

void SPI::writeToMemory(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin,
	uint32_t MemAddress, uint16_t MemAddSize,
	uint8_t *pData, uint16_t Size, dataCallback_f callbackFn) {

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
	operations.push(operation);
}

uint16_t SPI::queueSize() {
	return operations.size();
}