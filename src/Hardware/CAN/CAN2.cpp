#ifdef _JQB_USE_CAN2
#include "CAN2.h"

CAN *_CAN_instances[CAN_MAX_INSTANCES];
uint8_t _CAN_instancesNum;

CAN *CAN::getInstance(CAN_HandleTypeDef *_instance) {
    for (size_t i = 0; i < _CAN_instancesNum; i++) {
        if(_CAN_instances[i]->_pInstance->Instance == _instance->Instance) return _CAN_instances[i];
    }
    return nullptr;
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *pHandler)
{
    CAN::getInstance(pHandler)->rxInterrupt();
}

CAN::CAN(CAN_HandleTypeDef *pHandler) {
    _pInstance = pHandler;
    _CAN_instances[_CAN_instancesNum++] = this;

    sFilterConfig.FilterBank = 0;
    sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
    sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
    sFilterConfig.FilterIdHigh = 0;
    sFilterConfig.FilterIdLow = 0;
    sFilterConfig.FilterMaskIdHigh = 0;
    sFilterConfig.FilterMaskIdLow = 0;
    sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
    sFilterConfig.FilterActivation = ENABLE;
    sFilterConfig.SlaveStartFilterBank = 14;

    pTxHeader.IDE = CAN_ID_STD;
    pTxHeader.RTR = CAN_RTR_DATA;
    pTxHeader.StdId = 0x030;
    pTxHeader.ExtId = 0x02;
    pTxHeader.DLC = 8; // Number of bites to be transmitted max- 8
    pTxHeader.TransmitGlobalTime = DISABLE;
    
	HAL_CAN_ConfigFilter(_pInstance, &sFilterConfig);
	HAL_CAN_Start(_pInstance);
	HAL_CAN_ActivateNotification(_pInstance, CAN_IT_RX_FIFO0_MSG_PENDING);
       
    addTaskMain(taskCallback {
        if(!hasPacket) return;
        struct handlerStruct *temp = handlers;
        uint32_t commNumber = pRxHeader.IDE;
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
    if (HAL_CAN_GetRxMessage(_pInstance, CAN_RX_FIFO0, &pRxHeader, pData) != HAL_OK) return; 
    hasPacket = true;
}
void CAN::send(uint32_t identifier, uint8_t *pData,
            uint16_t Size, uint32_t DataLength) {
    pTxHeader.ExtId = identifier;
    pTxHeader.DLC = DataLength;
    HAL_CAN_AddTxMessage(_pInstance, &pTxHeader, pData, &canMailbox);
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