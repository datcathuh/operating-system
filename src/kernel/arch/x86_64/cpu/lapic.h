#pragma once

#include "types.h"

#define LAPIC_TIMER_VECTOR 0x40

void lapic_init(void);
uint8_t lapic_get_id(void);
void lapic_eoi(void);
