#include "PR/os_internal.h"

void __osSetCount(u32 v) {
    v = v * 192ull / 125ull;
    __asm__ ("mtc0 %0, $9" :: "r"(v));
}
