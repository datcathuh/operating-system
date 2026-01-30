#include "arch_setup.h"
#include "kmain.h"

void arch_x86_64_setup(uint64_t magic, void *mb_addr) {
    kmain(magic, mb_addr);
}
