#include "PR/os_internal.h"

void* memset(char* s, int c, size_t n) {
    int i;

    for (i = 0; i < n; i++) {
        s[i] = c;
    }

    return s;
}
