#include "interrupt.h"

void interrupt_stop() {
    __asm__ volatile("cli");
}

void interrupt_resume() {
    __asm__ volatile("sti");
}
