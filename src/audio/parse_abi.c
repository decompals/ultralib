#include <PR/os_version.h>
#if defined(_DEBUG) && BUILD_VERSION <= VERSION_G
#define _ULTRALIB_G_NO_OS_INTERNAL_DECLS
#endif
#include <ultra64.h>
#if defined(_DEBUG) && BUILD_VERSION <= VERSION_G
#undef _ULTRALIB_G_NO_OS_INTERNAL_DECLS
#endif
#include <libaudio.h>
#include <ultrahost.h>
#include <gu.h>

#ifndef _FINALROM
void alParseAbiCL(Acmd *cmdList, u32 nbytes)
{
    guDLPrintCB    cntlBlk;

    cntlBlk.dataSize = nbytes;
    cntlBlk.flags = 0;
    cntlBlk.dlType = GU_PARSE_ABI_TYPE;
    cntlBlk.paddr = osVirtualToPhysical(cmdList);
    
    osWriteHost(&cntlBlk,sizeof(cntlBlk));
    osWriteHost(cmdList,nbytes);

}  
        
#endif

























