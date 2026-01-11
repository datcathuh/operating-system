#include "memory.h"

void mem_set(void *ptr, unsigned char value, int size) {
	unsigned char *p = ptr;
	while (size--) {
		*p++ = value;
	}
}

void mem_copy(void *dst, const void *src, size_t n) {
	__asm__ volatile("cld\n"
	                 "rep movsb"
	                 : "+D"(dst), "+S"(src), "+c"(n)
	                 :
	                 : "memory");
}
