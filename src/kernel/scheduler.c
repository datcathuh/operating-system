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
}

#include "serial.h"

void scheduler_start(void) {
	if(task_current == NULL) {
		/* Never returning from this call since we
		   restore a context directly. The first task
		   entry function will be the new "main"
		*/
		task_current = tasks[0];
		context_restore(&task_current->ctx);
	}
}

void schedule(void) {
	cli();

	int next = (task_current_index + 1) % task_count;
	struct task *next_task = tasks[next];

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
