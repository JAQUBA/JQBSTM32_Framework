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

Timer::Timer(TIM_HandleTypeDef *pHandler): _pHandler(pHandler), _callbackCount(0) {
    _Timer_instances[_Timer_instancesNum++] = this;
    
    // Initialize callback array
    for (uint8_t i = 0; i < TIMER_MAX_CALLBACKS; i++) {
        _callbacks[i].active = false;
        _callbacks[i].callback = nullptr;
    }
    
    HAL_TIM_Base_Start_IT(_pHandler);
}

void Timer::setPeriod(uint32_t period) {
    _pHandler->Instance->ARR = period-1;
}

int8_t Timer::findFreeCallbackSlot() {
    for (uint8_t i = 0; i < TIMER_MAX_CALLBACKS; i++) {
        if (!_callbacks[i].active) {
            return i;
        }
    }
    return -1;
}

int8_t Timer::findCallback(InterruptType interruptType) {
    for (uint8_t i = 0; i < TIMER_MAX_CALLBACKS; i++) {
        if (_callbacks[i].active && _callbacks[i].type == interruptType) {
            return i;
        }
    }
    return -1;
}

bool Timer::attachInterrupt(InterruptType interruptType, voidCallback_f callback) {
    if (!callback) return false;
    
    // Check if callback for this type already exists
    int8_t existingIndex = findCallback(interruptType);
    if (existingIndex >= 0) {
        // Replace existing callback
        _callbacks[existingIndex].callback = callback;
        return true;
    }
    
    // Find free slot for new callback
    int8_t freeIndex = findFreeCallbackSlot();
    if (freeIndex < 0) return false; // No free slots
    
    _callbacks[freeIndex].type = interruptType;
    _callbacks[freeIndex].callback = callback;
    _callbacks[freeIndex].active = true;
    _callbackCount++;
    
    return true;
}

void Timer::detachInterrupt(InterruptType interruptType) {
    int8_t index = findCallback(interruptType);
    if (index >= 0) {
        _callbacks[index].active = false;
        _callbacks[index].callback = nullptr;
        _callbackCount--;
    }
}

void Timer::handleInterrupt(InterruptType interruptType) {
    for (uint8_t i = 0; i < TIMER_MAX_CALLBACKS; i++) {
        if (_callbacks[i].active && _callbacks[i].type == interruptType && _callbacks[i].callback) {
            _callbacks[i].callback();
        }
    }
}

uint8_t Timer::getCallbackCount() const {
    return _callbackCount;
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
