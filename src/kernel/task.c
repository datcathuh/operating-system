#include "memory.h"
#include "task.h"

void task_create(struct task *t, void (*entry)(void)) {
	void *page = mem_page_alloc();
	mem_page_map((uint64_t)page, (uint64_t)page, MEM_PAGE_PRESENT | MEM_PAGE_WRITABLE | MEM_PAGE_NX);
    t->kernel_stack = page;

	/* Subtract by 8 which MUST be done since the compiler believes
	   that a return address exist in the stack for the entry function.
	   But since we are jumping directly into the entry function we
	   must subtract by 8 in order for all the memory alignment
	   assumptions done by the compiler to be true. */
    uint64_t stack_top = (uint64_t)t->kernel_stack + MEM_PAGE_SIZE - 8;

    mem_set(&t->ctx, 0, sizeof(t->ctx));
    t->ctx.rip = (uint64_t)entry;
    t->ctx.rsp = stack_top;
    t->ctx.rflags = 0x202; // IF=1
	context_fpu_save(t->ctx.fxsave_area);
}
