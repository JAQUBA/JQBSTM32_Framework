#include "OneWire.h"

OneWire::OneWire(Timer* timer, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin) {
    this->timer = timer;
    this->GPIOx = GPIOx;
    this->GPIO_Pin = GPIO_Pin;

    timer->attachInterrupt(voidCallback {
        switch(operationState) {
            case IDLE: {
                if(!operations.empty()) {
                    currentOperation = operations.front();
                    operationState = CHECK_FREE;
                }
                break;
            }
            case CHECK_FREE: {
                if(OW_GET_PIN == GPIO_PIN_SET) {
                    operationState = WORK;
                } else {
                    operationState = WAITING;
                }
            }
            case WORK: {
                operationTimeout = millis()+4;
                work();
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
                        currentOperation.pData,
                        currentOperation.Size
                    );
                    operationState = CLEAR;
                    break;
                }
            }
            case CLEAR: {
                operations.pop();
                operationState = IDLE;
                break;
            }
        }
    });
}


#define delay_us(x) delay(x)

void OneWire::work() {
    switch(currentOperation.operationType) {
        case EoperationType::RESET: {
            switch(currentWork.state) {
                case OW_HIGH: {
                    OW_L;
                    delay_us(480);
                    currentWork.state = OW_LOW;
                    break;
                }
                case OW_LOW: {
                    OW_H;
                    delay_us(70);
                    currentWork.state = workState::OW_HIGH;
                    operationState = FINISH;
                    break;
                }
            }
            break;
        }
        case EoperationType::TRANSMIT: {
            switch(currentWork.state) {
                case OW_HIGH: {
                    OW_L;
                    delay_us(1);
                    currentWork.state = OW_LOW;
                    break;
                }
                case OW_LOW: {
                    OW_H;
                    delay_us(1);
                    currentWork.state = OW_HIGH;
                    operationState = FINISH;
                    break;
                }
            }
            break;
        }
        case EoperationType::RECEIVE: {
            switch(currentWork.state) {
                case OW_HIGH: {
                    OW_L;
                    delay_us(1);
                    currentWork.state = OW_LOW;
                    break;
                }
                case OW_LOW: {
                    OW_H;
                    delay_us(1);
                    currentWork.state = OW_HIGH;
                    operationState = FINISH;
                    break;
                }
            }
            break;
        }
    }
}

void OneWire::reset() {
    operation operation;
    operation.operationType = EoperationType::RESET;
    operations.push(operation);
}
void OneWire::transmit(
    uint8_t *pData, uint16_t Size,
    dataCallback_f callbackFn
){
    operation operation;
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
    dataCallback_f callbackFn
){
    operation operation;
    operation.operationType = EoperationType::TRANSMIT;
    operation.pData = (uint8_t*) malloc(txSize);
    memcpy(operation.pData, pData_tx, txSize);
    operation.Size = txSize;
    operation.free = true;
    operations.push(operation);
    receive(pData_rx, rxSize, callbackFn);
}
uint16_t OneWire::queueSize() {
    return operations.size();
}