#include "PR/os_internal.h"

int strcmp(u8* s, u8* t) {
    while (*s && *t && *s == *t) {
        s++;
        t++;
    }

    if (*s < *t) {
        return -1;
    }
    if (*s > *t) {
        return 1;
    }

    return 0;
}
