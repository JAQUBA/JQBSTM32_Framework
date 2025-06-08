/*
 * JQBSTM32 Framework - PWM.cpp Implementation
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
