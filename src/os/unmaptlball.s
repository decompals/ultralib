#include "PR/R4300.h"
#include "PR/os_version.h"
#include "sys/asm.h"
#include "sys/regdef.h"

.text
.set noreorder
LEAF(osUnmapTLBAll)
    mfc0    t0, C0_ENTRYHI
#if BUILD_VERSION < VERSION_I
    li      t1, NTLBENTRIES
#else
    li      t1, NTLBENTRIES-1 /* last reserved for rdb */
#endif
    li      t2, (K0BASE & TLBHI_VPN2MASK)
    mtc0    t2, C0_ENTRYHI
    mtc0    zero, C0_ENTRYLO0
    mtc0    zero, C0_ENTRYLO1
1:
    mtc0    t1, C0_INX
    nop
    tlbwi
    nop
    nop
    addi    t1, t1, -1
#if BUILD_VERSION < VERSION_I
    bnezl   t1, 1b
#else
    bgez    t1, 1b
#endif
     nop
    mtc0    t0, C0_ENTRYHI
    jr      ra
     nop
END(osUnmapTLBAll)
