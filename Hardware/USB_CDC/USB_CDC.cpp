// #ifdef __USB_DEVICE__H__

#include "USB_CDC.h"
#include "USB_DEVICE/App/usbd_cdc_if.h"
extern USBD_HandleTypeDef hUsbDeviceFS;
struct {
	void(*fpOnTransmit)();
	void(*fpOnReceive)(uint8_t* data, uint32_t length);
} UART0_CONFIG;
static int8_t CDC_Receive_FS2(uint8_t* Buf, uint32_t *Len) {
  USBD_CDC_SetRxBuffer(&hUsbDeviceFS, &Buf[0]);
  USBD_CDC_ReceivePacket(&hUsbDeviceFS);
  UART0_CONFIG.fpOnReceive(Buf, *Len);
  return (USBD_OK);
}
void USB_CDC::onReceiveHandler(void(*onReceive)(uint8_t* data, uint32_t length)) {
	USBD_Interface_fops_FS.Receive = CDC_Receive_FS2;
	UART0_CONFIG.fpOnReceive = onReceive;
}
void USB_CDC::onTransmitHandler(void(*onTransmit)()) {
	UART0_CONFIG.fpOnTransmit = onTransmit;
}
bool USB_CDC::send(uint8_t* Buf, uint16_t Len) {
	return CDC_Transmit_FS(Buf, Len) == USBD_OK;
}
bool USB_CDC::send(String buf) {
	int length = strlen(buf);
	uint8_t converted[length];
	for (int i = 0; i < length; i++) {
		converted[i] = static_cast<uint8_t>(buf[i]);
	}
	return CDC_Transmit_FS(converted, length) == USBD_OK;
}

// #endif