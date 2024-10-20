#include "Timer.h"

Timer::Timer(TIM_HandleTypeDef *htim) {
	this->htim = *htim;
}
void Timer::attachInterrupt(voidCallback_f callback) {
	HAL_TIM_Base_Start_IT(&htim);
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {

}