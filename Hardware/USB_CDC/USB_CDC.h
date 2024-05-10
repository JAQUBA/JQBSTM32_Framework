#ifndef __USB_CDC_H_
#define __USB_CDC_H_

#include "../../Core.h"

class USB_CDC {
    public:
        bool send(uint8_t* Buf, uint16_t Len);
        bool send(String buf);

        void onData(void (*_CallBack)(String data));
        void onData(void (*_CallBack)(uint8_t* Buf, uint16_t Len));

    private:

};

#endif