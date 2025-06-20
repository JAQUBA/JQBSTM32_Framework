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
#include <climits> // For LONG_MAX and LONG_MIN

Scheduler mainTasks;
Scheduler interruptTasks;

// Performance counters for monitoring
static uint32_t _systemStartTime = 0;
static uint32_t _taskOverruns = 0;
static uint32_t _maxExecutionTime = 0;

taskStruct addTaskInterrupt(taskCallback_f functionPointer, uint32_t delay, bool single, Scheduler::taskTime time) {
    return interruptTasks.addTask(functionPointer, delay, single, time);
}

taskStruct addTaskMain(taskCallback_f functionPointer, uint32_t delay, bool single, Scheduler::taskTime time) {
    return mainTasks.addTask(functionPointer, delay, single, time);
}

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
	if (delay_us == 0) return; // Optimize for zero delay
	
	uint32_t clk_cycle_start = DWT->CYCCNT;
	delay_us *= (HAL_RCC_GetHCLKFreq() / 1000000);
	while ((DWT->CYCCNT - clk_cycle_start) < delay_us);
}
#else
// Fallback delay_us implementation without DWT
void delay_us(volatile uint32_t delay_us) {
	// Simple loop-based delay (less accurate but works without DWT)
	volatile uint32_t count = delay_us * (SystemCoreClock / 1000000 / 4);
	while(count--);
}
#endif

void delay(volatile uint32_t delay_ms) {
	if (delay_ms == 0) return; // Optimize for zero delay
	HAL_Delay(delay_ms);
}

// Make ulMillis volatile and atomic for thread safety
volatile uint32_t ulMillis = 0;

uint32_t millis() {
	return ulMillis;
}

Core::Core() {
	HAL_Init();
	SystemClock_Config();
	SysTick_Config(HAL_RCC_GetHCLKFreq() / 10000UL);
	
	// Initialize system start time
	_systemStartTime = HAL_GetTick();
	
	// Add optimized time keeping task
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
		uint32_t currentTime = millis();
		
		// Monitor execution time for performance analysis
		uint32_t executionStart = currentTime;
		
		mainTasks.execute();
		loop();
		
		uint32_t executionTime = millis() - executionStart;
		if (executionTime > _maxExecutionTime) {
			_maxExecutionTime = executionTime;		}
		
		// Check for task overruns (execution taking too long)
		if (executionTime > 10) { // 10ms threshold
			_taskOverruns++;
		}
	}
}

void HAL_IncTick(void) {
	// Optimize interrupt handler - do minimal work
	interruptTasks.poll();
	interruptTasks.execute();
	mainTasks.poll();
}

// Enhanced map function with bounds checking and overflow protection
long map(long x, long in_min, long in_max, long out_min, long out_max) {
	// Input validation
	if (in_min == in_max) {
		return out_min; // Avoid division by zero
	}
	
	// Use 64-bit arithmetic to prevent overflow
	int64_t result = (int64_t)(x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
	
	// Clamp result to prevent overflow
	if (result > LONG_MAX) return LONG_MAX;
	if (result < LONG_MIN) return LONG_MIN;
	
	return (long)result;
}

// Add system performance monitoring functions
uint32_t getSystemUptime() {
	return millis() - _systemStartTime;
}

uint32_t getTaskOverruns() {
	return _taskOverruns;
}

uint32_t getMaxExecutionTime() {
	return _maxExecutionTime;
}

void resetPerformanceCounters() {
	_taskOverruns = 0;
	_maxExecutionTime = 0;
}
__weak void init() {}
__weak void setup() {}
__weak void loop() {}