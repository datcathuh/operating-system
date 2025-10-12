#pragma once

#include <stdint.h>

bool str_append_char(char *buf, const char c, const int size);
int str_length(const char *buf);
int str_compare(const char *s1, const char *s2);
bool str_from_uint32(char *buf, int size, uint32_t value);
bool str_hex_from_uint32(char *buf, int size, uint32_t value);
