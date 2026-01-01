/*

  Programmable Interupt Controller

*/
#include "io.h"

#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1

#define ICW1_INIT 0x11
#define ICW4_8086 0x01

void pic_disable(void) {
    // Mask all interrupts on both PICs
    io_outb(PIC1_DATA, 0xFF); // all IRQ0-7 masked
    io_outb(PIC2_DATA, 0xFF); // all IRQ8-15 masked

    // Optionally send EOI to both PICs to clear any pending IRQs
    io_outb(PIC1_COMMAND, 0x20); // End Of Interrupt command
    io_outb(PIC2_COMMAND, 0x20); // End Of Interrupt command
}

void pic_remap(void) {
	// IRQ0-7 → 0x20-0x27
	// IRQ8-15 → 0x28-0x2F

	unsigned char a1 = io_inb(PIC1_DATA);
	unsigned char a2 = io_inb(PIC2_DATA);

	io_outb(PIC1_COMMAND, ICW1_INIT);
	io_outb(PIC2_COMMAND, ICW1_INIT);
	io_outb(PIC1_DATA, 0x20); // PIC1 vector offset
	io_outb(PIC2_DATA, 0x28); // PIC2 vector offset
	io_outb(PIC1_DATA, 0x04); // tell PIC1 about PIC2 at IRQ2
	io_outb(PIC2_DATA, 0x02); // tell PIC2 its cascade identity
	io_outb(PIC1_DATA, ICW4_8086);
	io_outb(PIC2_DATA, ICW4_8086);

	// Restore masks
	io_outb(PIC1_DATA, a1);
	io_outb(PIC2_DATA, a2);
}
