#pragma once

#include "pci.h"
#include "video.h"

extern struct pci_identification bga_identification;
extern struct pci_device_driver bga_driver;

struct video_device *bga_device();
