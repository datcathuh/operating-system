#include "arch/x86_64/cpu/interrupt.h"
#include "scheduler.h"

#define TASK_MAX 64

struct task *task_current = NULL;
struct task *tasks[TASK_MAX] = {0};
int task_count = 0;
int task_current_index = 0;

void scheduler_task_add(struct task *task) {
	tasks[task_count] = task;
	task_count++;

	if (task_count == 1) {
		task_current = task;
	}
}

void schedule(void) {
	cli();

	int next = (task_current_index + 1) % task_count;
	struct task *next_task = tasks[next];

	if (next_task != task_current) {
		struct task *prev = task_current;
		task_current = next_task;
		context_switch(&prev->ctx, &next_task->ctx);
	}

	sti();
}

void yield(void) { schedule(); }
