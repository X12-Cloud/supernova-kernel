#include "supernova.h"

void get_cpu_vendor(char* vendor) {
    uint32_t ebx, edx, ecx;
    // CPUID with EAX = 0 returns the vendor string in EBX, EDX, ECX
    __asm__ volatile("cpuid"
                     : "=b"(ebx), "=d"(edx), "=c"(ecx)
                     : "a"(0));

    // Copy the 12 characters into our buffer
    ((uint32_t*)vendor)[0] = ebx;
    ((uint32_t*)vendor)[1] = edx;
    ((uint32_t*)vendor)[2] = ecx;
    vendor[12] = '\0';
}

void get_cpu_brand(char* brand) {
    uint32_t regs[4]; // eax, ebx, ecx, edx

    // We need to check if extended brand string is supported
    __asm__ volatile("cpuid" : "=a"(regs[0]) : "a"(0x80000000));
    if (regs[0] < 0x80000004) {
        kprint("Brand string not supported", -1, 0x0C);
        return;
    }

    // Brand string is in 3 parts: 0x80000002, 0x80000003, 0x80000004
    for (uint32_t i = 0; i < 3; i++) {
        __asm__ volatile("cpuid" 
                         : "=a"(regs[0]), "=b"(regs[1]), "=c"(regs[2]), "=d"(regs[3]) 
                         : "a"(0x80000002 + i));
        
        // Each call returns 16 bytes (4 regs * 4 bytes)
        for (int j = 0; j < 4; j++) {
            ((uint32_t*)brand)[i * 4 + j] = regs[j];
        }
    }
    brand[48] = '\0'; // Null terminate
}
