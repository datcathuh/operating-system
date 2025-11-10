#pragma once

#include "types.h"

static inline uint8_t io_inb(uint16_t port) {
    uint8_t value;
    __asm__ __volatile__("inb %1, %0" : "=a"(value) : "dN"(port));
    return value;
}

static inline uint16_t io_inw(uint16_t port) {
    uint16_t ret;
    __asm__ volatile ("inw %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline uint32_t io_inl(uint16_t port) {
    uint32_t ret;
    __asm__ volatile ("inl %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void io_outb(uint16_t port, uint8_t val) {
	__asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port) : "memory");
}

static inline void io_outw(uint16_t port, uint16_t val) {
    __asm__ volatile ("outw %0, %1" : : "a"(val), "Nd"(port));
}

static inline void io_outl(uint16_t port, uint32_t val) {
    __asm__ volatile ("outl %0, %1" : : "a"(val), "Nd"(port));
}

/* read indexed port (index->data pair) */
static inline uint8_t io_read_indexed(uint16_t idx_port, uint16_t data_port, uint8_t idx) {
    __asm__ volatile ("outb %0, %1" : : "a"(idx), "Nd"(idx_port));
    uint8_t v;
    __asm__ volatile ("inb %1, %0" : "=a"(v) : "Nd"(data_port));
    return v;
}
