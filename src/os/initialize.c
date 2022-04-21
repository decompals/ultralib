#include "PR/os_internal.h"
#include "PR/rcp.h"

typedef struct
{
   /* 0x0 */ unsigned int inst1;
   /* 0x4 */ unsigned int inst2;
   /* 0x8 */ unsigned int inst3;
   /* 0xC */ unsigned int inst4;
} __osExceptionVector;
extern __osExceptionVector __osExceptionPreamble;

extern OSPiHandle __Dom1SpeedParam;
extern OSPiHandle __Dom2SpeedParam;

#ifdef BBPLAYER
extern u32 __osBbIsBb;
extern u32 __osBbEepromSize;
extern u32 __osBbPakSize;
extern u32 __osBbFlashSize;
extern u32 __osBbEepromAddress;
extern u32 __osBbPakAddress[4];
extern u32 __osBbFlashAddress;
extern u32 __osBbSramSize;
extern u32 __osBbSramAddress;

char __osBbLibVersion[] = "libultra 12/06/04 08:28:27";
#endif

OSTime osClockRate = OS_CLOCK_RATE;
s32 osViClock = VI_NTSC_CLOCK;
u32 __osShutdown = 0;
#ifdef BBPLAYER
u32 __osShutdownTime = 0;
#endif
u32 __OSGlobalIntMask = OS_IM_ALL;
u32 __osFinalrom;

void __createSpeedParam(void) {
    __Dom1SpeedParam.type = DEVICE_TYPE_INIT;
    __Dom1SpeedParam.latency = IO_READ(PI_BSD_DOM1_LAT_REG);
    __Dom1SpeedParam.pulse = IO_READ(PI_BSD_DOM1_PWD_REG);
    __Dom1SpeedParam.pageSize = IO_READ(PI_BSD_DOM1_PGS_REG);
    __Dom1SpeedParam.relDuration = IO_READ(PI_BSD_DOM1_RLS_REG);

    __Dom2SpeedParam.type = DEVICE_TYPE_INIT;
    __Dom2SpeedParam.latency = IO_READ(PI_BSD_DOM2_LAT_REG);
    __Dom2SpeedParam.pulse = IO_READ(PI_BSD_DOM2_PWD_REG);
    __Dom2SpeedParam.pageSize = IO_READ(PI_BSD_DOM2_PGS_REG);
    __Dom2SpeedParam.relDuration = IO_READ(PI_BSD_DOM2_RLS_REG);
}

void __osInitialize_common() {
    u32 pifdata;

    __osFinalrom = TRUE;

    __osSetSR(__osGetSR() | SR_CU1);    //enable fpu
    __osSetFpcCsr(FPCSR_FS | FPCSR_EV); //flush denorm to zero, enable invalid operation
    __osSetWatchLo(0x4900000);

#ifdef BBPLAYER
    {
        u32 x, y;

        IO_WRITE(MI_BASE_REG + 0x3C, 0x22000);
        x = IO_READ(MI_BASE_REG + 0x3C); // MI_HW_INTR_MASK_REG
        IO_WRITE(MI_BASE_REG + 0x3C, 0x11000);
        y = IO_READ(MI_BASE_REG + 0x3C); // MI_HW_INTR_MASK_REG

        __osBbIsBb = ((x & 0x140) == 0x140) && ((y & 0x140) == 0);
    }

    if (__osBbIsBb) {
        if (IO_READ(PI_BASE_REG + 0x60) & 0xC0000000) { // PI_MISC_REG
            __osBbIsBb = 2;
        }
    }

    if (__osBbIsBb) {
        osTvType = OS_TV_NTSC;
        osRomType = 0;
        osResetType = 0;
        osVersion = 1;
    } else {
#endif
    while (__osSiRawReadIo(PIF_RAM_END - 3, &pifdata)) { //last byte of joychannel ram
        ;
    }
    while (__osSiRawWriteIo(PIF_RAM_END - 3, pifdata | 8)) {
        ; //todo: magic constant
    }
#ifdef BBPLAYER
    }
#endif

    *(__osExceptionVector *)UT_VEC = __osExceptionPreamble;
    *(__osExceptionVector *)XUT_VEC = __osExceptionPreamble;
    *(__osExceptionVector *)ECC_VEC = __osExceptionPreamble;
    *(__osExceptionVector *)E_VEC = __osExceptionPreamble;
    osWritebackDCache((void *)UT_VEC, E_VEC - UT_VEC + sizeof(__osExceptionVector));
    osInvalICache((void *)UT_VEC, E_VEC - UT_VEC + sizeof(__osExceptionVector));
    __createSpeedParam();
    osUnmapTLBAll();
    osMapTLBRdb();
    osClockRate = osClockRate * 3 / 4;

    if (osResetType == 0) { // cold reset
        bzero(osAppNMIBuffer, OS_APP_NMI_BUFSIZE);
    }

    if (osTvType == OS_TV_PAL) {
        osViClock = VI_PAL_CLOCK;
    } else if (osTvType == OS_TV_MPAL) {
        osViClock = VI_MPAL_CLOCK;
    } else {
        osViClock = VI_NTSC_CLOCK;
    }

    // Lock up if there are RCP interrupts pending
    if (__osGetCause() & CAUSE_IP5) {
        while (TRUE) {
            ;
        }
    }

#ifdef BBPLAYER
    if (!__osBbIsBb) {
        // On iQue these are set before the game runs
        __osBbEepromSize = 0x200;
        __osBbPakSize = 0x8000;
        __osBbFlashSize = 0x20000;
        __osBbEepromAddress = 0x803FFE00;
        __osBbPakAddress[0] = 0x803F7E00;
        __osBbPakAddress[1] = 0;
        __osBbPakAddress[2] = 0;
        __osBbPakAddress[3] = 0;
        __osBbFlashAddress = 0x803E0000;
        __osBbSramSize = 0x20000;
        __osBbSramAddress = 0x803E0000;
    } else {
        IO_WRITE(PI_BASE_REG + 0x64, IO_READ(PI_BASE_REG + 0x64) & 0x7FFFFFFF);
        IO_WRITE(MI_BASE_REG + 0x3C, 0x20000); // MI_HW_INTR_MASK_REG
        IO_WRITE(SI_BASE_REG + 0x0C, 0);
        IO_WRITE(SI_BASE_REG + 0x1C, (IO_READ(SI_BASE_REG + 0x1C) & 0x80FFFFFF) | 0x2F400000);
    }
#endif
    IO_WRITE(AI_CONTROL_REG, AI_CONTROL_DMA_ON);
    IO_WRITE(AI_DACRATE_REG, AI_MAX_DAC_RATE - 1);
    IO_WRITE(AI_BITRATE_REG, AI_MAX_BIT_RATE - 1);
}

void __osInitialize_autodetect() {
}

#ifdef BBPLAYER
#ident "$Revision: 1.1 $"
#endif
