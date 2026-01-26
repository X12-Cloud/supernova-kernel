#!/usr/bin/fish

set MODE $argv[1]

# Function to just launch QEMU
function launch_qemu
    if test -f supernova.iso
        qemu-system-x86_64 -cdrom supernova.iso -display sdl -hda disk.img
    else
        echo "Error: supernova.iso not found. Run with -clean first."
    end
end

# Function to build everything
function build_all
    make clean
    if make
        mkdir -p iso_root/boot/grub
        cp supernova.bin iso_root/boot/
        printf "set timeout=0\nset default=0\n\nmenuentry \"Supernova\" {\n    multiboot /boot/supernova.bin\n    boot\n}\n" > iso_root/boot/grub/grub.cfg
        
        if grub-mkrescue -o supernova.iso iso_root
            echo "Successfully built supernova.iso"
            launch_qemu
        else
            echo "Error: grub-mkrescue failed"
        end
    else
        echo "Error: Make failed"
    end
end

function fast
        if make
            cp supernova.bin iso_root/boot/
            # We don't need to remake the whole ISO if only the .bin changed 
            # and we are using a persistent iso_root for QEMU
            grub-mkrescue -o supernova.iso iso_root
            launch_qemu
        end
end

# Logic switch
switch "$MODE"
    case "-clean"
        build_all
    case "-run"
        launch_qemu
    case "-fast"
	fast
    case "*"
        echo "Usage: ./run.sh [-clean | -run]"
        echo "  -clean : Full rebuild, ISO generation, and run"
        echo "  -run   : Just launch the existing ISO in QEMU"
end
