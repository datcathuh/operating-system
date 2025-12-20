#include "kshell.h"
#include "kshell_sysinfo.h"
#include "pci.h"
#include "string.h"
#include "video/video.h"

static void kshell_sysinfo_cb() {
	struct terminal *terminal = video_current()->terminal;
	terminal->print(terminal, "PCI devices:\n");

	struct pci_device *devices;
	uint16_t count;
	pci_device_tree(&devices, &count);
	for (uint16_t i = 0; i < count; i++) {
		struct pci_device *dev = &devices[i];
		char vendorid[10];
		char devid[10];
		str_hex_from_uint32(vendorid, 10, dev->identification.vendor);
		str_hex_from_uint32(devid, 10, dev->identification.device);
		terminal->print(terminal, "  ");
		terminal->print(terminal, vendorid);
		terminal->print(terminal, ":");
		terminal->print(terminal, devid);

		char description[100];
		if (dev->driver) {
			terminal->print(terminal, " ");
			dev->driver->description(dev->driver, dev, description, 100);
			terminal->print(terminal, description);
		}
		terminal->print(terminal, "\n");
	}
}

void kshell_sysinfo_register() {
	struct kshell_command cmd = {.name = "sysinfo",
	                             .callback = kshell_sysinfo_cb};
	kshell_register_command(&cmd);
}
