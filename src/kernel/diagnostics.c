#include "serial.h"
#include "types.h"

extern uint32_t _start;

void print_diagnostics(void) {
    uint32_t esp, ebp, cs, ds, ss;
    struct {
		uint16_t limit;
		uint32_t base;
	} __attribute__((packed)) gdtr, idtr;

    __asm__ volatile ("mov %%esp, %0" : "=r"(esp));
    __asm__ volatile ("mov %%ebp, %0" : "=r"(ebp));
    __asm__ volatile ("mov %%cs, %0" : "=r"(cs));
    __asm__ volatile ("mov %%ds, %0" : "=r"(ds));
    __asm__ volatile ("mov %%ss, %0" : "=r"(ss));

    __asm__ volatile ("sgdt %0" : "=m"(gdtr));
    __asm__ volatile ("sidt %0" : "=m"(idtr));

    serial_puts("Diagnostics:\r\n");
    serial_puts("  ESP = "); serial_put_hex32(esp); serial_puts("\r\n");
    serial_puts("  EBP = "); serial_put_hex32(ebp); serial_puts("\r\n");
    serial_puts("  CS  = "); serial_put_hex32(cs);  serial_puts("\r\n");
    serial_puts("  DS  = "); serial_put_hex32(ds);  serial_puts("\r\n");
    serial_puts("  SS  = "); serial_put_hex32(ss);  serial_puts("\r\n");
    serial_puts("  GDTR base = "); serial_put_hex32(gdtr.base); serial_puts("\r\n");
    serial_puts("  IDTR base = "); serial_put_hex32(idtr.base); serial_puts("\r\n");
    serial_puts("  _start()   = "); serial_put_hex32((uint32_t)&_start); serial_puts("\r\n");
}
