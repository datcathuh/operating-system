#include "memory.h"

void mem_set(void *dst, unsigned char value, size_t size) {
	__asm__ volatile("cld\n\t"
	                 "rep stosb"
	                 : "+D"(dst), "+c"(size)
	                 : "a"(value)
	                 : "memory");
}

static inline void mem_set_qword_stosq(void *dst, uint64_t value, uint64_t qword_count) {
	__asm__ volatile("rep stosq"
	                 : "+D"(dst), "+c"(qword_count)
	                 : "a"(value)
	                 : "memory");
}

void mem_set_qword(void *dst, uint64_t value, size_t size) {
	uint64_t qwords = size / 8;
    uint64_t bytes  = size % 8;

    mem_set_qword_stosq(dst, value, qwords);

    uint8_t *tail = (uint8_t *)dst + qwords * 8;
    for (uint64_t i = 0; i < bytes; i++)
        tail[i] = (uint8_t)value;
}

void mem_copy(void *dst, const void *src, size_t n) {
	__asm__ volatile("cld\n"
	                 "rep movsb"
	                 : "+D"(dst), "+S"(src), "+c"(n)
	                 :
	                 : "memory");
}
