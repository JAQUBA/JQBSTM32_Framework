#include "PWM.h"


PWM::PWM(TIM_HandleTypeDef *pHandler, uint32_t channel, Display led) {
    _channel = channel;
    _pHandler = pHandler;
    HAL_TIM_PWM_Start(_pHandler, _channel);
    addTaskMain(taskCallback {
        led.set(displayMap::Y0);
		addTaskMain(taskCallback {led.reset(displayMap::Y0);}, _value, true);
        }, 500);
}

void PWM::set(uint32_t value) {
    _value=value>>1;
    __HAL_TIM_SET_COMPARE(_pHandler, _channel, value);
}