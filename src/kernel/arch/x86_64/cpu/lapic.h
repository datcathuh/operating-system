#pragma once

#include "types.h"

#define LAPIC_TIMER_VECTOR 0x40

void lapic_init(void);
uint8_t lapic_get_id(void);
void lapic_eoi(void);
uint32_t lapic_calibrate_timer(void);
void lapic_timer_setup(uint8_t vector, uint32_t frequency_hz);
