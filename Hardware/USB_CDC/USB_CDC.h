#ifndef __USB_CDC_H_
#define __USB_CDC_H_

#include "../../Core.h"

#include "USB_DEVICE/App/usb_device.h"
#include "USB_DEVICE/App/usbd_cdc_if.h"

class USB_CDC {
    public:
        void init();
        bool send(uint8_t* Buf, uint16_t Len);
        bool send(String buf);

        void onData(void (*_CallBack)(String data));
        void onData(void (*_CallBack)(uint8_t* Buf, uint16_t Len));

    private:

};

#endif