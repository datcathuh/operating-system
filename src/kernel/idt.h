#pragma once

#include "types.h"

void idt_gate_set(int n, uintptr_t handler);
void idt_install();
