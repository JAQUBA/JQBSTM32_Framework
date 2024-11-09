#include "OneWire.h"
#include "Core.h"
#include "main.h"

static OneWire *_OneWire_instance;

  
    #define    OW_TIMER_PROGRESS_END                0

      #define  OW_TIMER_PROGRESS_RESET  10
      #define  OW_TIMER_PROGRESS_RESET_WAIT_END_LOW   11
      #define  OW_TIMER_PROGRESS_RESET_WAIT_END_HIGH   12
     #define   OW_TIMER_PROGRESS_RESET_WAIT_READ_STATUS 13

     #define   OW_TIMER_PROCESS_WRITE   20
    #define   OW_TIMER_PROCESS_WRITE_WAIT_END_HIGH 21
    #define    OW_TIMER_PROCESS_WRITE_WAIT_END_LOW 22
    #define    OW_TIMER_PROCESS_WRITE_WAIT_STATUS 23

    #define    OW_TIMER_PROGRESS_READ  30
     #define   OW_TIMER_PROGRESS_READ_WAIT 31
     #define   OW_TIMER_PROGRESS_READ_GET 32
     #define   OW_TIMER_PROGRESS_READ_END 33
  

    #define OW_TIM_ELAPSED	ow_tim_delay==0

    static uint8_t  ow_tim_bit_index;
    static uint8_t  ow_tim_bit;
    static uint8_t  ow_tim_delay;
    static uint8_t  ow_tim_ready;
    static uint8_t  ow_tim_progress;

 
    #define        OW_PROGRESS_END 0
    #define        OW_PROGRESS_RESET 10
     #define       OW_PROGRESS_RESET_WAIT_TIMER_END 11
    #define        OW_PROGRESS_WRITE 20
    #define        OW_PROGRESS_WRITE_NEXT_BIT 21
    #define        OW_PROGRESS_WRITE_WAIT_TIMER_END  22
    #define        OW_PROGRESS_READ  30
    #define        OW_PROGRESS_READ_NEXT_BIT 31
    #define        OW_PROGRESS_READ_WAIT_TIMER_END 32
    


    static uint8_t  ow_progress;
    //static uint8_t  ow_byte_size;
    static uint8_t  ow_byte_index;
    static uint8_t  ow_byte;

    bool     ow_presents;


void OneWire::TimInterrupt() {
    switch (ow_tim_progress)
    {
        case OW_TIMER_PROGRESS_END:
        break;

        case OW_TIMER_PROGRESS_RESET://<<<<<<<<<<<<<<<<<<<<<<<
            HAL_GPIO_WritePin(DS_GPIO_Port, DS_Pin, GPIO_PIN_RESET);
            ow_tim_delay = 48;
            ow_tim_progress = OW_TIMER_PROGRESS_RESET_WAIT_END_LOW;
        break;

        case OW_TIMER_PROGRESS_RESET_WAIT_END_LOW:
            if (OW_TIM_ELAPSED)
            {
                HAL_GPIO_WritePin(DS_GPIO_Port, DS_Pin, GPIO_PIN_SET);
                ow_tim_delay=9;
                ow_tim_progress=OW_TIMER_PROGRESS_RESET_WAIT_END_HIGH;
            }
        break;

        case OW_TIMER_PROGRESS_RESET_WAIT_END_HIGH:
            if (OW_TIM_ELAPSED)
            {
                ow_presents = HAL_GPIO_ReadPin(DS_GPIO_Port, DS_Pin);
                ow_tim_delay=40;
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
           // ow_tim_delay=1;
            ow_tim_progress=OW_TIMER_PROGRESS_READ_WAIT;
        break;

        case OW_TIMER_PROGRESS_READ_WAIT:
            //if (OW_TIM_ELAPSED)
           // {
                HAL_GPIO_WritePin(DS_GPIO_Port, DS_Pin, GPIO_PIN_SET);
                ow_tim_delay=1;
                ow_tim_progress=OW_TIMER_PROGRESS_READ_GET;
          //  }
        break;

        case OW_TIMER_PROGRESS_READ_GET:
           // if (OW_TIM_ELAPSED)
           // {
                ow_tim_bit = HAL_GPIO_ReadPin(DS_GPIO_Port, DS_Pin);
                ow_tim_delay=1;
                ow_tim_progress=OW_TIMER_PROGRESS_READ_END;
          //  }
        break;

        case OW_TIMER_PROGRESS_READ_END:
            if (OW_TIM_ELAPSED)
            {
                ow_tim_ready=TRUE;
                ow_tim_progress = OW_TIMER_PROGRESS_END;
            }
        break;
    }
    if (ow_tim_delay>0) {ow_tim_delay--;}
}

OneWire *OneWire::getInstance(TIM_HandleTypeDef *pHandler) { return _OneWire_instance; }

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {OneWire::getInstance(htim)->TimInterrupt();}

OneWire::OneWire(Timer* timer, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin) {
    this->timer = timer;
    this->GPIOx = GPIOx;
    this->GPIO_Pin = GPIO_Pin;

    timer->attachInterrupt(voidCallback {

    });

    addTaskMain(taskCallback {
        switch (ow_progress)
        {
            case OW_PROGRESS_END:

            break;

            case OW_PROGRESS_RESET://<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
                ow_tim_ready=FALSE;
                ow_tim_progress = OW_TIMER_PROGRESS_RESET;//--->T
                ow_progress=OW_PROGRESS_RESET_WAIT_TIMER_END;
            break;

            case OW_PROGRESS_RESET_WAIT_TIMER_END:
              
                if (ow_tim_ready)// <---T
                {
                   	ow_tim_ready=FALSE;
                    ow_progress=OW_PROGRESS_END;//>>>
                }
            break;

            case OW_PROGRESS_WRITE://<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
                ow_byte_index = 0;
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
                    ow_byte_index++;
                    if (ow_byte_index<currentOperation.Size) {
                        ow_byte = (*(currentOperation.pData + ow_byte_index));
                        ow_tim_bit_index = 0;
                    }
                    else {
                        ow_progress = OW_PROGRESS_END;//>>>
                    }
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
                ow_byte_index = 0;
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
                    *(currentOperation.pData + ow_byte_index) = ow_byte;
                    ow_byte_index++;
                    if (ow_byte_index<currentOperation.Size) {
                        ow_byte = 0;
                        ow_tim_bit_index = 0;
                    }
                    else {
                        ow_progress = OW_PROGRESS_END;//>>>
                    }
                   // ow_progress = OW_PROGRESS_END;//>>>
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

        switch(operationState) {
			case IDLE:
				if(!operations.empty()) {
					currentOperation = operations.front();
					operationState = CHECK_FREE;
                }
			break;
			
			case CHECK_FREE:
				if(!ow_progress) {//gotowosc 1-wire
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
				else if(currentOperation.operationType == EoperationType::RESET)
                {
				    ow_progress = OW_PROGRESS_RESET;
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
    }, 0);
}

void OneWire::work() {
    
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
    //ow_byte_index = 0;
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
    //ow_byte_index = 0;
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
    //ow_byte_index = 0;
    operations.push(operation);
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
    //ow_byte_index = 0;
    operation.free = true;
    operations.push(operation);//transmit

    if (rxSize>0) receive(pData_rx, rxSize, callbackFn);

    if (res) reset();
  
}                

uint16_t OneWire::queueSize() {
    return operations.size();
}