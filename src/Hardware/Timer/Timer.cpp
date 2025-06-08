/*
 * JQBSTM32 Framework - Timer.cpp Implementation
 * Copyright (C) 2024 JAQUBA (kjakubowski0492@gmail.com)
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
#include "Timer.h"
#ifdef HAL_TIM_MODULE_ENABLED

Timer *_Timer_instances[TIMER_MAX_INSTANCES];
uint8_t _Timer_instancesNum = 0;

Timer *Timer::getInstance(TIM_HandleTypeDef *pHandler) {
    for (size_t i = 0; i < _Timer_instancesNum; i++) {
        if(_Timer_instances[i]->_pHandler->Instance == pHandler->Instance) return _Timer_instances[i];
    }
    return nullptr;
}

Timer::Timer(TIM_HandleTypeDef *pHandler): _pHandler(pHandler) {
    _Timer_instances[_Timer_instancesNum++] = this;
    HAL_TIM_Base_Start_IT(_pHandler);
}

void Timer::setPeriod(uint32_t period) {
    _pHandler->Instance->ARR = period-1;
}

void Timer::attachInterrupt(InterruptType interruptType, voidCallback_f callback) {
    _callbacks.push_back(std::make_pair(interruptType, callback));
}

void Timer::handleInterrupt(InterruptType interruptType) {
    for (const auto& entry : _callbacks) {
        if (entry.first == interruptType) entry.second();
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
   Timer *timer = Timer::getInstance(htim);
   if(timer) timer->handleInterrupt(Timer::PeriodElapsedCallback);
}
void HAL_TIM_PeriodElapsedHalfCpltCallback(TIM_HandleTypeDef *htim) {
    Timer *timer = Timer::getInstance(htim);
    if(timer) timer->handleInterrupt(Timer::PeriodElapsedHalfCpltCallback);
}
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim) {
    Timer *timer = Timer::getInstance(htim);
    if(timer) timer->handleInterrupt(Timer::OC_DelayElapsedCallback);
}
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
    Timer *timer = Timer::getInstance(htim);
    if(timer) timer->handleInterrupt(Timer::IC_CaptureCallback);
}
void HAL_TIM_IC_CaptureHalfCpltCallback(TIM_HandleTypeDef *htim) {
    Timer *timer = Timer::getInstance(htim);
    if(timer) timer->handleInterrupt(Timer::IC_CaptureHalfCpltCallback);
}
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {
    Timer *timer = Timer::getInstance(htim);
    if(timer) timer->handleInterrupt(Timer::PWM_PulseFinishedCallback);
}
void HAL_TIM_PWM_PulseFinishedHalfCpltCallback(TIM_HandleTypeDef *htim)	{
    Timer *timer = Timer::getInstance(htim);
    if(timer) timer->handleInterrupt(Timer::PWM_PulseFinishedHalfCpltCallback);
}
void HAL_TIM_TriggerCallback(TIM_HandleTypeDef *htim) {
    Timer *timer = Timer::getInstance(htim);
    if(timer) timer->handleInterrupt(Timer::TriggerCallback);
}
void HAL_TIM_TriggerHalfCpltCallback(TIM_HandleTypeDef *htim) {
    Timer *timer = Timer::getInstance(htim);
    if(timer) timer->handleInterrupt(Timer::TriggerHalfCpltCallback);
}
void HAL_TIM_ErrorCallback(TIM_HandleTypeDef *htim) {
    Timer *timer = Timer::getInstance(htim);
    if(timer) timer->handleInterrupt(Timer::ErrorCallback);
}

#endif // HAL_TIM_MODULE_ENABLED
