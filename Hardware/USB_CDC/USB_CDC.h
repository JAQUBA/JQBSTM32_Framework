#ifndef __USB_CDC_H_
#define __USB_CDC_H_

#ifdef __USB_DEVICE__H__

#include "../../Core.h"

class USB_CDC {
    public:
        bool send(uint8_t* Buf, uint16_t Len);
        bool send(String buf);

        void onReceiveHandler(void(*onReceive)(uint8_t* data, uint32_t length));
        void onTransmitHandler(void(*onTransmit)());

    private:

};

#endif
#endif