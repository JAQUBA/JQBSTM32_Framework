#include "OneWire.h"

OneWire::OneWire(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin) : GPIOx(GPIOx), GPIO_Pin(GPIO_Pin) {
    // Initialize the GPIO pin
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void OneWire::reset() {
    setPinOutput();
    HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_RESET);
    delayMicroseconds(480);
    setPinInput();
    delayMicroseconds(70);
    uint8_t presence = HAL_GPIO_ReadPin(GPIOx, GPIO_Pin);
    delayMicroseconds(410);
}

void OneWire::writeBit(uint8_t bit) {
    setPinOutput();
    if (bit) {
        HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_RESET);
        delayMicroseconds(10);
        HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_SET);
        delayMicroseconds(55);
    } else {
        HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_RESET);
        delayMicroseconds(65);
        HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_SET);
        delayMicroseconds(5);
    }
}

uint8_t OneWire::readBit() {
    setPinOutput();
    HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_RESET);
    delayMicroseconds(3);
    setPinInput();
    delayMicroseconds(10);
    uint8_t bit = HAL_GPIO_ReadPin(GPIOx, GPIO_Pin);
    delayMicroseconds(53);
    return bit;
}

void OneWire::writeByte(uint8_t byte) {
    for (int i = 0; i < 8; i++) {
        writeBit(byte & 0x01);
        byte >>= 1;
    }
}

uint8_t OneWire::readByte() {
    uint8_t byte = 0;
    for (int i = 0; i < 8; i++) {
        byte >>= 1;
        if (readBit()) {
            byte |= 0x80;
        }
    }
    return byte;
}

void OneWire::setPinOutput() {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void OneWire::setPinInput() {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void OneWire::delayMicroseconds(uint32_t us) {
    uint32_t start = DWT->CYCCNT;
    uint32_t delayTicks = us * (HAL_RCC_GetHCLKFreq() / 1000000);
    while ((DWT->CYCCNT - start) < delayTicks);
}