#pragma once

#include "types.h"

void serial_put_dec(uint64_t val);
void serial_put_hex8(uint8_t val);
void serial_put_hex16(uint16_t val);
void serial_put_hex32(uint32_t val);
void serial_put_hex64(uint64_t val);
void serial_putc(char c);
void serial_puts(const char *s);
