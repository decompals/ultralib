#ifndef _FINALROM

#include "dbgproto.h"
#include "PR/os_internal.h"
#include "PR/rcp.h"
#include "PR/sptask.h"
#include "rmonint.h"
#include "PR/rdb.h"

#include "macros.h"

// TODO: this comes from a header
#ident "$Revision: 1.4 $"

u8 __rmonUtilityBuffer[256] ALIGNED(8);

void __rmonWriteWordTo(u32* addr, u32 val) {
    while (__osSpRawWriteIo((u32)addr, val) != 0) {
        ;
    }
}

u32 __rmonReadWordAt(u32* addr) {
    u32 data;

    if (addr >= (u32*)SP_DMEM_START && addr < (u32*)0x05000000) {
        __osSpRawReadIo((u32)addr, &data);
        return data;
    }
    return 0;
}

void __rmonMemcpy(u8* dest, u8* srce, u32 count) {
    while (count--) {
        *dest++ = *srce++;
    }
}

void __rmonCopyWords(u32* dest, u32* srce, u32 count) {
    while (count--) {
        *dest++ = *srce++;
    }
}

static void strcpy(char* dest, char* srce) {
    while ((*dest++ = *srce++)) {
        ;
    }
}

int __rmonReadMem(KKHeader* req) {
    char* cPtr;
    int sent;
    int dataSize;
    KKReadRequest* request = (KKReadRequest*)req;
    KKBufferEvent* reply = (KKBufferEvent*)__rmonUtilityBuffer;
    u8* blockStart;

    ((void)"ReadMem @ %08x for %d\n");

    reply->header.code = request->header.code;
    reply->object = request->object;
    reply->header.error = TV_ERROR_NO_ERROR;

    if (request->addr == -1) {
        return TV_ERROR_INVALID_ADDRESS;
    }
    if (request->nbytes > 1024) {
        return TV_ERROR_INVALID_CAPABILITY;
    }

    if (req->method == RMON_RSP) {
        if ((request->addr < SP_IMEM_START || (request->addr + request->nbytes) > SP_IMEM_END)) {
            if ((request->addr < SP_DMEM_START || (request->addr + request->nbytes) > SP_DMEM_END)) {
                return TV_ERROR_INVALID_ADDRESS;
            }
        }
    } else if (osVirtualToPhysical((void*)request->addr) == 0xFFFFFFFF) {
        return TV_ERROR_INVALID_ADDRESS;
    }
    blockStart = (u8*)request->addr;
    reply->header.length = request->nbytes + 0x10;
    dataSize = request->nbytes + 0x10;
    cPtr = (u8*)&dataSize;
    sent = 0;

    while (sent < (signed)sizeof(dataSize)) {
        sent += __osRdbSend(cPtr + sent, sizeof(dataSize) - sent, RDB_TYPE_GtoH_DEBUG);
    }

    __rmonSendHeader(&reply->header, 0x10, KK_TYPE_REPLY);
    __rmonSendData(blockStart, request->nbytes);
    return TV_ERROR_NO_ERROR;
}

int __rmonWriteMem(KKHeader* req) {
    register KKWriteRequest* request = (KKWriteRequest*)req;
    KKObjectEvent reply;

    ((void)"WriteMem\n");

    if (req->method == RMON_CPU && osVirtualToPhysical(request->writeHeader.addr) == 0xFFFFFFFF) {
        return TV_ERROR_INVALID_ADDRESS;
    }

    if (request->writeHeader.nbytes > 1024) {
        return TV_ERROR_INVALID_CAPABILITY;
    }

    if ((request->writeHeader.addr >= SP_DMEM_START && (request->writeHeader.addr + request->writeHeader.nbytes) < 0x05000000)) {
        int align;
        u32 word;

        if (align = request->writeHeader.addr & 3) {
            ((void)"Long unaligned write...\n");

            if (request->writeHeader.nbytes != 1) {
                return TV_ERROR_INVALID_ADDRESS;
            }

            // Read the word at the back-aligned target address, substitute in the
            // write and write the full word back
            word = __rmonReadWordAt(request->writeHeader.addr & ~3);
            if (align == 1) {
                word = (word & ~0xFF0000) | (request->buffer[0] << 0x10);
            } else if (align == 2) {
                word = (word & ~0xFF00) | (request->buffer[0] << 8);
            } else {
                word = (word & ~0xFF) | (request->buffer[0] << 0);
            }
            __rmonWriteWordTo(request->writeHeader.addr & ~3, word);
        } else {
            int wordCount = request->writeHeader.nbytes / sizeof(u32);
            u32* wordPointer = (u32*)request->buffer;
            if (request->writeHeader.nbytes % sizeof(u32) != 0) {
                ((void)"RCP write not an integral number of words\n");
                return TV_ERROR_INVALID_ADDRESS;
            }

            while (wordCount--) {
                __rmonWriteWordTo(request->writeHeader.addr, *(wordPointer++));
                request->writeHeader.addr += 4;
            }
        }
    } else {
        __rmonMemcpy(request->writeHeader.addr, request->buffer, request->writeHeader.nbytes);
    }

    reply.header.code = request->writeHeader.header.code;
    reply.header.error = TV_ERROR_NO_ERROR;
    reply.object = request->writeHeader.object;
    __rmonSendReply(&reply.header, sizeof(reply), KK_TYPE_REPLY);

    return TV_ERROR_NO_ERROR;
}

