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

void fat32_init() {
    uint8_t sector[512];
    ata_read_sector(0, (uint16_t*)sector);

    // Reserved sectors (usually 32)
    uint16_t res_secs = *(uint16_t*)&sector[14];
    // Number of FAT tables (usually 2)
    uint8_t fat_count = sector[16];
    // Sectors per FAT table
    uint32_t fat_size = *(uint32_t*)&sector[36];

    // Data starts after all that:
    first_data_sector = res_secs + (fat_count * fat_size);
    
    kprint("FAT32: Data Area Starts at Sector ", -1, 0x0A);
    // print_int(first_data_sector);
}
