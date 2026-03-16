#include "interrupt.h"
#include "memory.h"
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

#include "serial.h"

void schedule(void) {
	if (!task_current) {
		return;
	}

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
		task_current->state = TASK_READY;
		next_task->state = TASK_RUNNING;

		task_current_index = next;
		struct task *prev = task_current;
		task_current = next_task;

		context_switch(&prev->ctx, &next_task->ctx);
		/* Code execution past the above line is never happening.
		   So this function never returns. */
	}
}

void schedule_from_interrupt(struct cpu_context *interrupted_context) {
	if (!task_current) {
		return;
	}

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
		// Save current task state
		mem_copy(&task_current->ctx.cpu, interrupted_context,
		         sizeof(struct cpu_context));
		context_fpu_save(task_current->ctx.fxsave_area);

		task_current->state = TASK_READY;

		// Switch to next task
		task_current_index = next;
		task_current = next_task;

		next_task->state = TASK_RUNNING;

		// Restore next task state
		mem_copy(interrupted_context, &next_task->ctx.cpu,
		         sizeof(struct cpu_context));
		context_fpu_restore(next_task->ctx.fxsave_area);
	}
}

void yield(void) { schedule(); }

void scheduler_task_enumerate(task_enumerate_cb cb) {
	for (size_t i = 0; i < task_count; i++) {
		cb(task_tasks[i]);
	}
}
