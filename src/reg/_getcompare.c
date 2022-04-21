#include "PR/os_internal.h"

u32 osGetCompare(void) {
    u32 compare;

    __asm__("mfc0 %0, $9" : "=r"(compare));
    return compare * 125ull / 192ull;
}
