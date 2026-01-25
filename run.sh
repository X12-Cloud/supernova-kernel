#!/usr/bin/fish

# 1. Compile the project
make clean
if make
    # 2. Prepare the ISO structure
    mkdir -p iso_root/boot/grub
    cp supernova.bin iso_root/boot/

    # 3. Create the GRUB config (Fish style)
    printf "set timeout=0\nset default=0\n\nmenuentry \"Supernova\" {\n    multiboot /boot/supernova.bin\n    boot\n}\n" > iso_root/boot/grub/grub.cfg

    # 4. Generate the ISO
    if grub-mkrescue -o supernova.iso iso_root
        echo "Successfully built supernova.iso"
        # 5. Launch QEMU
        qemu-system-x86_64 -cdrom supernova.iso -display sdl
    else
        echo "Error: grub-mkrescue failed"
    end
else
    echo "Error: Make failed"
end
