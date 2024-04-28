#include "PR/os_internal.h"

int memcmp(void* s1, void* s2, size_t n) {
    u8* a = s1;
    u8* b = s2;
    u8 a1, b1;

    while (n-- > 0) {
        b1 = *b++;
        a1 = *a++;
        if (a1 != b1) {
            return a1 - b1;
        }
    }
    return 0;
}
