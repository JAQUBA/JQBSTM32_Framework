#include "OneWire.h"

OneWire::OneWire(Timer* timer, GPIO_TypeDef* GPIO_Port, uint16_t GPIO_Pin) : OW_Timer(timer), OW_Port(GPIO_Port), OW_Pin(GPIO_Pin) {
    OW_Timer->attachInterrupt(Timer::PeriodElapsedCallback, voidCallback {
        switch(ow_tim_progress) {
            case OW_TIMER_PROGRESS_IDLE: {
                break;
            }
            case OW_TIMER_PROGRESS_RESET: {
                HAL_GPIO_WritePin(OW_Port, OW_Pin, GPIO_PIN_RESET);
                OW_Timer->setPeriod(490);
                ow_tim_progress = OW_TIMER_PROGRESS_RESET_WAIT_END_LOW;
                break;
            }
            case OW_TIMER_PROGRESS_RESET_WAIT_END_LOW: {
                HAL_GPIO_WritePin(OW_Port, OW_Pin, GPIO_PIN_SET);
                OW_Timer->setPeriod(90);
                ow_tim_progress=OW_TIMER_PROGRESS_RESET_WAIT_END_HIGH;
                break;
            }
            case OW_TIMER_PROGRESS_RESET_WAIT_END_HIGH: {
                ow_presents = HAL_GPIO_ReadPin(OW_Port, OW_Pin);
                OW_Timer->setPeriod(400);
                ow_tim_progress=OW_TIMER_PROGRESS_IDLE;
                break;
            }
            case OW_TIMER_PROGRESS_WRITE: {
                HAL_GPIO_WritePin(OW_Port, OW_Pin, GPIO_PIN_RESET);
                if (ow_tim_bit>0) {
                    OW_Timer->setPeriod(10);
                    ow_tim_progress=OW_TIMER_PROGRESS_WRITE_WAIT_END_HIGH;
                } else {
                    OW_Timer->setPeriod(30);
                    ow_tim_progress=OW_TIMER_PROGRESS_WRITE_WAIT_END_LOW;
                }
                break;
            }
            case OW_TIMER_PROGRESS_WRITE_WAIT_END_HIGH: {
                HAL_GPIO_WritePin(OW_Port, OW_Pin, GPIO_PIN_SET);
                OW_Timer->setPeriod(30);
                ow_tim_progress=OW_TIMER_PROGRESS_IDLE;
                break;
            }
            case OW_TIMER_PROGRESS_WRITE_WAIT_END_LOW: {
                HAL_GPIO_WritePin(OW_Port, OW_Pin, GPIO_PIN_SET);
                OW_Timer->setPeriod(10);
                ow_tim_progress=OW_TIMER_PROGRESS_IDLE;
                break;
            }
            case OW_TIMER_PROGRESS_READ: {
                ow_tim_bit = 0;
                HAL_GPIO_WritePin(OW_Port, OW_Pin, GPIO_PIN_RESET);
                OW_Timer->setPeriod(10);
                ow_tim_progress=OW_TIMER_PROGRESS_READ_WAIT;
                break;
            }
            case OW_TIMER_PROGRESS_READ_WAIT: {
                HAL_GPIO_WritePin(OW_Port, OW_Pin, GPIO_PIN_SET);
                OW_Timer->setPeriod(10);
                ow_tim_progress=OW_TIMER_PROGRESS_READ_GET;
                break;
            }
            case OW_TIMER_PROGRESS_READ_GET: {
                ow_tim_bit = HAL_GPIO_ReadPin(OW_Port, OW_Pin);
                OW_Timer->setPeriod(10);
                ow_tim_progress=OW_TIMER_PROGRESS_IDLE;
                break;
            }
        }
    });

    addTaskMain(taskCallback {
        switch(ow_progress) {
            case OW_PROGRESS_IDLE: {
                break;
            }
            case OW_PROGRESS_WRITE: {
                ow_byte_index = 0;
                ow_tim_bit_index = 0;
                ow_progress=OW_PROGRESS_WRITE_NEXT_BIT;
                break;
            }
            case OW_PROGRESS_WRITE_NEXT_BIT: {
                if (ow_tim_bit_index<8) {
                    ow_tim_bit = ow_byte & 0x01;
                    ow_tim_progress = OW_TIMER_PROGRESS_WRITE;
                    ow_progress = OW_PROGRESS_WRITE_WAIT_TIMER_END;
                } else {
                    ow_byte_index++;
                    if (ow_byte_index<currentOperation.Size) {
                        ow_byte = (*(currentOperation.pData + ow_byte_index));
                        ow_tim_bit_index = 0;
                    } else {
                        ow_progress = OW_PROGRESS_IDLE;
                    }
                }
                break;
            }
            case OW_PROGRESS_WRITE_WAIT_TIMER_END: {
                if (ow_tim_progress==OW_TIMER_PROGRESS_IDLE) {
                    ow_byte >>= 1;
                    ow_tim_bit_index++;
                    ow_progress = OW_PROGRESS_WRITE_NEXT_BIT;
                }
                break;
            }
            case OW_PROGRESS_READ: {
                ow_byte = 0;
                ow_byte_index = 0;
                ow_tim_bit_index = 0;
                ow_progress=OW_PROGRESS_READ_NEXT_BIT;
                break;
            }
            case OW_PROGRESS_READ_NEXT_BIT: {
                if (ow_tim_bit_index < 8) {
                    ow_byte >>= 1;
                    ow_tim_progress = OW_TIMER_PROGRESS_READ;
                    ow_progress = OW_PROGRESS_READ_WAIT_TIMER_END;
                } else {
                    *(currentOperation.pData + ow_byte_index) = ow_byte;
                    ow_byte_index++;
                    if (ow_byte_index<currentOperation.Size) {
                        ow_byte = 0;
                        ow_tim_bit_index = 0;
                    } else {
                        ow_progress = OW_PROGRESS_IDLE;
                    }
                }
                break;
            }
            case OW_PROGRESS_READ_WAIT_TIMER_END: {
                if (ow_tim_progress==OW_TIMER_PROGRESS_IDLE) {
                    if (ow_tim_bit)	ow_byte |= 0x80;
                    ow_tim_bit_index ++;
                    ow_progress = OW_PROGRESS_READ_NEXT_BIT;
                }
                break;
            }
        }

        switch(operationState) {
			case IDLE:
				if(!operations.empty()) {
					currentOperation = operations.front();
					operationState = CHECK_FREE;
                }
			break;
			
			case CHECK_FREE:
				if(ow_tim_progress == OW_TIMER_PROGRESS_IDLE) {
					operationState = WORK;
                }
            break;
			
			case WORK:
				operationTimeout = millis()+40;
                          
				if(currentOperation.operationType == EoperationType::TRANSMIT) {
                    ow_byte = (*currentOperation.pData);
					ow_progress = OW_PROGRESS_WRITE;
				}
				else if(currentOperation.operationType == EoperationType::RECEIVE) {
                    ow_byte = 0;
					ow_progress = OW_PROGRESS_READ;
				}
				else if(currentOperation.operationType == EoperationType::RESET) {
                    ow_tim_progress = OW_TIMER_PROGRESS_RESET;
                    ow_presents = false;
				}
                operationState = WAITING;
			break;
			
			case WAITING:
       			if ((ow_progress==0) || (millis() >= operationTimeout)) {
					operationState = FINISH;
                }
            break;
			

			case FINISH: 
            	if(currentOperation.callback_f != nullptr) {
					currentOperation.callback_f(
						currentOperation.pData,
						currentOperation.Size
					);
                }
				operationState = CLEAR;
    		break;
				
			case CLEAR:
				if(currentOperation.free) free(currentOperation.pData);
				operations.pop();
				operationState = IDLE;
            break;
            
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
    uint8_t *pData, uint16_t Size,
    dataCallback_f callbackFn
){
    operation operation;
    reset();
    operation.operationType = EoperationType::TRANSMIT;
    operation.pData = (uint8_t*) malloc(Size);
    memcpy(operation.pData, pData, Size);
    operation.Size = Size;
    operation.free = true;
    operation.callback_f = callbackFn;
    operations.push(operation);
}

void OneWire::receive(
    uint8_t *pData, uint16_t Size,
    dataCallback_f callbackFn
){
    operation operation;
    operation.operationType = EoperationType::RECEIVE;
    operation.pData = pData;
    operation.Size = Size;
    operation.free = false;
    operation.callback_f = callbackFn;
    operations.push(operation);
}

void OneWire::transmitThenReceive(
    uint8_t *pData_tx, uint16_t txSize,
    uint8_t *pData_rx, uint16_t rxSize,
    bool res,
    dataCallback_f callbackFn
){
    
    operation operation;

    reset();
    operation.operationType = EoperationType::TRANSMIT;
    operation.pData = (uint8_t*) malloc(txSize);
    memcpy(operation.pData, pData_tx, txSize);
    operation.Size = txSize;
    operation.free = true;
    operations.push(operation);//transmit
    receive(pData_rx, rxSize, callbackFn);
}

void OneWire::sesja(
    uint8_t ROMcomm,
    uint8_t *adres,
    uint8_t FUNcomm,
    uint8_t *pData_tx, uint8_t txSize,
    uint8_t *pData_rx, uint8_t rxSize,
    bool     res,
    dataCallback_f callbackFn
){
    uint8_t Size;
    operation operation;

    reset();

    operation.operationType = EoperationType::TRANSMIT;
    if (adres==NULL){
        Size = 2;
        if (txSize>0) Size += txSize;
        operation.pData = (uint8_t*) malloc(Size);
        *(operation.pData+0) = ROMcomm;
        *(operation.pData+1) = FUNcomm;
        if (txSize>0) memcpy(operation.pData+Size, pData_tx, txSize);
    } else {
        Size=10;
        if (txSize>0) Size += txSize;
        operation.pData = (uint8_t*) malloc(Size);
        *(operation.pData+0) = ROMcomm;
        memcpy(operation.pData+1, adres, 8);
        *(operation.pData+9) = FUNcomm;
        if (txSize>0) memcpy(operation.pData+Size, pData_tx, txSize);
    }
    operation.Size = Size;
    operation.free = true;
    operations.push(operation);//transmit

    if (rxSize>0) receive(pData_rx, rxSize, callbackFn);
    if (res) reset();
}       

uint16_t OneWire::queueSize() {
    return operations.size();
}