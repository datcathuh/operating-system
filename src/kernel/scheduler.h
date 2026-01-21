#pragma once

#include "task.h"

void schedule(void);
void scheduler_start(void);
void yield(void);

void scheduler_task_add(struct task *task);
