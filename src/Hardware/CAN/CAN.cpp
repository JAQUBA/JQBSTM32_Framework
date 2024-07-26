#ifdef _JQB_USE_CAN
#include "CAN.h"
#include "can.h"

CAN *_CAN_instances[CAN_MAX_INSTANCES];
uint8_t _CAN_instancesNum;
CAN *CAN::getInstance(CAN_HandleTypeDef *_instance) {
    for (size_t i = 0; i < _CAN_instancesNum; i++) {
        if(_CAN_instances[i]->_pInstance->Instance == _instance->Instance) return _CAN_instances[i];
    }
    return nullptr;
}
void HAL_CAN_RxFifo0Callback(CAN_HandleTypeDef *pHandler, uint32_t RxFifo0ITs) {
    if((RxFifo0ITs & CAN_IT_RX_FIFO0_NEW_MESSAGE) != RESET) CAN::getInstance(pHandler)->rxInterrupt();
}

CAN::CAN(CAN_HandleTypeDef *pHandler) {
    _pInstance = pHandler;
    _CAN_instances[_CAN_instancesNum++] = this;

    sFilterConfig.IdType = CAN_EXTENDED_ID;
	sFilterConfig.FilterType = CAN_FILTER_MASK;
	sFilterConfig.FilterID1 = 0;
	sFilterConfig.FilterID2 = 0;
	HAL_CAN_ConfigFilter(_pInstance, &sFilterConfig);
	HAL_CAN_ConfigRxFifoOverwrite(_pInstance, CAN_RX_FIFO0, CAN_RX_FIFO_OVERWRITE);
	HAL_CAN_ConfigRxFifoOverwrite(_pInstance, CAN_RX_FIFO1, CAN_RX_FIFO_OVERWRITE);
	HAL_CAN_Start(_pInstance);
	HAL_CAN_ActivateNotification(_pInstance, CAN_IT_RX_FIFO0_NEW_MESSAGE, 0);

    pTxHeader.IdType = CAN_EXTENDED_ID;
	pTxHeader.DataLength = CAN_DLC_BYTES_8;
	pTxHeader.TxFrameType = CAN_DATA_FRAME;
	pTxHeader.ErrorStateIndicator = CAN_ESI_ACTIVE;
	pTxHeader.BitRateSwitch = CAN_BRS_OFF;
	pTxHeader.FDFormat = CAN_FD_CAN;
	pTxHeader.TxEventFifoControl = CAN_NO_TX_EVENTS;
	pTxHeader.MessageMarker = 0;

    addTaskMain(taskCallback {
        if(!hasPacket) return;
        struct handlerStruct *temp = handlers;
        uint32_t commNumber = pRxHeader.Identifier;
        while (temp != NULL) {
            if(temp->commNumber == commNumber) {
                temp->handler(pData, sizeof(pData));
                break;
            }
            temp = temp->next;
        }
        hasPacket = false;
    });
}
void CAN::rxInterrupt() {
    if(HAL_CAN_GetRxMessage(_pInstance, CAN_RX_FIFO0, &pRxHeader, pData) != HAL_OK) return;
    hasPacket = true;
}
void CAN::send(uint32_t identifier, uint8_t *pData, uint16_t Size, uint32_t DataLength) {
    pTxHeader.Identifier = identifier;
    pTxHeader.DataLength = DataLength;
    HAL_CAN_AddMessageToTxFifoQ(_pInstance, &pTxHeader, pData);
}

void CAN::onPacket(uint16_t commNumber, dataCallback_f handler) {
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
#endif