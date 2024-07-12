#include "UART.h"

UART *_UART_instances[UART_MAX_INSTANCES];
uint8_t _UART_instancesNum;

UART *UART::getInstance(UART_HandleTypeDef *pHandler) {
    for (size_t i = 0; i < _UART_instancesNum; i++) {
        if(_UART_instances[i]->_pHandler->Instance == pHandler->Instance) return _UART_instances[i];
    }
    return nullptr;
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {UART::getInstance(huart)->rxInterrupt();}
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {UART::getInstance(huart)->txInterrupt();}

UART::UART(UART_HandleTypeDef *pHandler) {
    _pHandler = pHandler;
    _UART_instances[_UART_instancesNum++] = this;
    
    HAL_UART_Receive_IT(_pHandler, &Received_u1, 1);

    addTaskMain(taskCallback {
        if(received && millis() > lastReceivedByte + 20) {
            if(fpOnReceive) fpOnReceive(rx_buffer, rx_data_index);
            rx_data_index = 0;
            received = false;
        }
    }, 0);
}


void UART::rxInterrupt() {
    rx_buffer[rx_data_index] = Received_u1;
    rx_data_index++;
    if(rx_data_index >= 256) {
		rx_data_index = 0;
	}
    lastReceivedByte = millis();
    received = true;
    HAL_UART_Receive_IT(_pHandler, &Received_u1, 1);
}
void UART::txInterrupt() {

}

void UART::onReceiveHandler(std::function<void(uint8_t* data, uint16_t length)> onReceive) {fpOnReceive = onReceive;}
void UART::onTransmitHandler(std::function<void()> onTransmit) {fpOnTransmit = onTransmit;}

void UART::send(uint8_t *data, uint16_t length) {
    if(fpOnTransmit) fpOnTransmit();
    HAL_UART_Transmit_DMA(_pHandler, data, length);
    // HAL_UART_Transmit(_pHandler, data, length, 1000);
}
void UART::send(const char *data, uint16_t length) {
    send((uint8_t *)data, length);
}



