#include "GPIO.h"

HardwareGPIO GPIO;

HardwareGPIO::HardwareGPIO() {
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
}

void HardwareGPIO::setup(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint32_t mode) {
    GPIO_InitTypeDef GPIO_InitStruct  = {0};

    GPIO_InitStruct.Pin = GPIO_Pin;
    GPIO_InitStruct.Mode = mode;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}
void HardwareGPIO::setup(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint32_t mode, uint32_t pullup) {
    GPIO_InitTypeDef GPIO_InitStruct  = {0};

    GPIO_InitStruct.Pin = GPIO_Pin;
    GPIO_InitStruct.Mode = mode;
    GPIO_InitStruct.Pull = pullup;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}
void HardwareGPIO::setup(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint32_t mode, uint32_t pullup, uint32_t alternate) {
    GPIO_InitTypeDef GPIO_InitStruct  = {0};
    GPIO_InitStruct.Pin = GPIO_Pin;
    GPIO_InitStruct.Mode = mode;
    GPIO_InitStruct.Pull = pullup;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    // GPIO_InitStruct.Alternate = alternate;
    HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void HardwareGPIO::set(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState) {
    HAL_GPIO_WritePin(GPIOx, GPIO_Pin, PinState);
}
void HardwareGPIO::toggle(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin) {
    HAL_GPIO_TogglePin(GPIOx, GPIO_Pin);
}
GPIO_PinState HardwareGPIO::readInput(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin) {
    return HAL_GPIO_ReadPin(GPIOx, GPIO_Pin);
}
GPIO_PinState HardwareGPIO::readOutput(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin) {
    assert_param(IS_GPIO_PIN(GPIO_Pin));
    if ((GPIOx->ODR & GPIO_Pin) != 0x00u) return GPIO_PIN_SET;
    else return GPIO_PIN_RESET;
}

void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin) {}
void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin) {}