#pragma once

#include "task.h"

void schedule(void);
void yield(void);

void scheduler_task_add(struct task *task);
