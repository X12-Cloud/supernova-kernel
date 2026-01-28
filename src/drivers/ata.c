#include "supernova.h"

#define ATA_DATA        0x1F0
#define ATA_ERROR       0x1F1
#define ATA_SECCOUNT    0x1F2
#define ATA_LBA_LOW     0x1F3
#define ATA_LBA_MID     0x1F4
#define ATA_LBA_HIGH    0x1F5
#define ATA_DRIVE_SEL   0x1F6
#define ATA_COMMAND     0x1F7
#define ATA_STATUS      0x1F7

void ata_wait_ready() {
    // Wait for BSY (bit 7) to clear and RDY (bit 6) to set
    while ((inb(ATA_STATUS) & 0x80) || !(inb(ATA_STATUS) & 0x40));
}

void ata_read_sector(uint32_t lba, uint16_t* buffer) {
    outb(0x1F6, 0xE0 | ((lba >> 24) & 0x0F));
    outb(0x1F2, 1);
    outb(0x1F3, (uint8_t)lba);
    outb(0x1F4, (uint8_t)(lba >> 8));
    outb(0x1F5, (uint8_t)(lba >> 16));
    outb(0x1F7, 0x20); // Read Command

    // New Polling with Debugging
    for (int timeout = 0; timeout < 1000000; timeout++) {
        uint8_t status = inb(0x1F7);
        
        // If ERR (bit 0) is set, something is wrong with the disk image
        if (status & 0x01) {
            kprint("ATA Error: Status bit 0 set!\n", -1, 0x0C);
            return;
        }

        // If BSY (bit 7) is clear and DRQ (bit 3) is set, we are good to go!
        if (!(status & 0x80) && (status & 0x08)) {
            for (int i = 0; i < 256; i++) {
                buffer[i] = inw(0x1F0);
            }
            return;
        }
    }

    kprint("ATA Error: Read Timeout! Disk not responding.\n", -1, 0x0C);
}

void ata_write_sector(uint32_t lba, uint16_t* buffer) {
    ata_wait_ready();

    outb(ATA_DRIVE_SEL, 0xE0 | ((lba >> 24) & 0x0F));
    outb(ATA_SECCOUNT, 1);
    outb(ATA_LBA_LOW,  (uint8_t)lba);
    outb(ATA_LBA_MID,  (uint8_t)(lba >> 8));
    outb(ATA_LBA_HIGH, (uint8_t)(lba >> 16));
    outb(ATA_COMMAND,  0x30); // 0x30 is "Write Sectors"

    ata_wait_ready();

    // Write 256 words (512 bytes) from our RAM to the Disk
    for (int i = 0; i < 256; i++) {
        outw(ATA_DATA, buffer[i]); // You'll need an 'outw' function
    }
    
    // Flush the cache to ensure the disk actually saves it
    outb(0x1F7, 0xE7); // Cache Flush command
    ata_wait_ready();
}
