#include "PR/ultratypes.h"
#include "string.h"

// TODO: this comes from a header
#ident "$Revision: 1.23 $"

char* strchr(const char* s, int c) {
    const char ch = c;
    for (; *s != ch; s++) {
        if (*s == 0) {
            return NULL;
        }
    }
    return (char*)s;
}

size_t strlen(const char* s) {
    const char* sc;
    for (sc = s; *sc != 0; sc++) {
        ;
    }
    return sc - s;
}

void* memcpy(void* s1, const void* s2, size_t n) {
    char* su1;
    const char* su2;
    for (su1 = s1, su2 = s2; n > 0; su1++, su2++, n--) {
        *su1 = *su2;
    }
    return (void*)s1;
}
