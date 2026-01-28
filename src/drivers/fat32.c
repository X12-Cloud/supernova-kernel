#include "supernova.h"

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

void fat32_create_file(char* filename) {
    uint8_t sector_buf[512];
    // Read the current root directory state
    ata_read_sector(root_dir_sector, (uint16_t*)sector_buf);

    fat32_entry_t* entries = (fat32_entry_t*)sector_buf;
    
    // Scan all 16 entries in the sector
    for (int i = 0; i < 16; i++) {
        uint8_t first_char = (uint8_t)entries[i].name[0];

        // 0x00 = Never used, 0xE5 = Deleted
        if (first_char == 0x00 || first_char == 0xE5) {
            kprint("\nFound empty slot at index: ", -1, 0x0A);
            kprint_int(i, 0x0A);
            kprint("\n", -1, 0x07);

            memset(&entries[i], 0, sizeof(fat32_entry_t));

            char formatted_name[11];
            format_to_83(filename, formatted_name);

            memcpy(entries[i].name, formatted_name, 8);
            memcpy(entries[i].ext, &formatted_name[8], 3);
            entries[i].attributes = 0x20; // Archive attribute
            entries[i].cluster_low = 5;   // Hardcoded for now
            entries[i].size = 0;          // New file is empty

            ata_write_sector(root_dir_sector, (uint16_t*)sector_buf);
            
            kprint("File created successfully.\n", -1, 0x0A);
            return; // Exit function so we don't hit the error message below
        }
    }

    kprint("Error: No free slots in root directory.\n", -1, 0x0C);
}

void fat32_write_file(char* filename, char* data) {
    uint8_t buffer[512];
    memset(buffer, 0, 512); // Clear the scratchpad
    
    uint32_t data_len = strlen(data);
    if (data_len > 512) data_len = 512; 
    memcpy(buffer, data, data_len);

    uint32_t target_sector = cluster_to_sector(5);

    ata_write_sector(target_sector, (uint16_t*)buffer);

    kprint("Data written to Cluster 5\n", -1, 0x0A);
}

// Returns the index (0-15) of the file if found, or -1 if not found
int fat32_find_file(char* filename) {
    uint8_t buf[512];
    extern uint32_t root_dir_sector;
    char target[11];
    
    format_to_83(filename, target);
    ata_read_sector(root_dir_sector, (uint16_t*)buf);

    for (int i = 0; i < 512; i += 32) {
        if (buf[i] == 0x00) break;
        if ((uint8_t)buf[i] == 0xE5) continue;
        
        // Compare the 11 formatted bytes
        if (memcmp(&buf[i], target, 11) == 0) {
            return i / 32; // Found it! Return the entry index
        }
    }
    return -1; // Not found
}

void fat32_delete_file(char* filename) {
    uint8_t buf[512];
    char target[11];
    format_to_83(filename, target);
    
    ata_read_sector(root_dir_sector, (uint16_t*)buf);

    for (int i = 0; i < 512; i += 32) {
        if (buf[i] == 0x00) break;
        if (memcmp(&buf[i], target, 11) == 0) {
            // THE DELETE MOVE: Mark as 0xE5
            buf[i] = 0xE5; 
            
            // Save it back to disk
            ata_write_sector(root_dir_sector, (uint16_t*)buf);
            kprint("File deleted.\n", -1, 0x0A);
            return;
        }
    }
    kprint("File not found.\n", -1, 0x0C);
}
