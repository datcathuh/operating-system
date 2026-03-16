#pragma once

#include "cpu_context.h"
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

enum task_state {
	TASK_READY,    // Ready to run, waiting for CPU
	TASK_RUNNING,  // Currently on CPU
	TASK_SLEEPING, // Waiting until a specific time
	TASK_BLOCKED,  // Waiting for I/O or event
};

#define TASK_NAME_LEN 70

struct task {
	char name[TASK_NAME_LEN];
	struct cpu_context_full ctx;
	uint8_t *kernel_stack;
	enum task_state state; // RUNNABLE, BLOCKED, etc.
};

void task_create(const char *name, struct task *t, void (*entry)(void));

#ifdef __cplusplus
}
#endif
