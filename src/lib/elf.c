// src/lib/elf.c
#include "supernova.h"

#define ELF_MAGIC_0 0x7F
#define ELF_MAGIC_1 'E'
#define ELF_MAGIC_2 'L'
#define ELF_MAGIC_3 'F'

typedef struct {
    unsigned char e_ident[16];
    uint16_t      e_type;
    uint16_t      e_machine;
    uint32_t      e_version;
    uint64_t      e_entry;     
    uint64_t      e_phoff;     
    uint64_t      e_shoff;
    uint32_t      e_flags;
    uint16_t      e_ehsize;
    uint16_t      e_phentsize;
    uint16_t      e_phnum;     
    uint16_t      e_shentsize;
    uint16_t      e_shnum;
    uint16_t      e_shstrndx;
} __attribute__((packed)) Elf64_Ehdr;

typedef struct {
    uint32_t p_type;
    uint32_t p_flags;
    uint64_t p_offset;
    uint64_t p_vaddr;
    uint64_t p_paddr;
    uint64_t p_filesz;
    uint64_t p_memsz;
    uint64_t p_align;
} __attribute__((packed)) Elf64_Phdr;

int exec_elf(char* filename) {
    char target[11];
    format_to_83(filename, target); 

    uint8_t directory_buffer[512];
    extern uint32_t root_dir_sector;
    ata_read_sector(root_dir_sector, (uint16_t*)directory_buffer);

    fat32_entry_t* entry = (fat32_entry_t*)directory_buffer;
    int file_found = 0;
    int entry_idx = 0;

    for (int i = 0; i < 16; i++) {
        if (entry[i].name[0] == 0x00) break;
        if ((uint8_t)entry[i].name[0] == 0xE5) continue;

        if (memcmp(entry[i].name, target, 11) == 0) {
            entry_idx = i;
            file_found = 1;
            break;
        }
    }

    if (!file_found) {
        kprint("ELF Loader: File not found.\n", -1, 0x0C);
        return -1;
    }

    uint16_t cluster = entry[entry_idx].cluster_low;
    uint32_t start_sector = root_dir_sector + (cluster - 2);

    uint8_t elf_sector_scratch[512];
    ata_read_sector(start_sector, (uint16_t*)elf_sector_scratch);

    Elf64_Ehdr* ehdr = (Elf64_Ehdr*)elf_sector_scratch;

    if (ehdr->e_ident[0] != ELF_MAGIC_0 || ehdr->e_ident[1] != ELF_MAGIC_1 ||
        ehdr->e_ident[2] != ELF_MAGIC_2 || ehdr->e_ident[3] != ELF_MAGIC_3) {
        kprint("ELF Loader: Invalid Magic! Not an ELF executable.\n", -1, 0x0C);
        return -1;
    }

    if (ehdr->e_machine != 0x3E) { 
        kprint("ELF Loader: Incorrect architecture! Expected x86_64.\n", -1, 0x0C);
        return -1;
    }

    uint8_t* phdr_start = elf_sector_scratch + ehdr->e_phoff;

    for (int i = 0; i < ehdr->e_phnum; i++) {
        Elf64_Phdr* phdr = (Elf64_Phdr*)(phdr_start + (i * ehdr->e_phentsize));

        if (phdr->p_type == 1) {
            uint8_t* destination_ram = (uint8_t*)phdr->p_vaddr;
            uint32_t file_size_bytes = entry[entry_idx].size;
            uint32_t sectors_to_read = (file_size_bytes + 511) / 512;

            for (uint32_t s = 0; s < sectors_to_read; s++) {
                ata_read_sector(start_sector + s, (uint16_t*)(destination_ram + (s * 512)));
            }

            if (phdr->p_offset > 0) {
                for (uint64_t b = 0; b < phdr->p_filesz; b++) {
                    destination_ram[b] = destination_ram[phdr->p_offset + b];
                }
            }

            if (phdr->p_memsz > phdr->p_filesz) {
                size_t bss_size = phdr->p_memsz - phdr->p_filesz;
                memset(destination_ram + phdr->p_filesz, 0, bss_size);
            }
        }
    }

    kprint("ELF Loaded! Executing entry jump...\n", -1, 0x0A);

    void (*entry_point)(void) = (void (*)(void))ehdr->e_entry;
    entry_point();

    return 0;
}
