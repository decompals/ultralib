#include "PR/ultratypes.h"
#include "PR/os_version.h"
#include "string.h"

// TODO: this comes from a header
#ident "$Revision: 1.23 $"

char* strchr(const char* s, int c) {
    const char ch = c;
#if defined(_DEBUG) && BUILD_VERSION <= VERSION_G
    for (; *s != ch; s++) {
#else
    while (*s != ch) {
#endif
        if (*s == 0) {
            return NULL;
        }
#if !(defined(_DEBUG) && BUILD_VERSION <= VERSION_G)
        s++;
#endif
    }
    return (char*)s;
}

size_t strlen(const char* s) {
    const char* sc = s;
#if defined(_DEBUG) && BUILD_VERSION <= VERSION_G
    for (; *sc; sc++) {
#else
    while (*sc != 0) {
        sc++;
#endif
    }
    return sc - s;
}

void* memcpy(void* s1, const void* s2, size_t n) {
#if defined(_DEBUG) && BUILD_VERSION <= VERSION_G
    char* su1;
    const char* su2;
#else
    char* su1 = (char*)s1;
    const char* su2 = (const char*)s2;
#endif
#if defined(_DEBUG) && BUILD_VERSION <= VERSION_G
    for (su1 = (char*)s1, su2 = (const char*)s2; 0 < n; ++su1, ++su2, --n)
        *su1 = *su2;
#else
    while (n > 0) {
        *su1 = *su2;
        su1++;
        su2++;
        n--;
    }
#endif
    return (void*)s1;
}
