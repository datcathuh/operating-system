#include "memory.h"
#include "string.h"
#include "task.h"

void task_create(const char *name, struct task *t, void (*entry)(void)) {
	mem_set(t, 0, sizeof(struct task));
	str_copy(t->name, TASK_NAME_LEN, name);

	void *page = mem_page_alloc();
	mem_page_map((uint64_t)page, (uint64_t)page,
	             MEM_PAGE_PRESENT | MEM_PAGE_WRITABLE | MEM_PAGE_NX);
	t->kernel_stack = page;

	/* Subtract by 8 which MUST be done since the compiler believes
	   that a return address exist in the stack for the entry function.
	   But since we are jumping directly into the entry function we
	   must subtract by 8 in order for all the memory alignment
	   assumptions done by the compiler to be true. */
	uint64_t stack_top = (uint64_t)t->kernel_stack + MEM_PAGE_SIZE - 8;

	t->ctx.rip = (uint64_t)entry;
	t->ctx.rsp = stack_top;
	t->ctx.rflags = 0x202; // IF=1
	context_fpu_save(t->ctx.fxsave_area);
}
