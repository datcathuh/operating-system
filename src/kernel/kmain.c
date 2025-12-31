#include "acpi.h"
#include "boot/multiboot2.h"
#include "diagnostics.h"
#include "idt.h"
#include "isr/irq_double_fault.h"
#include "isr/irq_gp.h"
#include "isr/irq_page_fault.h"
#include "isr/irq_keyboard.h"
#include "isr/irq_timer.h"
#include "kshell/kshell.h"
#include "kshell/kshell_bga.h"
#include "kshell/kshell_help.h"
#include "kshell/kshell_julia.h"
#include "kshell/kshell_mandelbrot.h"
#include "kshell/kshell_shutdown.h"
#include "kshell/kshell_snake.h"
#include "kshell/kshell_sysinfo.h"
#include "kshell/kshell_tetris.h"
#include "lapic.h"
#include "memory.h"
#include "serial.h"
#include "pci.h"
#include "pic.h"
#include "video/gop.h"
#include "video/video.h"
#include "video/bga.h"

void kmain(uint64_t magic, void* mb_addr) {
	mem_page_init();
	__asm__ volatile("cli");
	video_init();
	serial_puts("Video init\n");

	if(magic == 0) {
		serial_puts("kmain: legacy boot\n");
	}
	else if(magic == MULTIBOOT2_BOOTLOADER_MAGIC) {
		/* We are booting using UEFI. This means that VGA and BGA
		   arent available. UEFI has configured a framebuffer for
		   us that we can use until we are using a real graphics
		   driver.

		   This means that we blacklists the BGA driver so it won't
		   be initialized.
		*/
	  serial_puts("kmain: multiboot2\n");
		pci_blacklist(&bga_identification);
		multiboot2_parse(mb_addr);
		struct multiboot2_tag_framebuffer *fb = multiboot2_get_framebuffer();

		struct video_resolution res = {
			.width = fb->width,
			.height = fb->height,
			.bpp = fb->bpp
		};
		struct video_device *vd = gop_device((uint8_t*)fb->addr, &res);
		video_set(vd);
		serial_puts("Video GOP @");
		serial_put_hex64(fb->addr);
		serial_puts(" ");
		serial_put_hex64(res.width);
		serial_puts("x");
		serial_put_hex64(res.height);
		serial_puts(" bpp: ");
		serial_put_hex64(res.bpp);
		serial_puts("\n");
	}

	lapic_default_init();
	serial_puts("LAPIC init\n");
	acpi_init();
	serial_puts("ACPI init\n");
	pci_build_device_tree();
	pci_debug_dump();
	pic_remap();
	idt_install();
	irq_double_fault_register();
	irq_gp_register();
	irq_page_fault_register();
	irq_keyboard_register();
	irq_timer_register();
	__asm__ volatile("sti");

	print_diagnostics();

	kshell_init();
	kshell_bga_register();
	kshell_help_register();
	kshell_julia_register();
	kshell_mandelbrot_register();
	kshell_shutdown_register();
	kshell_snake_register();
	kshell_sysinfo_register();
	kshell_tetris_register();

	mem_page_debug_dump();

	kshell();
}
