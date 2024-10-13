#include "Timer.h"

Timer::Timer(TIM_HandleTypeDef *htim) {
    _htim = htim;
}
void Timer::period(uint32_t period) {
}
void Timer::delay_us(uint32_t period) {
}
void Timer::attachInterrupt(voidCallback_f callback) {

}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    // if (htim->Instance == TIM4) {
        // timerCallback();
    // }
}