#include "serial.h"
#include "types.h"

extern uint32_t legacy_entry;

/* Print helper */
static inline void print_check(const char *name, bool ok)
{
	serial_puts(name);
	serial_puts(ok ? " OK" : " FAIL");
	serial_puts("\n");
}

/* Main check function */
void print_diagnostics_sse(void) {
	serial_puts("SSE Diagnostics:\n");

    uint64_t cr0;
    uint64_t cr4;
	uint64_t rflags;

    __asm__ volatile ("mov %%cr0, %0" : "=r"(cr0));
    __asm__ volatile ("mov %%cr4, %0" : "=r"(cr4));
    __asm__ volatile (
        "pushfq\n"
        "pop %0" : "=r"(rflags) : : "memory"
    );

    bool cr0_em        = (cr0 & (1ULL << 2)) == 0;     /* EM = 0 */
    bool cr0_mp        = (cr0 & (1ULL << 1)) != 0;     /* MP = 1 */
	bool cr0_ts        = (cr0 & (1ULL << 3)) == 0;     /* TS = 0 */
    bool cr4_osfxsr    = (cr4 & (1ULL << 9)) != 0;     /* OSFXSR */
    bool cr4_osxmmexc  = (cr4 & (1ULL << 10)) != 0;    /* OSXMMEXCPT */
    bool rflags_ok     = (rflags == 0x202);

    print_check("CR0.EM == 0", cr0_em);
    print_check("CR0.MP == 1", cr0_mp);
    print_check("CR0.TS == 0", cr0_ts);
    print_check("CR4.OSFXSR == 1", cr4_osfxsr);
    print_check("CR4.OSXMMEXCPT == 1", cr4_osxmmexc);
    print_check("RFLAGS == 0x202", rflags_ok);
	serial_put_hex64(rflags);

    serial_puts("\n");
}

void print_diagnostics(void) {
	uint32_t esp, ebp, cs, ds, ss;
	struct {
		uint16_t limit;
		uint32_t base;
	} __attribute__((packed)) gdtr, idtr;

	__asm__ volatile("mov %%esp, %0" : "=r"(esp));
	__asm__ volatile("mov %%ebp, %0" : "=r"(ebp));
	__asm__ volatile("mov %%cs, %0" : "=r"(cs));
	__asm__ volatile("mov %%ds, %0" : "=r"(ds));
	__asm__ volatile("mov %%ss, %0" : "=r"(ss));

	__asm__ volatile("sgdt %0" : "=m"(gdtr));
	__asm__ volatile("sidt %0" : "=m"(idtr));

	serial_puts("Diagnostics:\r\n");
	serial_puts("  ESP = ");
	serial_put_hex32(esp);
	serial_puts("\n");
	serial_puts("  EBP = ");
	serial_put_hex32(ebp);
	serial_puts("\n");
	serial_puts("  CS  = ");
	serial_put_hex32(cs);
	serial_puts("\n");
	serial_puts("  DS  = ");
	serial_put_hex32(ds);
	serial_puts("\n");
	serial_puts("  SS  = ");
	serial_put_hex32(ss);
	serial_puts("\n");
	serial_puts("  GDTR base = ");
	serial_put_hex32(gdtr.base);
	serial_puts("\n");
	serial_puts("  IDTR base = ");
	serial_put_hex32(idtr.base);
	serial_puts("\n");
	serial_puts("  legacy_entry()   = ");
	serial_put_hex32((uintptr_t)&legacy_entry);
	serial_puts("\n");
	serial_puts("\n");
}
