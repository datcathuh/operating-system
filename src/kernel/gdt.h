#pragma once

/* GDT selectors (index << 3) */
#define GDT_KERNEL_CODE  0x08
#define GDT_KERNEL_DATA  0x10
#define GDT_TSS          0x18  /* occupies 0x18 and 0x20 */

void gdt_init(void);
