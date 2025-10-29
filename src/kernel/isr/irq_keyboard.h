#pragma once

#include "types.h"

void irq_keyboard_register(void);
uint32_t irq_keyboard_count(void);
bool irq_keyboard_consume_key(uint8_t *sc);
