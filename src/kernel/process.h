#pragma once

#include "types.h"

struct cpu_state {
	uint64_t r15, r14, r13, r12;
	uint64_t r11, r10, r9, r8;
	uint64_t rsi, rdi, rbp, rdx;
	uint64_t rcx, rbx, rax;

	uint64_t rip;
	uint64_t cs;
	uint64_t rflags;
	uint64_t rsp;
	uint64_t ss;
};

typedef enum {
	PROC_READY,
	PROC_RUNNING,
	PROC_BLOCKED,
	PROC_EXITED
} proc_state_t;

struct process {
	uint64_t pid;
	proc_state_t state;
	uint64_t cr3; // Page table base (PML4 physical address)
	struct cpu_state cpu;
	void *kernel_stack;
	uint64_t kernel_stack_top;

	void *script_ptr;
	uint64_t script_size;

	struct process *next;
};
