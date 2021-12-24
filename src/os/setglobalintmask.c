#include "PR/ultratypes.h"

typedef u32 OSEvent;
typedef u32 OSIntMask;
typedef u32 OSPageMask;
typedef u32 OSHWIntr;

extern OSIntMask __OSGlobalIntMask;	/* global interrupt mask */
extern u32		__osDisableInt(void);
extern void		__osRestoreInt(u32);

void __osSetGlobalIntMask(OSHWIntr mask)
{
    register u32 saveMask = __osDisableInt();
    __OSGlobalIntMask |= mask;
    __osRestoreInt(saveMask);
}
