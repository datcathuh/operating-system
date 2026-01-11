#include "memory.h"

void mem_set(void *dst, unsigned char value, size_t size) {
	__asm__ volatile("cld\n\t"
	                 "rep stosb"
	                 : "+D"(dst), "+c"(size)
	                 : "a"(value)
	                 : "memory");
}

void mem_copy(void *dst, const void *src, size_t n) {
	__asm__ volatile("cld\n"
	                 "rep movsb"
	                 : "+D"(dst), "+S"(src), "+c"(n)
	                 :
	                 : "memory");
}
