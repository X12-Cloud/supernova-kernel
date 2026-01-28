#include "supernova.h"

typedef struct {
    uint8_t  boot_jmp[3];
    char     oem_name[8];
    uint16_t bytes_per_sector;
    uint8_t  sectors_per_cluster;
    uint16_t reserved_sector_count;
    uint8_t  table_count;
    uint16_t root_entry_count;
    uint16_t total_sectors_16;
    uint8_t  media_type;
    uint16_t table_size_16;
    uint16_t sectors_per_track;
    uint16_t head_side_count;
    uint32_t hidden_sector_count;
    uint32_t total_sectors_32;
    // FAT32 Extended Fields
    uint32_t table_size_32;
    uint16_t extended_flags;
    uint16_t fat_version;
    uint32_t root_cluster;
    uint16_t fat_info;
    uint16_t backup_BS_sector;
    uint8_t  reserved_0[12];
    uint8_t  drive_number;
    uint8_t  reserved_1;
    uint8_t  boot_signature;
    uint32_t volume_id;
    char     volume_label[11];
    char     file_system_type[8];
} __attribute__((packed)) fat32_bpb_t;

// Static buffer to hold the BPB so it doesn't vanish off the stack
static uint8_t bpb_buffer[512];
static fat32_bpb_t* bpb;
uint32_t first_data_sector;

static uint8_t sector_buffer[512];
uint32_t root_dir_sector;

void fat32_init() {
    ata_read_sector(0, (uint16_t*)sector_buffer);

    uint16_t reserved_sectors = *(uint16_t*)&sector_buffer[14];
    uint8_t fat_count = sector_buffer[16];
    uint32_t sectors_per_fat = *(uint32_t*)&sector_buffer[36];

    // Calculate Root Dir Sector
    root_dir_sector = reserved_sectors + (fat_count * sectors_per_fat);

    kprint("FAT32: Root Directory is at Sector: ", -1, 0x0A);
    kprint_int(root_dir_sector, 0x0E); // Print the number in Yellow
    putchar('\n', 0x07);
}

uint32_t cluster_to_sector(uint32_t cluster) {
    // sectors_per_cluster is usually 1 or 8 (check byte 13 of your BPB!)
    return root_dir_sector + ((cluster - 2) * 1); 
}

void format_to_83(char* input, char* output) {
    // Fill output with spaces first
    for(int i = 0; i < 11; i++) output[i] = ' ';

    int i = 0;
    // Copy the name part (up to 8 chars or until a dot)
    while (input[i] != '.' && input[i] != '\0' && i < 8) {
        char c = input[i];
        if (c >= 'a' && c <= 'z') c -= 32; // Force uppercase
        output[i] = c;
        i++;
    }

    // Move to the extension part
    char* dot = 0;
    for (int j = 0; input[j] != '\0'; j++) {
        if (input[j] == '.') {
            dot = &input[j + 1];
            break;
        }
    }

    if (dot) {
        int j = 0;
        while (dot[j] != '\0' && j < 3) {
            char c = dot[j];
            if (c >= 'a' && c <= 'z') c -= 32; // Force uppercase
            output[8 + j] = c;
            j++;
        }
    }
}
