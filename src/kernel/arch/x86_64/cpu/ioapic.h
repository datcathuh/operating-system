#pragma once

#include "types.h"

#define IOAPIC_DM_FIXED (0 << 8)
#define IOAPIC_DM_LOWEST (1 << 8)

#define IOAPIC_DEST_PHYSICAL (0 << 11)
#define IOAPIC_DEST_LOGICAL (1 << 11)

#define IOAPIC_POLARITY_HIGH (0 << 13)
#define IOAPIC_POLARITY_LOW (1 << 13)

#define IOAPIC_TRIGGER_EDGE (0 << 15)
#define IOAPIC_TRIGGER_LEVEL (1 << 15)

#define IOAPIC_MASKED (1 << 16)
#define IOAPIC_UNMASKED (0 << 16)

void ioapic_init(void);
void ioapic_set_irq(uint32_t gsi, uint8_t vector, uint8_t lapic_id,
                    uint64_t flags);
