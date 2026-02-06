#include "arch/x86_64/arch_setup.h"
#include "arch/x86_64/pit.h"
#include "arch/x86_64/cpu/gdt.h"
#include "arch/x86_64/cpu/idt.h"
#include "arch/x86_64/cpu/ioapic.h"
#include "arch/x86_64/cpu/lapic.h"
#include "arch/x86_64/entry/acpi.h"
#include "arch/x86_64/entry/multiboot2.h"
#include "arch/x86_64/interrupt/irq_double_fault.h"
#include "arch/x86_64/interrupt/irq_gp.h"
#include "arch/x86_64/interrupt/irq_nm.h"
#include "arch/x86_64/interrupt/irq_page_fault.h"
#include "arch/x86_64/interrupt/irq_keyboard.h"
#include "arch/x86_64/interrupt/irq_timer.h"
#include "arch/x86_64/interrupt/irq_ud.h"
#include "kmain.h"
#include "memory.h"
#include "panic.h"
#include "pic.h"
#include "pit.h"
#include "serial.h"
#include "sleep.h"
#include "video/gop.h"
#include "video/video.h"
#include "video/bga.h"
#include "video/vga.h"

void arch_x86_64_setup(uint64_t magic, void *mb_addr) {
	/* No interrupts please. */
	__asm__ volatile("cli");

	/* We have no idea what stage2 or grub has configured for
	   us. Lets setup the Global Descriptor Table. This also
	   setups stack for double faults etc. */
	gdt_init();

	/* Setup paging so many of the below _init functions can map
	   memory used. */
	mem_page_init();

	/* Setup video. Not much is done at this stage since we havn't
	   checked if we should be doing VGA/BGA or use a framebuffer
	   provided by grub. */
	video_init();

	/* Setup the Interupt Descriptor Table (IDT). This is needed
	   since we need to register handler for a wide range of
	   interrupts from now on. */
	idt_init();

	irq_double_fault_register();
	irq_gp_register();
	irq_page_fault_register();
	irq_keyboard_register();
	irq_nm_register();
	irq_timer_register();
	irq_ud_register();

	if (magic == 0) {
		/* We have come to this point using MBR -> stage2 -> kernel.
		   No UEFI involved in this boot. We need to find ACPI in
		   memory etc. */
		serial_puts("kmain: legacy boot\n");

		/* During boot of a computer we set the default device
		   to be VGA with 80x25. When running virtual we will
		   most likely find a BGA device when we scan the PCI bus. */
		vga_init();
		struct video_device *dev = vga_device();
		video_set(dev);
		struct video_resolution res = {.width = 80, .height = 25, .bpp = 4};
		dev->resolution_set(dev, &res);

		uintptr_t acpi_address = acpi_legacy_find_address();
		if (!acpi_address) {
			panic("ACPI not found during Legacy boot\n");
		}
		acpi_parse((void *)acpi_address);
	} else if (magic == MULTIBOOT2_BOOTLOADER_MAGIC) {
		/* We are booting using UEFI. This means that VGA and BGA
		   arent available. UEFI has configured a framebuffer for
		   us that we can use until we are using a real graphics
		   driver.

		   This means that we blacklists the BGA driver so it won't
		   be initialized. This is common on virtual machines.
		*/
		serial_puts("kmain: multiboot2\n");
		pci_blacklist(&bga_identification);
		multiboot2_parse(mb_addr);

		void *acpi_rsdp = multiboot2_get_acpi_rsdp();
		if (acpi_rsdp) {
			acpi_parse(acpi_rsdp);
		}

		struct multiboot2_tag_framebuffer *fb = multiboot2_get_framebuffer();

		struct video_resolution res = {
			.width = fb->width, .height = fb->height, .bpp = fb->bpp};
		struct video_device *vd = gop_device((uint8_t *)fb->addr, &res);
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
	} else {
		/* Unknown magic number during boot. At this point it could be a
		   legacy system using multiboot 1. */
		panic("Unsupported boot method");
	}

	/* We disable the Programmable Interupt Controller (PIC) completely
	   since modern system have a different kind of routing done for
	   interrupts. */
	pic_disable();

	/* At this point we have parsed ACPI information and know at which
	   address the IOAPIC can be reached. Normally only one IOAPIC per
	   system unless you have a high end server. This IOAPIC is receiving
	   interrupts which then is routed to the LAPIC for further processing. */
	ioapic_init();

	/* Initialize the LAPIC (there is one for each logical CPU in a system.
	 */
	lapic_init();

	/* Route keyboard interrupt through the IOAPIC into the
	   LAPIC for the running CPU */
	/* TODO: This code isn't fully safe since we also need to check
	   ACPI and the GSI tables if the keyboard has been configured
	   for something else than GSI 1 */
	uint64_t keyboard_flags = IOAPIC_DM_FIXED | IOAPIC_DEST_PHYSICAL |
	                          IOAPIC_POLARITY_HIGH | IOAPIC_TRIGGER_EDGE |
	                          IOAPIC_UNMASKED;
	ioapic_set_irq(1, 0x21, lapic_get_id(), keyboard_flags);

	kmain(magic, mb_addr);
}
