#include "Core.h"

Scheduler mainTasks;
Scheduler interruptTasks;

void addTaskInterrupt(taskCallback_f functionPointer, uint32_t delay, bool single) {interruptTasks.addTask(functionPointer, delay, single);}
void addTaskMain(taskCallback_f functionPointer, uint32_t delay, bool single) {mainTasks.addTask(functionPointer, delay, single);}

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
  init();
	HAL_TIM_Base_Start_IT(&TASK_TIMER);
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
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim == &TASK_TIMER) {
		interruptTasks.poll1ms();
		interruptTasks.execute();
		mainTasks.poll1ms();
		ulMillis++;
	}
}
long map(long x, long in_min, long in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

__weak void init() {}
__weak void setup() {}
__weak void loop() {}