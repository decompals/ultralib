#include "PR/R4300.h"
#include "sys/asm.h"
#include "sys/regdef.h"

.text
LEAF(__osSetFpcCsr)
    STAY2(cfc1 v0, $31)
    STAY2(ctc1 a0, $31)
    jr ra
END(__osSetFpcCsr)
.globl __osSetSR
