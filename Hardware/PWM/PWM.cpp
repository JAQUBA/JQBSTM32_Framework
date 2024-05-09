#include "PWM.h"

PWM::PWM(TIM_HandleTypeDef *instance, uint32_t channel) {
    _channel = channel;
    _instance = instance;
}
void PWM::init() {
    HAL_TIM_PWM_Start(_instance, _channel);
}
void PWM::set(uint32_t value) {
    __HAL_TIM_SET_COMPARE(_instance, _channel, value);
}