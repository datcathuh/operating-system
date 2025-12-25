# Introduction

Having fun implementing my own OS.

# Dependencies

## Fedora

Install the following:

dnf install nasm gcc make qemu
dnf install bochs bochs-gdb bochs-debugger
dnf install grub2-efi-x64-modules grub2-tools-extra

# Compiling and running

Use the following:

  make rebuild run


Some usefull links:

https://dev.to/frosnerd/series/9585
https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html


# Boot process

MBR is executed by BIOS  0x7c00
Stage2 is load at        0x0500
Kernel is loaded at     0x10000
Kernel is copied in to 0x100000

 00000h –  003FFh   IVT (Interrupt Vector Table)
 00400h –  004FFh   BIOS Data Area
 00500h –  07BFFh   Free RAM               29439 bytes
 07C00h –  07DFFh   Boot Sector (Stage 1)
 07E00h – 09FFFFh   Free RAM               623103 bytes
0A0000h – 0FFFFFh   VGA, ROM, BIOS
