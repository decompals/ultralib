#include "PR/R4300.h"
#include "sys/asm.h"
#include "sys/regdef.h"

.text
LEAF(__osSetFpcCsr)
    CFC1(v0, fcr31)
    CTC1(a0, fcr31)
    jr ra
END(__osSetSR) # @bug: Should be __osSetFpcCsr
