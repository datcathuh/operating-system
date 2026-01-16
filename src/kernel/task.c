#include "memory.h"
#include "task.h"

void task_create(struct task *t, void (*entry)(void)) {
    t->kernel_stack = mem_page_alloc();
    uint64_t stack_top = (uint64_t)t->kernel_stack + MEM_PAGE_SIZE;

    mem_set(&t->ctx, 0, sizeof(t->ctx));
    t->ctx.rip = (uint64_t)entry;
    t->ctx.rsp = stack_top;
    t->ctx.rflags = 0x202; // IF=1
}
