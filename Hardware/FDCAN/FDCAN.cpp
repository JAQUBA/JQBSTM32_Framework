#ifdef _JQB_USE_FDCAN
#include "FDCAN.h"


FDCAN *_FDCAN_instances[FDCAN_MAX_INSTANCES];
uint8_t _FDCAN_instancesNum;

FDCAN::FDCAN(FDCAN_HandleTypeDef *pHandler) {
    _pHandler = pHandler;
    _FDCAN_instances[_FDCAN_instancesNum++] = this;
}
void FDCAN::init() {
    sFilterConfig.IdType = FDCAN_EXTENDED_ID;
	sFilterConfig.FilterType = FDCAN_FILTER_MASK;
	sFilterConfig.FilterID1 = 0;
	sFilterConfig.FilterID2 = 0;
	HAL_FDCAN_ConfigFilter(_pHandler, &sFilterConfig);
	HAL_FDCAN_ConfigRxFifoOverwrite(_pHandler, FDCAN_RX_FIFO0, FDCAN_RX_FIFO_OVERWRITE);
	HAL_FDCAN_ConfigRxFifoOverwrite(_pHandler, FDCAN_RX_FIFO1, FDCAN_RX_FIFO_OVERWRITE);
	HAL_FDCAN_Start(_pHandler);
	HAL_FDCAN_ActivateNotification(_pHandler, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);


    pTxHeader.IdType = FDCAN_EXTENDED_ID;
	pTxHeader.DataLength = FDCAN_DLC_BYTES_8;
	pTxHeader.TxFrameType = FDCAN_DATA_FRAME;
	pTxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
	pTxHeader.BitRateSwitch = FDCAN_BRS_OFF;
	pTxHeader.FDFormat = FDCAN_FD_CAN;
	pTxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
	pTxHeader.MessageMarker = 0;

}

void FDCAN::send(uint32_t identifier, uint8_t *data, uint16_t dataLen, uint32_t DataLength) {
    pTxHeader.Identifier = identifier;
    pTxHeader.DataLength = DataLength;
    HAL_FDCAN_AddMessageToTxFifoQ(_pHandler, &pTxHeader, data);
}

FDCAN *FDCAN::getInstance(FDCAN_HandleTypeDef *_instance) {
    for (size_t i = 0; i < _FDCAN_instancesNum; i++) {
        if(_FDCAN_instances[i]->_pHandler->Instance == _instance->Instance) return _FDCAN_instances[i];
    }
    return nullptr;
}
void FDCAN::handleMessage() {
    if(HAL_FDCAN_GetRxMessage(_pHandler, FDCAN_RX_FIFO0, &pRxHeader, recv_buff) != HAL_OK) return;
    hasPacket = true;
}
void FDCAN::onPacket(uint16_t commNumber, void(*handler)(uint8_t *data, uint16_t dataLen)) {
    struct handlerStruct *temp = handlers, *r;
    if(handlers==NULL) {
        temp = (struct handlerStruct *)malloc(sizeof(struct handlerStruct));
		temp->handler=handler;
		temp->commNumber=commNumber;
		temp->next=NULL;
		handlers=temp;
    } else {
        while(temp->next != NULL) temp = temp->next;
		r = (struct handlerStruct*)malloc(sizeof(struct handlerStruct));
		r->handler=handler;
		r->commNumber=commNumber;
		r->next=NULL;
		temp->next=r;
    }
}
void FDCAN::poll() {
    if(!hasPacket) return;
    struct handlerStruct *temp = handlers;
    uint32_t commNumber = pRxHeader.Identifier;
    while (temp != NULL) {
        if(temp->commNumber == commNumber) {
            temp->handler(recv_buff, sizeof(recv_buff));
            break;
        }
        temp = temp->next;
    }
    hasPacket = false;
}
extern "C" void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *pHandler, uint32_t RxFifo0ITs) {
    if((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != RESET) FDCAN::getInstance(pHandler)->handleMessage();
}
#endif