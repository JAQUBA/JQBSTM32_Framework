#include "USB_CDC.h"

#include "USB_DEVICE/App/usbd_cdc_if.h"

struct {
	void (*recv_function)(String data);
} UART0_CONFIG;
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
void USB_CDC::onData(void (*_CallBack)(String data)) {
	UART0_CONFIG.recv_function = _CallBack;
}