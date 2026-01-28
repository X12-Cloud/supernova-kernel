#include "supernova.h"

int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

int str_to_int(char* s) {
    int res = 0;
    while (*s >= '0' && *s <= '9') {
        res = res * 10 + (*s - '0');
        s++;
    }
    return res;
}

void kprint_int(uint32_t n, int color) {
    if (n == 0) {
        putchar('0', color);
        return;
    }

    char buf[11]; // Max uint32_t is 10 digits
    int i = 10;
    buf[i] = '\0';

    while (n > 0) {
        buf[--i] = (n % 10) + '0';
        n /= 10;
    }

    kprint(&buf[i], -1, color);
}

int memcmp(const void* s1, const void* s2, size_t n) {
    const uint8_t* p1 = (const uint8_t*)s1;
    const uint8_t* p2 = (const uint8_t*)s2;

    for (size_t i = 0; i < n; i++) {
        if (p1[i] < p2[i]) return -1;
        if (p1[i] > p2[i]) return 1;
    }
    return 0;
}
