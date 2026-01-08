#pragma once

#include "types.h"

void lapic_init(void);
uint8_t lapic_get_id(void);
void lapic_eoi(void);
