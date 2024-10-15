#include "Timer.h"

Timer::Timer(TIM_TypeDef *timerBase, uint8_t prescaler, uint8_t period) {
	TIM_MasterConfigTypeDef sMasterConfig = {0};
	htim.Instance = timerBase;
	htim.Init.Prescaler = prescaler;
	htim.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim.Init.Period = period;
	htim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim) != HAL_OK) {
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim, &sMasterConfig) != HAL_OK) {
		Error_Handler();
	}
	__TIM7_CLK_ENABLE();
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {

}