#include "PR/os_internal.h"
#include "PR/rcp.h"

u8 __osBbCardChange;

const struct /* size=0x8 */ {
    /* 0x0000 */ u8 mfg;
    /* 0x0001 */ u8 dev;
    /* 0x0002 */ u16 blocks;
    /* 0x0004 */ u32 config;
} __osBbCardConfig[6] = {
    { 0x00, 0x00, 0x0000, 0x753E3EFF },
    { 0xEC, 0x76, 0x1000, 0x441F1F3F },
    { 0xEC, 0x79, 0x2000, 0x441F1F3F },
    { 0x98, 0x76, 0x1000, 0x753E1F3F },
    { 0x20, 0x76, 0x1000, 0x441F1F3F },

    { 0xFF, 0xFF, 0x0000, 0x000000FF }
};

u8 __osBbCardMultiplane;
u8 __osBbCardInit;
u16 __osBbCardBlocks;
static OSPiHandle __osBbCardHandler;
static OSMesgQueue __osBbCardDmaQ;
static OSMesg __osBbCardDmaBuf;
static OSMesg cardAccessBuf[1];
OSMesgQueue __osBbCardAccessQueue;

s32 __osBbCardFlushEvent(void);

void __osBbCardInitEvent(void);

s32 osBbCardReadId(u32 dev, u32* mfg, u32* type);

s32 __osBbCardPresent(void) {
    if (__osBbCardFlushEvent()) {
        __osBbCardChange = TRUE;
    }
    return !(IO_READ(MI_BASE_REG + 0x38) >> 0x19 & 1);
}

void __osBbCardRelAccess(void) {
    osSendMesg(&__osBbCardAccessQueue, NULL, OS_MESG_NOBLOCK);
}

s32 __osBbCardGetAccess(void) {
    osRecvMesg(&__osBbCardAccessQueue, NULL, OS_MESG_BLOCK);

    if (!__osBbCardPresent() || __osBbCardChange) {
        __osBbCardRelAccess();

        if (!__osBbCardPresent()) {
            return -1;
        }
        return -4;
    }
    return 0;
}

void osBbCardInit(void);
#ifdef NON_MATCHING
// Single delay slot reordering
void osBbCardInit(void) {
    u32 type;
    u32 mfg;

    if (!__osBbCardInit) {
        osCreateMesgQueue(&__osBbCardAccessQueue, &cardAccessBuf, 1);
        osSendMesg(&__osBbCardAccessQueue, NULL, 0);
        __osBbCardInitEvent();

        IO_WRITE(MI_BASE_REG + 0x3C, 0x08002000);

        __osBbCardHandler.type = 6;
        __osBbCardHandler.baseAddress = 0;
        __osBbCardHandler.latency = 0;
        __osBbCardHandler.pulse = 0;
        __osBbCardHandler.pageSize = 0;
        __osBbCardHandler.relDuration = 0;
        __osBbCardHandler.domain = 0;
        __osBbCardHandler.speed = 0;

        bzero(&__osBbCardHandler.transferInfo, sizeof(__OSTranxInfo));
        osEPiLinkHandle(&__osBbCardHandler);
        osCreateMesgQueue(&__osBbCardDmaQ, &__osBbCardDmaBuf, 1);
        __osBbCardInit = TRUE;
    }

    IO_WRITE(PI_BASE_REG + 0x48, 0);
    __osBbCardChange = FALSE;

    __osBbCardFlushEvent();

    if (!(IO_READ(MI_BASE_REG + 0x38) & 0x2000000)) {
        u16 i;

        IO_WRITE(PI_BASE_REG + 0x4C, __osBbCardConfig[0].config);
        osBbCardReadId(0, &mfg, &type);

        for (i = 0; __osBbCardConfig[i].mfg != 0xFF; i++) {
            if (__osBbCardConfig[i].mfg == mfg && __osBbCardConfig[i].dev == type) {
                __osBbCardBlocks = __osBbCardConfig[i].blocks;
                IO_WRITE(PI_BASE_REG + 0x4C, __osBbCardConfig[i].config);
                break;
            }
        }
    } else {
        __osBbCardBlocks = 0;
    }
}
#endif

void __osBbCardDmaCopy(u32 which, void* addr, u32 dir) {
    OSIoMesg m;

    m.hdr.pri = 0;
    m.hdr.retQueue = &__osBbCardDmaQ;
    m.dramAddr = addr;
    m.size = 0x200;
    m.devAddr = which * 0x200;

    if (dir == OS_WRITE) {
        osWritebackDCache(addr, 0x200);
    }
    osEPiStartDma(&__osBbCardHandler, &m, dir);
    osRecvMesg(&__osBbCardDmaQ, NULL, OS_MESG_BLOCK);
    if (dir == OS_READ) {
        osInvalDCache(addr, 0x200);
    }
}
