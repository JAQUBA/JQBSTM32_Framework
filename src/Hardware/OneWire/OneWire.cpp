#include "OneWire.h"

OneWire::OneWire(Timer* timer, GPIO_TypeDef* GPIO_Port, uint16_t GPIO_Pin) : OW_Timer(timer), OW_Port(GPIO_Port), OW_Pin(GPIO_Pin) {
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
				if(!operations.empty()) {
					currentOperation = operations.front();
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

void OneWire::reset() {
    operation operation;
    operation.operationType = EoperationType::RESET;
    operation.free = false;
    operations.push(operation);
}
void OneWire::transmit(
    const uint8_t* pData, uint16_t size,
    dataCallback_f callbackFn
){
    operation operation;
    operation.operationType = EoperationType::TRANSMIT;
    operation.pData = (uint8_t*) malloc(size);
    memcpy(operation.pData, pData, size);
    operation.Size = size;
    operation.free = true;
    operation.callback_f = callbackFn;
    operations.push(operation);
}

void OneWire::receive(
    uint8_t* pData, uint16_t size,
    dataCallback_f callbackFn
){
    operation operation;
    operation.operationType = EoperationType::RECEIVE;
    operation.pData = pData;
    operation.Size = size;
    operation.free = false;
    operation.callback_f = callbackFn;
    operations.push(operation);
}

void OneWire::transmitThenReceive(
    const uint8_t* pData_tx, uint16_t txSize,
    uint8_t* pData_rx, uint16_t rxSize,
    dataCallback_f callbackFn
){
    operation operation;
    operation.operationType = EoperationType::TRANSMIT;
    operation.pData = (uint8_t*) malloc(txSize);
    memcpy(operation.pData, pData_tx, txSize);
    operation.Size = txSize;
    operation.free = true;
    operations.push(operation);//transmit
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
    operation operation;

    reset();

    operation.operationType = EoperationType::TRANSMIT;
    if (address==NULL){
        Size = 2;
        if (txSize>0) Size += txSize;
        operation.pData = (uint8_t*) malloc(Size);
        *(operation.pData+0) = romCommand;
        *(operation.pData+1) = functionCommand;
        if (txSize>0) memcpy(operation.pData+Size, pData_tx, txSize);
    } else {
        Size=10;
        if (txSize>0) Size += txSize;
        operation.pData = (uint8_t*) malloc(Size);
        *(operation.pData+0) = romCommand;
        memcpy(operation.pData+1, address, 8);
        *(operation.pData+9) = functionCommand;
        if (txSize>0) memcpy(operation.pData+Size, pData_tx, txSize);
    }
    operation.Size = Size;
    operation.free = true;
    operations.push(operation);

    if (rxSize>0) receive(pData_rx, rxSize, callbackFn);
    if (resetAfterTransaction) reset();
}       

uint16_t OneWire::queueSize() {
    return operations.size();
}