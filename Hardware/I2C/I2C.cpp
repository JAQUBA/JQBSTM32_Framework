#include "I2C.h"

I2C *_I2C_instances[I2C_MAX_INSTANCES];
uint8_t _I2C_instancesNum = 0;

I2C::I2C(I2C_HandleTypeDef* instance) {
    _pInstance = instance;
	_I2C_instances[_I2C_instancesNum++] = this;
}
I2C *I2C::getInstance(I2C_HandleTypeDef *_instance) {
    for (size_t i = 0; i < _I2C_instancesNum; i++) {
        if(_I2C_instances[i]->_pInstance->Instance == _instance->Instance) return _I2C_instances[i];
    }
    return nullptr;
}
void I2C::send(uint16_t DevAddress, uint8_t *pData, uint16_t Size) {
	HAL_I2C_Master_Transmit(_pInstance, DevAddress,  pData,  Size, 1000);
}
void I2C::recv(uint16_t DevAddress, uint8_t *pData, uint16_t Size) {
    HAL_I2C_Master_Receive(_pInstance, DevAddress, pData, Size, 1000);
}
void I2C::txInterrupt() {
}


void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c) {I2C::getInstance(hi2c)->txInterrupt();}