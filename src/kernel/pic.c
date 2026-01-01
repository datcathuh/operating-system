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
