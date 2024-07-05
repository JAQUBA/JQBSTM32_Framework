#ifdef _JQB_USE_USB_CDC
#include "USB_CDC.h"

#include "USB_DEVICE/App/usbd_cdc_if.h"
extern USBD_HandleTypeDef hUsbDeviceFS;

USB_CDC *_USB_CDC_instance;
USB_CDC *USB_CDC::getInstance() {return _USB_CDC_instance;}
static int8_t CDC_Receive_FS2(uint8_t* Buf, uint32_t *Len) {
  USBD_CDC_SetRxBuffer(&hUsbDeviceFS, &Buf[0]);
  USBD_CDC_ReceivePacket(&hUsbDeviceFS);
  USB_CDC::getInstance()->rxInterrupt(Buf, Len);
  return USBD_OK;
}
static int8_t CDC_TransmitCplt_FS2(uint8_t *Buf, uint32_t *Len, uint8_t epnum) {
  USB_CDC::getInstance()->txInterrupt(Buf, Len, epnum);
  return USBD_OK;
}
USB_CDC::USB_CDC() {
	_USB_CDC_instance = this;

	USBD_Interface_fops_FS.Receive = CDC_Receive_FS2;
	USBD_Interface_fops_FS.TransmitCplt = CDC_TransmitCplt_FS2;

	addTaskMain(taskCallback {
		switch(operationState) {
			case IDLE: {
				if(!operations.empty()) {
					currentOperation = operations.front();
					operationState = WORK;
				}
				break;
			}
			case WORK: {
				operationTimeout = millis()+50;
				if(currentOperation.operationType == EoperationType::SEND) {
					uint8_t result = CDC_Transmit_FS(
						currentOperation.pData,
						currentOperation.Size
					);
					if(result == USBD_OK) {
						operationState = WAITING;
					} else if(result == USBD_BUSY) {
						operationState = WAITING;
					} else {
						operationState = CLEAR;
					}
					break;
				}
				else if(currentOperation.operationType == EoperationType::RECEIVE) {
					operationState = FINISH;
				}
			}
			case WAITING: {
				if(millis() >= operationTimeout) {
					operationState = CLEAR;
				}
				break;
			}
			case FINISH: {
				if(currentOperation.callback_f != nullptr) {
					currentOperation.callback_f(
						currentOperation.pData,
						currentOperation.Size
					);
				}
				operationState = CLEAR;
				break;
			}
			case CLEAR: {
				if(currentOperation.free) free(currentOperation.pData);
				operations.pop();
				operationState = IDLE;
				break;
			}
			default: {}
		}
	});
}

void USB_CDC::rxInterrupt(uint8_t* Buf, uint32_t *Len) {
	fpOnReceive(Buf, *Len);
}
void USB_CDC::txInterrupt(uint8_t *Buf, uint32_t *Len, uint8_t epnum) {
	if(operationState == WAITING) {
		operationState = FINISH;
	}
}

void USB_CDC::onReceiveHandler(dataCallback_f onReceive) {fpOnReceive = onReceive;}
void USB_CDC::onTransmitHandler(voidCallback_f onTransmit) {fpOnTransmit = onTransmit;}

void USB_CDC::send(uint8_t *pData, uint16_t Size, dataCallback_f callbackFn) {
	operation operation;
	operation.operationType = EoperationType::SEND;
	operation.pData = (uint8_t*) malloc(Size);
	memcpy(operation.pData, pData, Size);
	operation.Size = Size;
	operation.callback_f = callbackFn;
	operations.push(operation);
}

void USB_CDC::send(const char *buf) {
	uint16_t Len = strlen(buf);
	uint8_t Buf[Len];
	for (int i = 0; i < Len; i++) {
		Buf[i] = static_cast<uint8_t>(buf[i]);
	}
	send(Buf, Len);
}

uint16_t USB_CDC::queueSize() {
	return operations.size();
}

#endif