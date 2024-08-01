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

void HAL_CAN_TxCpltCallback(CAN_HandleTypeDef* pHandler) {CAN::getInstance(pHandler)->txInterrupt();}
void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* pHandler) {CAN::getInstance(pHandler)->rxInterrupt();}
void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *pHandler) {CAN::getInstance(pHandler)->errorInterrupt();}

CAN::CAN(CAN_HandleTypeDef *pHandler) {
    _pInstance = pHandler;
    _CAN_instances[_CAN_instancesNum++] = this;

	HAL_CAN_Start(_pInstance);

    addTaskMain(taskCallback {

    });
}
void CAN::rxInterrupt() {

}
void CAN::txInterrupt() {

}
void CAN::errorInterrupt() {

}
void CAN::send(uint32_t identifier, uint8_t *pData, uint16_t Size, uint32_t DataLength) {

}
void CAN::onPacket(uint16_t commNumber, dataCallback_f handler) {

}
#endif