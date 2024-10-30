#include "../../Core.h"
#ifdef HAL_PCD_MODULE_ENABLED

#ifndef __USB_CDC_H_
#define __USB_CDC_H_

#include "Interface/IBus.h"

class USB_CDC : public IBus {
    public:
        USB_CDC();
        static USB_CDC *getInstance();

        void send(uint8_t *pData, uint16_t Size, dataCallback_f callbackFn = nullptr);
        void send(const char *buf);

        void onReceiveHandler(dataCallback_f onReceive);
        void onTransmitHandler(voidCallback_f onTransmit);

        void rxInterrupt(uint8_t *Buf, uint32_t *Len);
        void txInterrupt(uint8_t *Buf, uint32_t *Len, uint8_t epnum);

        uint16_t queueSize();

    private:
        dataCallback_f fpOnReceive;
        voidCallback_f fpOnTransmit;

        uint32_t operationTimeout;
        enum { IDLE, CHECK_FREE, WORK, WAITING, CLEAR, FINISH } operationState = IDLE;
        enum EoperationType {RECEIVE, SEND};

        struct operation {
            EoperationType operationType;
            uint8_t *pData;
            uint16_t Size;
            dataCallback_f callback_f;
            bool free = true;
        } currentOperation;
        
        std::queue<operation> operations;
};
#endif
#endif