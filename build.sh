#!/bin/bash

export PREFIX="$HOME/opt/cross"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"

BUILD_DIR=build
SRC_DIR=src

mkdir -p $BUILD_DIR

echo "Assembling boot sector..."
nasm -f elf32 $SRC_DIR/bootloader/boot.asm -o $BUILD_DIR/boot.o

echo "Compiling kernel..."
$TARGET-gcc -c $SRC_DIR/kernel/kernel.c -o $BUILD_DIR/kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

echo "Linking kernel..."
$TARGET-gcc -T $SRC_DIR/linker.ld -o $BUILD_DIR/kernel.bin -ffreestanding -O2 -nostdlib $BUILD_DIR/boot.o $BUILD_DIR/kernel.o -lgcc

echo "Creating ISO image..."
mkdir -p $BUILD_DIR/isodir/boot/grub
cp $BUILD_DIR/kernel.bin $BUILD_DIR/isodir/boot/kernel.bin

cat > $BUILD_DIR/isodir/boot/grub/grub.cfg << 'EOF'
menuentry "MyOS" {
    multiboot /boot/kernel.bin
    boot
}
EOF

find_grub_files() {
    local paths=(
        "/usr/lib/grub/i386-pc"
        "/usr/share/grub/i386-pc" 
        "/usr/lib/grub/i386-efi"
        "/usr/share/grub/i386-efi"
        "/usr/lib/grub/x86_64-efi"
        "/usr/share/grub/x86_64-efi"
    )
    
    for path in "${paths[@]}"; do
        if [ -f "$path/eltorito.img" ]; then
            echo "$path/eltorito.img"
            return 0
        fi
    done
    
    return 1
}

if command -v grub-mkrescue &> /dev/null; then
    echo "Using grub-mkrescue..."
    grub-mkrescue -o $BUILD_DIR/myos.iso $BUILD_DIR/isodir
else
    echo "grub-mkrescue not found, using alternative method..."
    
    ELTORITO_IMG=$(find_grub_files)
    
    if [ -n "$ELTORITO_IMG" ]; then
        echo "Found GRUB boot image: $ELTORITO_IMG"
        cp "$ELTORITO_IMG" $BUILD_DIR/
        xorriso -as mkisofs -R -b eltorito.img -no-emul-boot -boot-load-size 4 -boot-info-table -o $BUILD_DIR/myos.iso $BUILD_DIR/isodir
    else
        echo "Error: Could not find eltorito.img"
        echo "Searching for alternative GRUB boot images..."
        
        BOOT_IMAGES=$(find /usr -name "*.img" 2>/dev/null | grep -E "(grub|boot|cdboot)" | head -5)
        if [ -n "$BOOT_IMAGES" ]; then
            echo "Found these potential boot images:"
            echo "$BOOT_IMAGES"
            echo "Trying the first one..."
            FIRST_IMG=$(echo "$BOOT_IMAGES" | head -1)
            cp "$FIRST_IMG" $BUILD_DIR/eltorito.img
            xorriso -as mkisofs -R -b eltorito.img -no-emul-boot -boot-load-size 4 -boot-info-table -o $BUILD_DIR/myos.iso $BUILD_DIR/isodir
        else
            echo "No boot images found. Creating simple ISO without boot sector..."
            genisoimage -R -b boot/grub/grub.cfg -no-emul-boot -boot-load-size 4 -boot-info-table -o $BUILD_DIR/myos.iso $BUILD_DIR/isodir
        fi
    fi
fi

echo "Build complete! ISO image: $BUILD_DIR/myos.iso"