#include "Timer.h"

Timer::Timer(TIM_HandleTypeDef *htim) {
	this->htim = *htim;
}
void Timer::attachInterrupt(voidCallback_f callback) {
	HAL_TIM_RegisterCallback(&htim, HAL_TIM_PERIOD_ELAPSED_CB_ID, interrupt);
	HAL_TIM_Base_Start_IT(&htim);
}
void Timer::interrupt(TIM_HandleTypeDef *pHandler) {
	// Do something
}