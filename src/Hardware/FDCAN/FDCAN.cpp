#ifdef _JQB_USE_FDCAN
#include "FDCAN.h"


FDCAN *_FDCAN_instances[FDCAN_MAX_INSTANCES];
uint8_t _FDCAN_instancesNum;
FDCAN *FDCAN::getInstance(FDCAN_HandleTypeDef *_instance) {
    for (size_t i = 0; i < _FDCAN_instancesNum; i++) {
        if(_FDCAN_instances[i]->_pInstance->Instance == _instance->Instance) return _FDCAN_instances[i];
    }
    return nullptr;
}
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *pHandler, uint32_t RxFifo0ITs) {
    if((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != RESET) FDCAN::getInstance(pHandler)->rxInterrupt();
}

FDCAN::FDCAN(FDCAN_HandleTypeDef *pHandler) {
    _pInstance = pHandler;
    _FDCAN_instances[_FDCAN_instancesNum++] = this;

    sFilterConfig.IdType = FDCAN_EXTENDED_ID;
	sFilterConfig.FilterType = FDCAN_FILTER_MASK;
	sFilterConfig.FilterID1 = 0;
	sFilterConfig.FilterID2 = 0;
	HAL_FDCAN_ConfigFilter(_pInstance, &sFilterConfig);
	HAL_FDCAN_ConfigRxFifoOverwrite(_pInstance, FDCAN_RX_FIFO0, FDCAN_RX_FIFO_OVERWRITE);
	HAL_FDCAN_ConfigRxFifoOverwrite(_pInstance, FDCAN_RX_FIFO1, FDCAN_RX_FIFO_OVERWRITE);
	HAL_FDCAN_Start(_pInstance);
	HAL_FDCAN_ActivateNotification(_pInstance, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);

    pTxHeader.IdType = FDCAN_EXTENDED_ID;
	pTxHeader.DataLength = FDCAN_DLC_BYTES_8;
	pTxHeader.TxFrameType = FDCAN_DATA_FRAME;
	pTxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
	pTxHeader.BitRateSwitch = FDCAN_BRS_OFF;
	pTxHeader.FDFormat = FDCAN_FD_CAN;
	pTxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
	pTxHeader.MessageMarker = 0;

    addTaskMain(taskCallback {
        if(!hasPacket) return;
        uint32_t commNumber = pRxHeader.Identifier;
        for(auto &handler : handlers) {
            if(handler.commNumber == commNumber) {
                handler.handler(pData, sizeof(pData));
                return;
            }
        }
        hasPacket = false;
    });
}
void FDCAN::rxInterrupt() {
    if(HAL_FDCAN_GetRxMessage(_pInstance, FDCAN_RX_FIFO0, &pRxHeader, pData) != HAL_OK) return;
    hasPacket = true;
}
void FDCAN::send(uint32_t identifier, uint8_t *pData, uint16_t Size, uint32_t DataLength) {
    pTxHeader.Identifier = identifier;
    pTxHeader.DataLength = DataLength;
    HAL_FDCAN_AddMessageToTxFifoQ(_pInstance, &pTxHeader, pData);
}

void FDCAN::onPacket(uint16_t commNumber, dataCallback_f cHandler) {
    handler handler;
    handler.commNumber = commNumber;
    handler.handler = cHandler;
    handlers.push_back(handler);
}
#endif