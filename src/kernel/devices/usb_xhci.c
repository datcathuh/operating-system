#include "usb_xhci.h"
#include "string.h"

struct pci_identification usb_xhci_identification = {.vendor = 0x1b36,
                                                     .device = 0x000d};

static void usb_xhci_driver_description(struct pci_device_driver * /*driver*/,
                                        struct pci_device * /*device*/,
                                        char *buffer, uint16_t size) {
	str_copy(buffer, size, "QEMU XHCI USB Host Controller");
}

static bool usb_xhci_driver_initialize(struct pci_device_driver * /*driver*/,
                                       struct pci_device * /*device*/) {
	return true;
}

static bool usb_xhci_driver_unload(struct pci_device_driver * /*driver*/,
                                   struct pci_device * /*device*/) {
	return true;
}

struct pci_device_driver usb_xhci_driver = {
	.description = usb_xhci_driver_description,
	.initialize = usb_xhci_driver_initialize,
	.unload = usb_xhci_driver_unload};
