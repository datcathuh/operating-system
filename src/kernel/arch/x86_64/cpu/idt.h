#pragma once

#include "types.h"

void idt_gate_set(int n, void (*handler)());
void idt_init();
