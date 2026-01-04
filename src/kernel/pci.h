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
	void (*description)(struct pci_device_driver *driver,
	                    struct pci_device *device, char *buffer, uint16_t size);
	bool (*initialize)(struct pci_device_driver *driver,
	                   struct pci_device *device);
	bool (*unload)(struct pci_device_driver *driver, struct pci_device *device);
};

/* Scans the PCI bus and build a list of devices. */
void pci_init(void);

typedef void (*pci_enumerate_cb)(struct pci_device *);

uint8_t pci_cfg_read8(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
uint16_t pci_cfg_read16(uint8_t bus, uint8_t slot, uint8_t func,
                        uint8_t offset);
uint32_t pci_config_read32(uint8_t bus, uint8_t slot, uint8_t func,
                           uint8_t offset);
void pci_cfg_write32(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset,
                     uint32_t val);

/* Walk the PCI bus and call the callback for each found device */
void pci_enumerate(pci_enumerate_cb cb);

void pci_device_tree(struct pci_device **devices, uint16_t *device_count);
void pci_debug_dump(void);

void pci_blacklist(struct pci_identification *devid);
bool pci_blacklist_check(struct pci_identification *devid);
