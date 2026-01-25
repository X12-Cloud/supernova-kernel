void kernel_main_64() {
    volatile char* vidmem = (volatile char*)0xB8000;
    const char* msg = "SUCCESS: 64-BIT MODE ACTIVE";
    for(int i = 0; msg[i] != '\0'; i++) {
        vidmem[i*2] = msg[i];
        vidmem[i*2+1] = 0x0E; 
    }
    while(1);
}
