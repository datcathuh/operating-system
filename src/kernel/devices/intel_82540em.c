#include "intel_82540em.h"
#include "string.h"

struct pci_identification intel_82540em_identification = {.vendor = 0x8086,
                                                          .device = 0x100e};

static void
intel_82540em_driver_description(struct pci_device_driver * /*driver*/,
                                 struct pci_device * /*device*/, char *buffer,
                                 uint16_t size) {
	str_copy(buffer, size, "82540EM Gigabit Ethernet Controller");
}

static bool
intel_82540em_driver_initialize(struct pci_device_driver * /*driver*/,
                                struct pci_device * /*device*/) {
	return true;
}

static bool intel_82540em_driver_unload(struct pci_device_driver * /*driver*/,
                                        struct pci_device * /*device*/) {
	return true;
}

struct pci_device_driver intel_82540em_driver = {
	.description = intel_82540em_driver_description,
	.initialize = intel_82540em_driver_initialize,
	.unload = intel_82540em_driver_unload};
