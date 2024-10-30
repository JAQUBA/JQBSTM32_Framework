#include "../../Core.h"
#ifdef HAL_SPI_MODULE_ENABLED

#ifndef __SPI_H_
#define __SPI_H_

#ifndef SPI_MAX_INSTANCES
#define SPI_MAX_INSTANCES 2
#endif

#include "Interface/IBus.h"

class SPI : public IBus {
    public:
        SPI(SPI_HandleTypeDef *pHandler);
        static SPI *getInstance(SPI_HandleTypeDef *pHandler);

        void transmit(
            GPIO_TypeDef* CSPort, uint16_t CSPin,
            uint8_t *pData, uint16_t Size,
            dataCallback_f callbackFn = nullptr
        );
        void receive(
            GPIO_TypeDef* CSPort, uint16_t CSPin,
            uint8_t *pData, uint16_t Size,
            dataCallback_f callbackFn = nullptr
        );
        void transmitThenReceive(GPIO_TypeDef* CSPort, uint16_t CSPin,
            uint8_t *pData_tx, uint16_t txSize,
            uint8_t *pData_rx, uint16_t rxSize,
            dataCallback_f callbackFn = nullptr
        );
        void transmitReceive(
            GPIO_TypeDef* CSPort, uint16_t CSPin,
            uint8_t *pDataTx, uint8_t *pDataRx,
            uint16_t Size,
            dataCallback_f callbackFn = nullptr
        );

        void txInterrupt();
        void rxInterrupt();
        void errorInterrupt();

        uint16_t queueSize();
    private:
        SPI_HandleTypeDef* _pHandler;
        uint32_t operationTimeout;
        enum {
            IDLE,
            CHECK_FREE,
            WORK,
            WAITING,
            CLEAR,
            FINISH
        } operationState = IDLE;

        enum EoperationType {
            RECEIVE,
            TRANSMIT,
            TRANSMIT_RECEIVE
        };
        
        struct operation {
            EoperationType  operationType;
            
            GPIO_TypeDef*   _CSPort;
            uint16_t        _CSPin;
            bool            _pinReset = true;
            uint8_t         *pData;
            uint16_t        Size;
            bool            free = false;
            
            dataCallback_f  callback_f = nullptr;
        } currentOperation;
        
        std::queue<operation> operations;
};
#endif
#endif