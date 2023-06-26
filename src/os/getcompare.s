#include "PR/R4300.h"
#include "sys/asm.h"
#include "sys/regdef.h"

.text

#ifndef BBPLAYER

LEAF(__osGetCompare)
    STAY2(mfc0 v0, C0_COMPARE)
    jr ra
END(__osGetCompare)

#else
// needed to match elf header flags
.set noreorder
#endif
