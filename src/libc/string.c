#include "PR/ultratypes.h"
#include "string.h"

#ifndef BBPLAYER
// TODO: this comes from a header
#ident "$Revision: 1.23 $"
#endif

char* strchr(const char* s, int c) {
    const char ch = c;
    while (*s != ch) {
        if (*s == 0) {
            return NULL;
        }
        s++;
    }
    return (char*)s;
}

size_t strlen(const char* s) {
    const char* sc = s;
    while (*sc != 0) {
        sc++;
    }
    return sc - s;
}

void* memcpy(void* s1, const void* s2, size_t n) {
    char* su1 = (char*)s1;
    const char* su2 = (const char*)s2;
    while (n > 0) {
        *su1 = *su2;
        su1++;
        su2++;
        n--;
    }
    return (void*)s1;
}
