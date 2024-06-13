#ifndef __TASKI_H_
#define __TASKI_H_
#include "../../Core.h"
#ifdef __cplusplus
extern "C" {
#endif

struct _task_struct;
typedef void (*TASK_PROC)(struct _task_struct *self);

typedef struct _task_struct {
    struct _task_struct *next;
    TASK_PROC          proc;
    volatile uint8_t   flag;
    volatile uint16_t  trigger;
    volatile uint16_t  ctr;
} TASK;

#define TASK_PROCEDURE(name)    void name(TASK * self)

void Task_m_timer(void);
void init_task_m(TASK_PROC proc, uint16_t trigger);
void exec_next(void);
void trigger_task(void);
#ifdef __cplusplus
}
#endif

#endif