#pragma once

#include <stdint.h>

struct pci_device {
	uint16_t bus;
	uint8_t slot;
	uint8_t func;
	uint16_t vendor;
	uint16_t device;
	uint8_t class_id;
	uint8_t subclass;

	uint32_t bar[6];
};

typedef void(*pci_enumerate_cb)(struct pci_device*);

void pci_enumerate(pci_enumerate_cb cb);
