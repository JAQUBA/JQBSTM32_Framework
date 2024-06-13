#include "task_m.h"

TASK *task_pool = NULL;

uint8_t __fl_task_1mS;

void Task_m_timer(void) {
	__fl_task_1mS = true;
}

void init_task_m(TASK_PROC proc, uint16_t trigger)
{
	static TASK *tail = NULL;
	TASK *new_task = NULL;

	new_task = (TASK *) malloc(sizeof(TASK));
	new_task->proc = proc;
	new_task->trigger = trigger;
	new_task->flag = 0;
	new_task->ctr = trigger;

	if (task_pool == NULL) {
		task_pool = new_task;
		task_pool->next = task_pool;
		tail = task_pool;
	}
	else
	{
		new_task->next = task_pool;
		tail->next = new_task;
		tail = new_task;
	}
}

void exec_next(void)
{
	task_pool->proc(task_pool);
	task_pool = task_pool->next;
}

void trigger_task(void)
{
	if (__fl_task_1mS>0)//dyn
	{	__fl_task_1mS = 0;

		TASK *t = task_pool;

		do {
			if (t->ctr) {
				if (!--t->ctr) {
					t->ctr = t->trigger;
					bf_set(t->flag, 1);
				}
			}

			t = t->next;
		} while (t != task_pool);
	}
}
