/*
 * JQBSTM32 Framework - Core Implementation
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

#include "Core.h"

Scheduler mainTasks;
Scheduler interruptTasks;

taskStruct addTaskInterrupt(taskCallback_f functionPointer, uint32_t delay, bool single, Scheduler::taskTime time) {return interruptTasks.addTask(functionPointer, delay, single, time);}
taskStruct addTaskMain(taskCallback_f functionPointer, uint32_t delay, bool single, Scheduler::taskTime time) {return mainTasks.addTask(functionPointer, delay, single, time);}

#ifdef DWT
uint8_t DWT_COUNTER_ENABLE(void) {
	CoreDebug->DEMCR &= ~0x01000000;
	CoreDebug->DEMCR |=  0x01000000;
	DWT->CTRL &= ~0x00000001;
	DWT->CTRL |=  0x00000001;
	DWT->CYCCNT = 0;
	uint32_t c = DWT->CYCCNT;
	__ASM volatile ("NOP"); __ASM volatile ("NOP"); __ASM volatile ("NOP");
	if((DWT->CYCCNT - c) == 0){ return 0; }
	return (DWT->CYCCNT - c);
}
void delay_us(volatile uint32_t delay_us) {
	uint32_t clk_cycle_start = DWT->CYCCNT;
	delay_us *= (HAL_RCC_GetHCLKFreq() / 1000000);
	while ((DWT->CYCCNT - clk_cycle_start) < delay_us);
}
#endif
void delay(volatile uint32_t delay_ms) {HAL_Delay(delay_ms);}
uint32_t ulMillis;
uint32_t millis() {return ulMillis;}

Core::Core() {
	HAL_Init();
	SystemClock_Config();
	SysTick_Config(HAL_RCC_GetHCLKFreq() / 10000UL);
	addTaskInterrupt(taskCallback {
		uwTick += (uint32_t)uwTickFreq;
		ulMillis++;
	}, 1);
	init();
#ifdef DWT
	DWT_COUNTER_ENABLE();
#endif
}
Core _core;

int main() {
	setup();
	while (1) {
		mainTasks.execute();
		loop();
	}
}
void HAL_IncTick(void) {
	interruptTasks.poll();
	interruptTasks.execute();
	mainTasks.poll();
}
long map(long x, long in_min, long in_max, long out_min, long out_max) {
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
__weak void init() {}
__weak void setup() {}
__weak void loop() {}