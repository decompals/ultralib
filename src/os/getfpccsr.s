#include "PR/R4300.h"
#include "sys/asm.h"
#include "sys/regdef.h"

.text
LEAF(__osGetFpcCsr)
    STAY2(cfc1 v0, $31)
    jr ra
END(__osGetFpcCsr)
.globl __osGetSR
