#include "idt.h"
#include "irq_keyboard.h"
#include "irq_timer.h"
#include "kshell.h"
#include "kshell_bga.h"
#include "kshell_help.h"
#include "kshell_julia.h"
#include "kshell_mandelbrot.h"
#include "kshell_shutdown.h"
#include "pci.h"
#include "pic.h"
#include "serial.h"
#include "string.h"
#include "video/video.h"

void pci_cb(struct pci_device *dev) {
	char vendorid[10];
	char devid[10];
	str_hex_from_uint32(vendorid, 10, dev->vendor);
	str_hex_from_uint32(devid, 10, dev->device);
	serial_puts("PCI device found: ");
	serial_puts(vendorid);
	serial_puts(":");
	serial_puts(devid);
	serial_puts("\n");
}

void kmain(void) {
	__asm__ volatile("cli");
	pci_enumerate(pci_cb);
	pic_remap();
	idt_install();
	irq_keyboard_register();
	irq_timer_register();
	__asm__ volatile("sti");

	video_init();

	kshell_init();
	kshell_bga_register();
	kshell_help_register();
	kshell_julia_register();
	kshell_mandelbrot_register();
	kshell_shutdown_register();

	kshell();
}
