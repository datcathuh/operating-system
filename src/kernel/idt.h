#pragma once

#include <stdint.h>

void idt_gate_set(int n, uint32_t handler);
void idt_install();
