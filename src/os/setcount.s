#include "PR/R4300.h"
#include "sys/asm.h"
#include "sys/regdef.h"

.text

#ifndef BBPLAYER

LEAF(__osSetCount)
    STAY2(mtc0 a0, C0_COUNT)
    jr ra
END(__osSetCount)

#else
// needed to match elf header flags
.set noreorder
#endif
