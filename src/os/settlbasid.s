#include "PR/ultraerror.h"
#include "PR/R4300.h"
#include "sys/asm.h"
#include "sys/regdef.h"

.text
LEAF(osSetTLBASID)
#ifdef __sgi
#ifdef _DEBUG
.set noreorder
    bgez a0, 1f
    nop
    b 2f
    nop
1:
    li t1, 0xFF
.set noat
    slt AT, t1, a0
    beqz AT, 3f
    nop
.set at
2:
    move a2, a0
    li a0, ERR_OSSETTLBASID
    li a1, 1
    j __osError
    nop
3:
.set reorder
#endif
#endif
    STAY2(mtc0 a0, C0_ENTRYHI)
    jr ra
END(osSetTLBASID)
