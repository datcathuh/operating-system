#pragma once

#include "types.h"

struct cpu_context {
    uint64_t r15, r14, r13, r12;
    uint64_t r11, r10, r9,  r8;
    uint64_t rsi, rdi, rbp, rdx, rcx, rbx, rax;
    uint64_t rsp;
    uint64_t rip;
    uint64_t rflags;
};

void context_switch(struct cpu_context *old, struct cpu_context *new);
