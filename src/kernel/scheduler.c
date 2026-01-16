#include "task.h"

#define TASK_MAX 64

struct task *current;
struct task *tasks[TASK_MAX];
int task_count;
int task_current_index;

void schedule(void) {
	// TODO cli

    int next = (task_current_index + 1) % task_count;
    struct task *next_task = tasks[next];

    if (next_task != current) {
        struct task *prev = current;
        current = next_task;
        context_switch(&prev->ctx, &next_task->ctx);
    }

	// TODO sti
}

void yield(void) { schedule(); }
