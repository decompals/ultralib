#include "piint.h"

// TODO: this comes from a header
#ifdef BBPLAYER
#ident "$Revision: 1.1 $"
#endif

s32 osEPiGetDeviceType(OSPiHandle* pihandle, OSPiInfo* info)
{
    info->type = pihandle->type;
    info->address = pihandle->baseAddress;
    return 0;
}
