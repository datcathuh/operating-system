#include "string.h"

bool str_append_char(char *buf, const char c, const int buf_size) {
	int size = str_length(buf);
	if (size < buf_size - 2) {
		buf[size] = c;
		buf[size + 1] = 0;
		return true;
	}
	return false;
}

int str_length(const char *buf) {
	int size = 0;
	while (*buf) {
		size++;
		buf++;
	}
	return size;
}

int str_compare(const char *s1, const char *s2) {
	while (*s1 && (*s1 == *s2)) {
		s1++;
		s2++;
	}
	return (unsigned char)*s1 - (unsigned char)*s2;
}

bool str_from_uint32(char *buf, int size, uint32_t value) {
	if (size < 2) {
		return false;
	}

	if (value == 0) {
		buf[0] = '0';
		buf[1] = 0;
		return true;
	}

	int i = 0;
	while (value > 0) {
		if (i == size - 1) {
			return false;
		}
		int digit = value % 10;
		buf[i++] = '0' + digit;
		value /= 10;
	}
	buf[i] = 0;
	return true;
}

bool str_hex_from_uint32(char *buf, int size, uint32_t value) {
	const char *hex_digits = "0123456789ABCDEF";
	bool started = false;

	int j = 0;
	for (int i = 7; i >= 0; i--) {
		if (j - 1 == size) {
			return false;
		}

		uint8_t nibble = (value >> (i * 4)) & 0xF;
		if (nibble != 0 || started || i == 0) {
			buf[j] = hex_digits[nibble];
			started = true;
			j++;
		}
	}
	buf[j] = 0;
	return true;
}

bool str_copy(char *dst, int size, const char *src) {
	if (str_length(src) + 1 > size) {
		return false;
	}

	uint32_t i = 0;
	while (src[i] != 0) {
		dst[i] = src[i];
		i++;
	}
	dst[i] = 0;
	return true;
}
