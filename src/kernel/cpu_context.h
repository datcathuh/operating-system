#pragma once

#include "types.h"
#include "arch/x86_64/cpu/interrupt.h"

#ifdef __cplusplus
extern "C" {
#endif

struct cpu_context {
	uint64_t r15, r14, r13, r12;
	uint64_t r11, r10, r9, r8;
	uint64_t rsi, rdi, rbp, rdx, rcx, rbx, rax;

	/* Hardware saved registers */
	struct interrupt_frame frame;
	uint8_t fxsave_area[512] __attribute__((aligned(16)));
};

void context_fpu_save(uint8_t *fxsave_area);
void context_capture(struct cpu_context *old);
void context_restore(struct cpu_context *newctx);
void context_switch(struct cpu_context *oldctx, struct cpu_context *newctx);

#ifdef __cplusplus
}
#endif
