#ifdef _JQB_USE_USB_CDC
#include "USB_CDC.h"
#include "USB_DEVICE/App/usbd_cdc_if.h"

extern USBD_HandleTypeDef hUsbDeviceFS;

USB_CDC::USB_CDC() {
	
}

struct {
	std::function<void()> fpOnTransmit;
    std::function<void(uint8_t* data, uint16_t length)> fpOnReceive;
} UART0_CONFIG;



static int8_t CDC_Receive_FS2(uint8_t* Buf, uint32_t *Len) {
  USBD_CDC_SetRxBuffer(&hUsbDeviceFS, &Buf[0]);
  USBD_CDC_ReceivePacket(&hUsbDeviceFS);
  UART0_CONFIG.fpOnReceive(Buf, *Len);
  return (USBD_OK);
}


void USB_CDC::onReceiveHandler(std::function<void(uint8_t* data, uint16_t length)> onReceive) {
	USBD_Interface_fops_FS.Receive = CDC_Receive_FS2;
	UART0_CONFIG.fpOnReceive = onReceive;
}
void USB_CDC::onTransmitHandler(std::function<void()> onTransmit) {
	UART0_CONFIG.fpOnTransmit = onTransmit;
}
bool USB_CDC::send(uint8_t* Buf, uint16_t Len) {
	return CDC_Transmit_FS(Buf, Len) == USBD_OK;
}
bool USB_CDC::send(const char *buf) {
	uint16_t length = strlen(buf);
	uint8_t Buf[length];
	for (int i = 0; i < length; i++) {
		Buf[i] = static_cast<uint8_t>(buf[i]);
	}
	return CDC_Transmit_FS(Buf, length) == USBD_OK;
}

#endif