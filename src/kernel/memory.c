#include "memory.h"

void mem_set(void *ptr, unsigned char value, int size) {
	unsigned char *p = ptr;
    while (size--) {
        *p++ = value;
    }
}

void mem_copy(void *dst, const void *src, int size) {
	unsigned char *d = dst;
    const unsigned char *s = src;
    while (size--) {
        *d++ = *s++;
    }
}
