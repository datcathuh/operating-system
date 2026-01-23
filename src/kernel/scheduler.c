
#include "arch/x86_64/cpu/interrupt.h"
#include "scheduler.h"

#define TASK_MAX 64

static struct task *task_current = NULL;
static struct task *task_tasks[TASK_MAX] = {0};
static size_t task_count = 0;
static int task_current_index = 0;

void scheduler_task_add(struct task *task) {
	task_tasks[task_count] = task;
	task_count++;
}

void scheduler_start(void) {
	if (task_current == NULL) {
		/* Never returning from this call since we
		   restore a context directly. The first task
		   entry function will be the new "main"
		*/
		task_current = task_tasks[0];
		context_restore(&task_current->ctx);
	}
}

void schedule(void) {
	cli();

	int next = (task_current_index + 1) % task_count;
	struct task *next_task = task_tasks[next];

	/*
	serial_puts("CS Task old: ");
	serial_puts(task_current->name);
	serial_puts(" new: ");
	serial_puts(next_task->name);
	serial_puts("\n");
	*/

	if (next_task != task_current) {
		task_current_index = next;
		struct task *prev = task_current;
		task_current = next_task;

		context_switch(&prev->ctx, &next_task->ctx);
	}

	sti();
}

void yield(void) { schedule(); }

void scheduler_task_enumerate(task_enumerate_cb cb) {
	for (int i = 0; i < task_count; i++) {
		cb(task_tasks[i]);
	}
}
