#include "UART.h"
#include "../GPIO/GPIO.h"

UART *_UART_instances[UART_MAX_INSTANCES];
uint8_t _UART_instancesNum;

UART::UART(UART_HandleTypeDef *instance) {
    _pInstance = instance;
    _UART_instances[_UART_instancesNum++] = this;
}

void UART::init() {
    HAL_UART_Receive_IT(_pInstance, &Received_u1, 1);
}

UART *UART::getInstance(UART_HandleTypeDef *_instance) {
    for (size_t i = 0; i < _UART_instancesNum; i++) {
        if(_UART_instances[i]->_pInstance->Instance == _instance->Instance) return _UART_instances[i];
    }
    return nullptr;
}
void UART::rxInterrupt() {
    rx_buffer[rx_data_index] = Received_u1;
    rx_data_index++;
    if(rx_data_index >= 256) {
		rx_data_index = 0;
	}
    lastReceivedByte = millis();
    received = true;
    HAL_UART_Receive_IT(_pInstance, &Received_u1, 1);
}
void UART::txInterrupt() {}

void UART::poll() {
    if(received && millis() > lastReceivedByte + 20) {
        if(fpOnReceive) fpOnReceive(rx_buffer, rx_data_index);
        rx_data_index = 0;
        received = false;
    }
}
void UART::onReceiveHandler(void(*onReceive)(uint8_t* data, uint16_t length)) {fpOnReceive = onReceive;}
void UART::onTransmitHandler(void(*onTransmit)()) {fpOnTransmit = onTransmit;}
void UART::send(uint8_t *data, uint16_t length) {
    if(fpOnTransmit) fpOnTransmit();
    // HAL_UART_Transmit_DMA(_pInstance, data, length);
    HAL_UART_Transmit(_pInstance, data, length, 1000);
}
void UART::send(const char *data, uint16_t length) {send((uint8_t *)data, length);}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {UART::getInstance(huart)->rxInterrupt();}
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {UART::getInstance(huart)->txInterrupt();}