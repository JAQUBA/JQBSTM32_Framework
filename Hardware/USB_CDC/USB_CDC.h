#ifndef __USB_CDC_H_
#define __USB_CDC_H_

#include "../../Core.h"

class USB_CDC {
    public:
        USB_CDC();
        bool send(uint8_t* Buf, uint16_t Len);
        bool send(const char *buf);

        void onReceiveHandler(std::function<void(uint8_t* data, uint16_t length)> onReceive);
        void onTransmitHandler(std::function<void()> onTransmit);

    private:
};
#endif