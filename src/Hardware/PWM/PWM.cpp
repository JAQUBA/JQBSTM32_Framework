#include "PWM.h"
#ifdef HAL_TIM_MODULE_ENABLED

PWM::PWM(TIM_HandleTypeDef *pHandler, uint32_t channel) : Timer(pHandler), _channel(channel) {
    HAL_TIM_PWM_Start(_pHandler, _channel);
}
void PWM::set(uint32_t value) {
    _value=value;
    __HAL_TIM_SET_COMPARE(_pHandler, _channel, value);
}
uint32_t PWM::get() {
    return _value;
}
#endif