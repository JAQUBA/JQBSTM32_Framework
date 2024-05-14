#include "FDCAN.h"

FDCAN *_FDCAN_instances[FDCAN_MAX_INSTANCES];
uint8_t _FDCAN_instancesNum;

FDCAN::FDCAN(FDCAN_HandleTypeDef *instance) {
    _pInstance = instance;
    _FDCAN_instances[_FDCAN_instancesNum++] = this;
}

void FDCAN::init() {
    FDCAN_FilterTypeDef sFilterConfig;
    sFilterConfig.IdType = FDCAN_EXTENDED_ID;
	sFilterConfig.FilterType = FDCAN_FILTER_MASK;
	sFilterConfig.FilterID1 = 0;
	sFilterConfig.FilterID2 = 0;
	HAL_FDCAN_ConfigFilter(_pInstance, &sFilterConfig);
	HAL_FDCAN_ConfigRxFifoOverwrite(_pInstance, FDCAN_RX_FIFO0, FDCAN_RX_FIFO_OVERWRITE);
	HAL_FDCAN_ConfigRxFifoOverwrite(_pInstance, FDCAN_RX_FIFO1, FDCAN_RX_FIFO_OVERWRITE);
	HAL_FDCAN_Start(_pInstance);
	HAL_FDCAN_ActivateNotification(_pInstance, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
	HAL_FDCAN_ConfigFilter(_pInstance, &sFilterConfig);
}

FDCAN *FDCAN::getInstance(FDCAN_HandleTypeDef *_instance) {
    for (size_t i = 0; i < _FDCAN_instancesNum; i++) {
        if(_FDCAN_instances[i]->_pInstance->Instance == _instance->Instance) return _FDCAN_instances[i];
    }
    return nullptr;
}
void FDCAN::handleMessage() {
	FDCAN_RxHeaderTypeDef RxHeader;
    uint8_t RxData[8];
    if(HAL_FDCAN_GetRxMessage(_pInstance, FDCAN_RX_FIFO0, &RxHeader, RxData) == HAL_OK) {
        hasPacket = true;
    }
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
extern "C" void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs) {
    if((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != RESET) {
        FDCAN::getInstance(hfdcan)->handleMessage();
    }
}