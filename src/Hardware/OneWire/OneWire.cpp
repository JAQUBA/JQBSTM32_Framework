#include "OneWire.h"

OneWire::OneWire(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin) {
    this->GPIOx = GPIOx;
    this->GPIO_Pin = GPIO_Pin;
}

void OneWire::transmit(
    uint8_t *pData, uint16_t Size,
    dataCallback_f callbackFn
){

}

void OneWire::receive(
    uint8_t *pData, uint16_t Size,
    dataCallback_f callbackFn
){

}
void OneWire::write(uint8_t data) {
    
}
uint8_t OneWire::read(){

}