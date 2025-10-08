#pragma once

#include <stdbool.h>
#include <stdint.h>

void irq_keyboard_register(void);
bool irq_keyboard_consume_key(uint8_t *sc);
