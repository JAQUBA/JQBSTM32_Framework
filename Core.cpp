#include "Core.h"

#include "tim.h"

struct taskStruct *tasksMain;
struct taskStruct *tasksInterrupt;

void delay(uint32_t delay_ms) {
	HAL_Delay(delay_ms);
}
unsigned long ulMillis;
unsigned long millis() {return ulMillis;}

int main() {
	HAL_Init();
	SystemClock_Config();
	
	init();

	MX_TIM7_Init();
	HAL_TIM_Base_Start_IT(&htim7);

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

void addTaskMain(void (*functionPointer)(struct taskStruct *task), uint16_t delay, bool single) {
	struct taskStruct *temp = tasksMain, *r;
	if(tasksMain==NULL) {
		temp = (struct taskStruct *)malloc(sizeof(struct taskStruct));
		temp->functionPointer=functionPointer;
		temp->delay=delay;
		temp->_delay=delay;
		temp->_single=single;
		temp->next=NULL;
		tasksMain=temp;
	} else {
		while(temp->next != NULL) temp = temp->next;
		r = (struct taskStruct*)malloc(sizeof(struct taskStruct));
		r->functionPointer=functionPointer;
		r->next=NULL;
		r->delay=delay;
		r->_delay=delay;
		r->_single=single;
		temp->next=r;
	}
}
void addTaskInterrupt(void (*functionPointer)(struct taskStruct *task), uint16_t delay, bool single) {
	struct taskStruct *temp = tasksInterrupt, *r;
	if(tasksInterrupt==NULL) {
		temp = (struct taskStruct *)malloc(sizeof(struct taskStruct));
		temp->functionPointer=functionPointer;
		temp->next=NULL;
		temp->delay=delay;
		temp->_delay=delay;
		temp->_single=single;
		tasksInterrupt=temp;
	} else {
		while(temp->next != NULL) temp = temp->next;
		r = (struct taskStruct*)malloc(sizeof(struct taskStruct));
		r->functionPointer=functionPointer;
		r->next=NULL;
		r->delay=delay;
		r->_delay=delay;
		r->_single=single;
		temp->next=r;
	}
}

long map(long x, long in_min, long in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}