# Introduction

Having fun implementing my own OS.

# Dependencies

## Fedora

Install the following:

dnf install nasm gcc make qemu

# Compiling and running

Use the following:

  make rebuild run


Some usefull links:

https://dev.to/frosnerd/series/9585
https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html


# Boot process

MBR is executed by BIOS  0x7c00
Stage2 is load at        0x1000
Kernel is loaded at     0x10000
Kernel is copied in to 0x100000
