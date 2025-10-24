#pragma once

#include "types.h"

struct pci_device_driver;

struct pci_identification {
	uint16_t vendor;
	uint16_t device;
};

struct pci_device {
	uint16_t bus;
	uint8_t slot;
	uint8_t func;
	struct pci_identification identification;
	uint8_t class_id;
	uint8_t subclass;

	uint32_t bar[6];

	struct pci_device_driver *driver;
};

struct pci_device_driver {
	bool (*initialize)(struct pci_device_driver* driver, struct pci_device *device);
	bool (*unload)(struct pci_device_driver* driver, struct pci_device *device);
};

typedef void(*pci_enumerate_cb)(struct pci_device*);

/* Walk the PCI bus and call the callback for each found device */
void pci_enumerate(pci_enumerate_cb cb);

/* Scans the PCI bus and build a list of devices. */
void pci_build_device_tree(void);
void pci_device_tree(struct pci_device **devices, uint16_t *device_count);
