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

typedef struct {
    char     name[8];      // Filename (padded with spaces)
    char     ext[3];       // Extension (padded with spaces)
    uint8_t  attributes;   // 0x20 for file, 0x10 for folder
    uint8_t  reserved;     // Always 0
    uint8_t  create_ms;    // Creation time (milliseconds)
    uint16_t create_time;
    uint16_t create_date;
    uint16_t access_date;
    uint16_t cluster_high; // High 16 bits of cluster (usually 0 for small disks)
    uint16_t modify_time;
    uint16_t modify_date;
    uint16_t cluster_low;  // Low 16 bits of cluster (The important one!)
    uint32_t size;         // File size in bytes
} __attribute__((packed)) fat32_entry_t;
