#include "gdt.h"
#include "memory.h"
#include "types.h"

struct gdt_entry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t  base_mid;
    uint8_t  access;
    uint8_t  granularity;
    uint8_t  base_high;
} __attribute__((packed));

struct gdt_tss_entry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t  base_mid;
    uint8_t  access;
    uint8_t  granularity;
    uint8_t  base_high;
    uint32_t base_upper;
    uint32_t reserved;
} __attribute__((packed));

struct gdt_ptr {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

struct tss {
    uint32_t reserved0;
    uint64_t rsp0;
    uint64_t rsp1;
    uint64_t rsp2;
    uint64_t reserved1;
    uint64_t ist[7];
    uint64_t reserved2;
    uint16_t reserved3;
    uint16_t io_map_base;
} __attribute__((packed));

static struct {
    struct gdt_entry     null;
    struct gdt_entry     kernel_code;
    struct gdt_entry     kernel_data;
    struct gdt_tss_entry tss;
} __attribute__((packed)) gdt;

static struct gdt_ptr gdt_descriptor;
static struct tss kernel_tss;
extern uint8_t stack_df_top[];

static struct gdt_entry make_gdt_entry(uint8_t access, uint8_t flags) {
    struct gdt_entry e = {0};
    e.access = access;
    e.granularity = flags;
    return e;
}

static void write_tss_descriptor(struct gdt_tss_entry *e, struct tss *tss) {
    uint64_t base = (uint64_t)tss;
    uint32_t limit = sizeof(struct tss) - 1;

    e->limit_low   = limit & 0xFFFF;
    e->base_low    = base & 0xFFFF;
    e->base_mid    = (base >> 16) & 0xFF;
    e->access      = 0x89;   /* Present | Executable | TSS */
    e->granularity = (limit >> 16) & 0x0F;
    e->base_high   = (base >> 24) & 0xFF;
    e->base_upper  = (base >> 32);
    e->reserved    = 0;
}

void gdt_init(void) {
	mem_set(&kernel_tss, 0, sizeof(struct tss));

    kernel_tss.rsp0 = 0;
    kernel_tss.ist[1] = (uint64_t) stack_df_top;  /* Double fault stack goes here */
    kernel_tss.io_map_base = sizeof(struct tss);

    gdt.null        = make_gdt_entry(0, 0);
    gdt.kernel_code = make_gdt_entry(0x9A, 0x20); /* 64-bit code */
    gdt.kernel_data = make_gdt_entry(0x92, 0x00); /* data */

    write_tss_descriptor(&gdt.tss, &kernel_tss);

    /* Load GDT */
    gdt_descriptor.limit = sizeof(gdt) - 1;
    gdt_descriptor.base  = (uint64_t)&gdt;

    __asm__ volatile (
        "lgdt %0\n"
        "pushq %[cs]\n"
        "leaq 1f(%%rip), %%rax\n"
        "pushq %%rax\n"
        "lretq\n"
        "1:\n"
        "mov %[ds], %%ds\n"
        "mov %[ds], %%es\n"
        "mov %[ds], %%ss\n"
        :
        : "m"(gdt_descriptor),
          [cs]"i"(GDT_KERNEL_CODE),
          [ds]"r"(GDT_KERNEL_DATA)
        : "rax", "memory"
    );

    /* Load TSS */
    __asm__ volatile ("ltr %0" : : "r"((uint16_t)GDT_TSS));
}
