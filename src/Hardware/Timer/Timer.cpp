#include "Timer.h"

Timer::Timer(TIM_HandleTypeDef *htim) {
    _htim = htim;
}
void Timer::setup(uint32_t period) {
    _htim->Init.Prescaler = 10000 - 1;
    _htim->Init.Period = period - 1;
}
void Timer::attachInterrupt(voidCallback_f callback) {

}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    // if (htim->Instance == TIM4) {
        // timerCallback();
    // }
}