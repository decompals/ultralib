#include "PR/os_internal.h"
#include "PR/sptask.h"
#include "PR/rcp.h"

// TODO: this comes from a header
#ifdef BBPLAYER
#ident "$Revision: 1.1 $"
#endif

OSYieldResult osSpTaskYielded(OSTask *tp) {
    u32 status;
    OSYieldResult result;
    
    status = __osSpGetStatus();
    result = status >> 8; 
    result &= 1;

    if (status & SP_STATUS_YIELD) {
        tp->t.flags = ~(OS_TASK_DP_WAIT) & (tp->t.flags | result);
    }
    
    return result;
}
