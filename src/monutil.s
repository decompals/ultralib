#ifndef _FINALROM
#include "sys/asm.h"
#include "sys/regdef.h"
#include "PR/R4300.h"
#include "PR/ultratypes.h"

.text
LEAF(__isExpJP)
.set noreorder
    j __isExp
     nop
    sw k0, -0x10(sp)
    la k0, __isExp
    jr k0
     nop
.set reorder
END(__isExpJP)

LEAF(__isExp)
.set noreorder
    sw k0, -0x10(sp)
    sw k1, -8(sp)
    mfc0 k0, C0_CAUSE
    nop
    andi k0, k0, 0x7C
    bnez k0, non_stop
     nop
    mfc0 k0, C0_CAUSE
    nop
    andi k0, k0, 0x4000
    beqz k0, throw_os_exception
     nop
    j go_monitor
     nop

non_stop:
    sra k0, k0, 2
    addiu k1,  zero, 9
    bne k0, k1, non_bp
     nop
    j go_monitor
     nop

non_bp:
    addiu k1, zero, 4
    bne k0, k1, non_adrs_store_exp
     nop
    j go_monitor
     nop

non_adrs_store_exp:
    addiu k1, zero, 5
    bne k0, k1, non_adrs_load_exp
     nop
    j go_monitor
     nop

non_adrs_load_exp:
    addiu k1, zero, 6
    bne k0, k1, non_bus_code_exp
     nop
    j go_monitor
     nop

non_bus_code_exp:
    addiu k1, zero, 7
    bne k0, k1, non_bus_data_exp
     nop
    j go_monitor
     nop

non_bus_data_exp:
    addiu k1, zero, 1
    bne k0, k1, non_tlb_mod
     nop
    j go_monitor
     nop

non_tlb_mod:
    addiu k1, zero, 2
    bne k0, k1, non_tlb_load
     nop
    j go_monitor
     nop

non_tlb_load:
    addiu k1, zero, 3
    bne k0, k1, non_tlb_store
     nop
    j go_monitor
     nop

non_tlb_store:
    addiu k1, zero, 0xA
    bne k0, k1, non_resv
     nop
    j go_monitor
     nop

non_resv:
    lw k1, -8(sp)
throw_os_exception:
    j ramOldVector
     nop

go_monitor:
    move k0, sp
    addiu sp, sp, -0x200
    sw k0, 0x78(sp)
    lw k1, -8(k0)
    sw k1, 0x70(sp)
    lw k1, -0x10(k0)
    sw k1, 0x6c(sp)
    nop

w:
    lui k0, 0xA460
    lw k0, 0x10(k0)
    nop
    andi k0, k0, 3
    bnez k0, w
     nop
    lui k0, 0xBFF0
    addiu k0, k0, 0x50
    jr k0
     nop
.set reorder
END(__isExp)

LEAF(MonitorInitBreak)
.set noreorder
    nop
    nop
INIT_BREAK_POSITION:
    break 4
    nop
    nop
    nop
    jr ra
     nop
.set reorder
END(MonitorInitBreak)

#endif
