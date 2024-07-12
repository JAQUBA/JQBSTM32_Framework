#include "UART.h"

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

UART::UART(UART_HandleTypeDef *pHandler) {
    _pHandler = pHandler;
    _UART_instances[_UART_instancesNum++] = this;
    
    HAL_UART_Receive_IT(_pHandler, &Received_u1, 1);

    addTaskMain(taskCallback {
        if(received && millis() > lastReceivedByte + 20) {
            if(fpOnReceive) fpOnReceive(rx_buffer, rx_data_index);
            rx_data_index = 0;
            received = false;
        }
        switch(operationState) {
			case IDLE: {
				if(!operations.empty()) {
					currentOperation = operations.front();
					operationState = CHECK_FREE;
				}
				break;
			}
			case CHECK_FREE: {
				if(HAL_UART_GetState(_pHandler) == HAL_UART_STATE_READY) {
					operationState = WORK;
				}
				break;
			}
			case WORK: {
				operationTimeout = millis()+2;
				if(currentOperation.operationType == EoperationType::SEND) {
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
				if(currentOperation.callback_f != nullptr) currentOperation.callback_f(currentOperation.pData,
						currentOperation.Size);
				operationState = CLEAR;
				break;
			}
			case CLEAR: {
				if(currentOperation.free) free(currentOperation.pData);
				operations.pop();
				operationState = IDLE;
				break;
			}
			default: {}
		}


    }, 0);
}


void UART::rxInterrupt() {
    rx_buffer[rx_data_index] = Received_u1;
    rx_data_index++;
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

void UART::onReceiveHandler(std::function<void(uint8_t* data, uint16_t length)> onReceive) {fpOnReceive = onReceive;}
void UART::onTransmitHandler(std::function<void()> onTransmit) {fpOnTransmit = onTransmit;}


void UART::send(uint8_t *pData, uint16_t Size, dataCallback_f callbackFn) {
    operation operation;
	operation.operationType = EoperationType::SEND;
	operation.pData = (uint8_t*) malloc(Size);
	memcpy(operation.pData, pData, Size);
	operation.Size = Size;
	operation.callback_f = callbackFn;
	operations.push(operation);
}
