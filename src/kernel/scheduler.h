#pragma once

#include "task.h"

#ifdef __cplusplus
extern "C" {
#endif

void schedule(void);
void scheduler_start(void);
void yield(void);

void scheduler_task_add(struct task *task);

typedef void (*task_enumerate_cb)(struct task *);
void scheduler_task_enumerate(task_enumerate_cb cb);

#ifdef __cplusplus
}
#endif
