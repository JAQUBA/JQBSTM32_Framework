#include "OneWire.h"

OneWire::OneWire(Timer* timer, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin) {
    this->timer = timer;
    this->GPIOx = GPIOx;
    this->GPIO_Pin = GPIO_Pin;

    timer->attachInterrupt(voidCallback {

        /*
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
        }*/

        switch (ow_tim_progress)
        {
            case OW_TIMER_PROGRESS_RESET://<<<<<<<<<<<<<<<<<<<<<<<
                HAL_GPIO_WritePin(DS_GPIO_Port, DS_Pin, GPIO_PIN_RESET);
                ow_tim_delay = 50;//500uS
                ow_tim_progress = OW_TIMER_PROGRESS_RESET_WAIT_END_LOW;
            break;

            case OW_TIMER_PROGRESS_RESET_WAIT_END_LOW:
                if (OW_TIM_ELAPSED)
                {
                    HAL_GPIO_WritePin(DS_GPIO_Port, DS_Pin, GPIO_PIN_SET);
                    ow_tim_delay=12;
                    ow_tim_progress=OW_TIMER_PROGRESS_RESET_WAIT_END_HIGH;
                }
            break;

            case OW_TIMER_PROGRESS_RESET_WAIT_END_HIGH:
                if (OW_TIM_ELAPSED)
                {
                    ow_tim_bit = HAL_GPIO_ReadPin(DS_GPIO_Port, DS_Pin);
                    ow_tim_delay=48;
                    ow_tim_progress=OW_TIMER_PROGRESS_RESET_WAIT_READ_STATUS;
                }
            break;

            case OW_TIMER_PROGRESS_RESET_WAIT_READ_STATUS:
                if (OW_TIM_ELAPSED)
                    {
                        ow_tim_ready=TRUE;
                        ow_tim_progress = OW_TIMER_PROGRESS_END;
                    }
            break;

            case OW_TIMER_PROCESS_WRITE://<<<<<<<<<<<<<<<<<<<<<<
                HAL_GPIO_WritePin(DS_GPIO_Port, DS_Pin, GPIO_PIN_RESET);
                if (ow_tim_bit>0)
                {
                    ow_tim_delay=1;
                    ow_tim_progress=OW_TIMER_PROCESS_WRITE_WAIT_END_HIGH;
                }
                else
                {
                    ow_tim_delay=8;
                    ow_tim_progress=OW_TIMER_PROCESS_WRITE_WAIT_END_LOW;
                }
            break;

            case OW_TIMER_PROCESS_WRITE_WAIT_END_HIGH:
                if (OW_TIM_ELAPSED)
                {
                    HAL_GPIO_WritePin(DS_GPIO_Port, DS_Pin, GPIO_PIN_SET);
                    ow_tim_delay=8;
                    ow_tim_progress=OW_TIMER_PROCESS_WRITE_WAIT_STATUS;
                }
            break;

            case OW_TIMER_PROCESS_WRITE_WAIT_END_LOW:
                if (OW_TIM_ELAPSED)
                {
                    HAL_GPIO_WritePin(DS_GPIO_Port, DS_Pin, GPIO_PIN_SET);
                    ow_tim_delay=1;
                    ow_tim_progress=OW_TIMER_PROCESS_WRITE_WAIT_STATUS;
                }
            break;

            case OW_TIMER_PROCESS_WRITE_WAIT_STATUS:
                if (OW_TIM_ELAPSED)
                {
                    ow_tim_ready=TRUE;
                    ow_tim_progress = OW_TIMER_PROGRESS_END;
                }
            break;

            case OW_TIMER_PROGRESS_READ://<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
                ow_tim_bit = 0;
                HAL_GPIO_WritePin(DS_GPIO_Port, DS_Pin, GPIO_PIN_RESET);
                ow_tim_delay=1;
                ow_tim_progress=OW_TIMER_PROGRESS_READ_WAIT;
            break;

            case OW_TIMER_PROGRESS_READ_WAIT:
                if (OW_TIM_ELAPSED)
                {
                    HAL_GPIO_WritePin(DS_GPIO_Port, DS_Pin, GPIO_PIN_SET);
                    ow_tim_delay=1;
                    ow_tim_progress=OW_TIMER_PROGRESS_READ_GET;
                }
            break;

            case OW_TIMER_PROGRESS_READ_GET:
                if (OW_TIM_ELAPSED)
                {
                    ow_tim_bit = HAL_GPIO_ReadPin(DS_GPIO_Port, DS_Pin);
                    ow_tim_delay=4;
                    ow_tim_progress=OW_TIMER_PROGRESS_READ_END;
                }
            break;

            case OW_TIMER_PROGRESS_READ_END:
                if (OW_TIM_ELAPSED)
                {
                    ow_tim_ready=TRUE;
                    ow_tim_progress = OW_TIMER_PROGRESS_END;
                }
            break;
        }
        if (ow_tim_delay>0) ow_tim_delay--;
    });//end timer

    addTaskMain(taskCallback {
        switch (ow_progress)
        {
            case OW_PROGRESS_RESET://<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
                ow_tim_ready=FALSE;
                ow_tim_progress = OW_TIMER_PROGRESS_RESET;//--->T
                ow_progress=OW_PROGRESS_RESET_WAIT_TIMER_END;
            break;

            case OW_PROGRESS_RESET_WAIT_TIMER_END:

                if (ow_tim_ready)// <---T
                {	ow_tim_ready=FALSE;
                    ow_progress=OW_PROGRESS_END;//>>>
                }
            break;

            case OW_PROGRESS_WRITE://<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
                ow_tim_bit_index = 0;
                ow_progress=OW_PROGRESS_WRITE_NEXT_BIT;
            break;

            case OW_PROGRESS_WRITE_NEXT_BIT:
                if (ow_tim_bit_index<8)
                {
                    ow_tim_bit = ow_byte & 0x01;
                    ow_tim_ready = FALSE;
                    ow_tim_progress = OW_TIMER_PROCESS_WRITE;//--->T
                    ow_progress = OW_PROGRESS_WRITE_WAIT_TIMER_END;
                }
                else
                {
                    ow_progress = OW_PROGRESS_END;//>>>
                }
            break;

            case OW_PROGRESS_WRITE_WAIT_TIMER_END:

                if (ow_tim_ready)// <---T
                {
                    ow_byte >>= 1;
                    ow_tim_bit_index++;
                    ow_progress = OW_PROGRESS_WRITE_NEXT_BIT;
                }
            break;

            case OW_PROGRESS_READ://<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
                ow_byte = 0;
                ow_tim_bit_index = 0;
                ow_progress=OW_PROGRESS_READ_NEXT_BIT;
            break;

            case OW_PROGRESS_READ_NEXT_BIT:
                if (ow_tim_bit_index < 8)
                {
                    ow_byte >>= 1;
                    ow_tim_ready=FALSE;
                    ow_tim_progress = OW_TIMER_PROGRESS_READ;//--->T
                    ow_progress = OW_PROGRESS_READ_WAIT_TIMER_END;
                }
                else
                {
                    ow_progress = OW_PROGRESS_END;//>>>
                }
            break;

            case OW_PROGRESS_READ_WAIT_TIMER_END:

                if (ow_tim_ready)// <---T
                {
                    if (ow_tim_bit)	ow_byte |= 0x80;
                    ow_tim_bit_index ++;
                    ow_progress = OW_PROGRESS_READ_NEXT_BIT;
                }
            break;
        }
    }, 0);
}

//#define delay_us(x) delay(x)

void OneWire::work() {
    /*switch(currentOperation.operationType) {
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
    }*/
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
    bool adres,
    bool reset,
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