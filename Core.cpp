#include "Core.h"

#include "tim.h"

struct taskStruct *tasksMain;
struct taskStruct *tasksInterrupt;

#ifdef DWT
uint8_t DWT_COUNTER_ENABLE(void)
{
  uint32_t c;
  //Wlacz TRC,
  //Ustawienie bitu TRCENA
  //Wlacza takie bloki jak DWT, ITM, ETM, TPIU
  CoreDebug->DEMCR &= ~0x01000000;
  CoreDebug->DEMCR |=  0x01000000;
   
  //Wlacz DWT w rejestrze kontrolnym
  DWT->CTRL &= ~0x00000001; //Czyszczenie
  DWT->CTRL |=  0x00000001; //Ustawienie
   
  //Ustawienie licznika na wartosc 0
  DWT->CYCCNT = 0;
   
  //Wartosci z CYCCNT do zmiennej c
  c = DWT->CYCCNT;
  //Czekanie   
  __ASM volatile ("NOP"); __ASM volatile ("NOP"); __ASM volatile ("NOP");
   
  //Zwraca roznice pomiedzy DWT->CYCCNT a ta wartoscia kilka cykli wczesniej
  //Jesli wynosi ona 0 to licznik nie dziala
  if((DWT->CYCCNT - c) == 0)
  { return 0; }
  return (DWT->CYCCNT - c);
}
void delay_us(volatile uint32_t delay_us) {
  uint32_t clk_cycle_start = DWT->CYCCNT;
  delay_us *= (HAL_RCC_GetHCLKFreq() / 1000000);
  while ((DWT->CYCCNT - clk_cycle_start) < delay_us);
}
#endif

void delay(volatile uint32_t delay_ms) {HAL_Delay(delay_ms);}

unsigned long ulMillis;
unsigned long millis() {return ulMillis;}

int main() {
	HAL_Init();
	SystemClock_Config();
	
	init();

	MX_TIM7_Init();
	HAL_TIM_Base_Start_IT(&htim7);
	
#ifdef DWT
	DWT_COUNTER_ENABLE();
#endif
	setup();

	while (1) {
        struct taskStruct *temp = tasksMain, *old;
		while (temp != NULL) {
			if(temp->_delay == 0) {
				temp->functionPointer(temp);
                temp->_delay = temp->delay;

				if(temp->_single) {
					if(temp == tasksMain) {
						tasksMain=temp->next;
					} else {
						old->next = temp->next;
					}
					free(temp);
				}
			} else {
				old = temp;
				temp = temp->next;
			}
		}
        loop();
    }
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim == &htim7 ) {
		struct taskStruct *tempMain = tasksMain;
		while (tempMain != NULL) {
			if(tempMain->_delay > 0) {
				tempMain->_delay--;
			}
			tempMain = tempMain->next;
		}

		struct taskStruct *tempInterrupt = tasksInterrupt, *old;
		while (tempInterrupt != NULL) {
			if(tempInterrupt->_delay > 0) {
				tempInterrupt->_delay--;
			}
			if(tempInterrupt->_delay == 0) {
				tempInterrupt->functionPointer(tempInterrupt);
				tempInterrupt->_delay = tempInterrupt->delay;

				if(tempInterrupt->_single) {
					if(tempInterrupt == tasksInterrupt) {
						tasksInterrupt=tempInterrupt->next;
					} else {
						old->next = tempInterrupt->next;
					}
					free(tempInterrupt);
				}
			} else {
				old = tempInterrupt;
				tempInterrupt = tempInterrupt->next;
			}
			// tempInterrupt = tempInterrupt->next;
		}
		ulMillis++;
	}
}

task *addTaskMain(void (*functionPointer)(struct taskStruct *task), uint32_t delay, bool single) {
	struct taskStruct *temp = tasksMain, *r;
	if(tasksMain==NULL) {
		temp = (struct taskStruct *)malloc(sizeof(struct taskStruct));
		temp->functionPointer=functionPointer;
		temp->delay=delay;
		temp->_delay=delay;
		temp->_single=single;
		temp->next=NULL;
		tasksMain=temp;
		return (task*)temp;
	} else {
		while(temp->next != NULL) temp = temp->next;
		r = (struct taskStruct*)malloc(sizeof(struct taskStruct));
		r->functionPointer=functionPointer;
		r->next=NULL;
		r->delay=delay;
		r->_delay=delay;
		r->_single=single;
		temp->next=r;
		return (task*)r;
	}
}
task *addTaskInterrupt(void (*functionPointer)(struct taskStruct *task), uint32_t delay, bool single) {
	struct taskStruct *temp = tasksInterrupt, *r;
	if(tasksInterrupt==NULL) {
		temp = (struct taskStruct *)malloc(sizeof(struct taskStruct));
		temp->functionPointer=functionPointer;
		temp->next=NULL;
		temp->delay=delay;
		temp->_delay=delay;
		temp->_single=single;
		tasksInterrupt=temp;
		return (task*)temp;
	} else {
		while(temp->next != NULL) temp = temp->next;
		r = (struct taskStruct*)malloc(sizeof(struct taskStruct));
		r->functionPointer=functionPointer;
		r->next=NULL;
		r->delay=delay;
		r->_delay=delay;
		r->_single=single;
		temp->next=r;
		return (task*)r;
	}
}

long map(long x, long in_min, long in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}