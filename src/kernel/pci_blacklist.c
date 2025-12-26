#include "pci.h"

#define PCI_BLACKLIST_LENGTH 256
static struct pci_identification _blacklist[PCI_BLACKLIST_LENGTH] = {0};
static uint16_t _blacklist_count = 0;

void pci_blacklist(struct pci_identification *devid) {
	for (int i = 0; i < PCI_BLACKLIST_LENGTH; i++) {
		if (_blacklist[i].vendor == 0 && _blacklist[i].device == 0) {
			/* Free slot found. Lets blacklist the device. */
			_blacklist[i].vendor = devid->vendor;
			_blacklist[i].device = devid->device;
			_blacklist_count++;
			return;
		}
	}
}

bool pci_blacklist_check(struct pci_identification *devid) {
	for (int i = 0; i < PCI_BLACKLIST_LENGTH; i++) {
		if (_blacklist[i].vendor == devid->vendor &&
		    _blacklist[i].device == devid->device) {
			return true;
		}
	}
	return false;
}