int __rmonListProcesses(KKHeader* req) {
    KKObjectRequest* request = (KKObjectRequest*)req;
    KKObjsEvent reply;

    ((void)"ListProcesses\n");

    reply.object = 0;
    reply.objs.number = 1;
    reply.objs.objects[0] = (req->method == RMON_RSP) ? RMON_PID_RSP : RMON_PID_CPU;
    reply.header.code = request->header.code;
    reply.header.error = TV_ERROR_NO_ERROR;

    __rmonSendReply(&reply.header, sizeof(reply), KK_TYPE_REPLY);
    return TV_ERROR_NO_ERROR;
}

int __rmonLoadProgram(KKHeader* request) {
    ((void)"LoadProgram\n");

    return TV_ERROR_ILLEGAL_CALL;
}

int __rmonGetExeName(KKHeader* req) {
    KKObjectRequest* request = (KKObjectRequest*)req;
    KKBufferEvent* reply = (KKBufferEvent*)__rmonUtilityBuffer;

    ((void)"GetExeName\n");

    reply->header.code = request->header.code;
    reply->header.error = TV_ERROR_NO_ERROR;
    reply->object = request->object;

    if (req->method == RMON_RSP) {
        strcpy(reply->buffer, "imem");
    } else {
        strcpy(reply->buffer, "rmon");
    }
    __rmonSendReply(&reply->header, 0x18, KK_TYPE_REPLY);

    return TV_ERROR_NO_ERROR;
}

int __rmonGetRegionCount(KKHeader* req) {
    KKObjectRequest* request = (KKObjectRequest*)req;
    KKNumberEvent reply;

    ((void)"GetRegionCount\n");

    reply.header.code = request->header.code;
    reply.header.error = TV_ERROR_NO_ERROR;
    reply.object = request->object;

    reply.number = (req->method == RMON_RSP) ? 2 : 5;

    __rmonSendReply(&reply.header, sizeof(reply), KK_TYPE_REPLY);

    return TV_ERROR_NO_ERROR;
}

int __rmonGetRegions(KKHeader* req) {
    KKObjectRequest* request = (KKObjectRequest*)req;
    KKRegionEvent* reply = (KKRegionEvent*)__rmonUtilityBuffer;
    int numRegions;

    ((void)"GetRegions\n");

    numRegions = (req->method == RMON_RSP) ? 2 : 6;

    reply->header.length = numRegions * sizeof(reply->regions[0]) + sizeof(*reply);
    reply->header.code = request->header.code;
    reply->header.error = TV_ERROR_NO_ERROR;
    reply->object = request->object;
    reply->number = numRegions;

    reply->regions[1].vaddr = SP_IMEM_START;
    reply->regions[1].size = SP_IMEM_END + 1 - SP_IMEM_START;
    reply->regions[1].flags = 1 | 2 | 4;
    reply->regions[1].paddr = SP_IMEM_START;

    reply->regions[0].vaddr = SP_DMEM_START;
    reply->regions[0].size = SP_DMEM_END + 1 - SP_DMEM_START;
    reply->regions[0].flags = 1 | 2;
    reply->regions[0].paddr = SP_DMEM_START;

    if (numRegions > 2) {
        reply->regions[2].vaddr = 0x88200000;
        reply->regions[2].size = 0x6130;
        reply->regions[2].flags = 1 | 4;
        reply->regions[2].paddr = 0;

        reply->regions[3].vaddr = 4;
        reply->regions[3].size = 0x200000;
        reply->regions[3].flags = 1 | 2;
        reply->regions[3].paddr = 0;

        reply->regions[4].vaddr = 0x4002000;
        reply->regions[4].size = 0x800000;
        reply->regions[4].flags = 1 | 2;
        reply->regions[4].paddr = 0;

        reply->regions[5].vaddr = 0x88206130;
        reply->regions[5].size = 0x9000;
        reply->regions[5].flags = 1 | 2;
        reply->regions[5].paddr = 0;
    }

    __rmonSendReply(&reply->header, reply->header.length, KK_TYPE_REPLY);

    return TV_ERROR_NO_ERROR;
}

#endif
