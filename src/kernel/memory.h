#pragma once

#include "types.h"

void mem_set(void *ptr, unsigned char value, int size);
void mem_copy(void *dst, const void *src, int size);

void *mem_page_alloc(size_t npages);
void mem_page_free(void *addr, size_t npages);

void *mem_alloc(size_t size);
void *mem_alloc_zero(size_t size);
void mem_free(void *ptr);
size_t mem_alloc_usable_size(void *ptr);
