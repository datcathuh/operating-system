#pragma once

#include "cpu_context.h"
#include "types.h"

enum task_state {
	TASK_STATE_RUNNABLE,
	TASK_STATE_BLOCKED
};

struct task {
    struct cpu_context ctx;
    uint8_t *kernel_stack;
    enum task_state state;  // RUNNABLE, BLOCKED, etc.
};

void task_create(struct task *t, void (*entry)(void));
