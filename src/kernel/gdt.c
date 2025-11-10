#include <types.h>

struct __attribute__((packed)) gdt_entry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t  base_middle;
    uint8_t  access;
    uint8_t  granularity;
    uint8_t  base_high;
};

struct __attribute__((packed)) gdt_ptr {
    uint16_t limit;
    uint32_t base;
};

static struct gdt_entry gdt[3];
static struct gdt_ptr   gp;

static void gdt_set_gate(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran)
{
    gdt[num].limit_low    = limit & 0xFFFF;
    gdt[num].base_low     = base  & 0xFFFF;
    gdt[num].base_middle  = (base >> 16) & 0xFF;
    gdt[num].access       = access;
    gdt[num].granularity  = ((limit >> 16) & 0x0F) | (gran & 0xF0);
    gdt[num].base_high    = (base >> 24) & 0xFF;
}

extern void gdt_flush(uint32_t gp_addr); /* implemented in assembly below */

void gdt_install(void)
{
    gdt_set_gate(0, 0, 0, 0, 0);                /* null */
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); /* code */
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); /* data */

    gp.limit = sizeof(gdt) - 1;
    gp.base  = (uintptr_t)&gdt;

    gdt_flush((uintptr_t)&gp); /* assembly helper to lgdt + far jump */
}
