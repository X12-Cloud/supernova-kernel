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

            // 1. Wipe the entry clean
            memset(&entries[i], 0, sizeof(fat32_entry_t));

            // 2. Format name to 8.3 (e.g. "test.txt" -> "TEST    TXT")
            char formatted_name[11];
            format_to_83(filename, formatted_name);

            // 3. Fill the entry fields
            memcpy(entries[i].name, formatted_name, 8);
            memcpy(entries[i].ext, &formatted_name[8], 3);
            entries[i].attributes = 0x20; // Archive attribute
            entries[i].cluster_low = 5;   // Hardcoded for now
            entries[i].size = 0;          // New file is empty

            // 4. IMPORTANT: Write the modified sector back to disk!
            ata_write_sector(root_dir_sector, (uint16_t*)sector_buf);
            
            kprint("File created successfully.\n", -1, 0x0A);
            return; // Exit function so we don't hit the error message below
        }
    }

    kprint("Error: No free slots in root directory.\n", -1, 0x0C);
}
