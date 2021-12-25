#ifndef _OS_SYSTEM_H_
#define _OS_SYSTEM_H_

#ifdef _LANGUAGE_C_PLUS_PLUS
extern "C" {
#endif

#include "ultratypes.h"
#include "os_exception.h"

/*
 * Values for osTvType 
 */
#define OS_TV_PAL   0
#define OS_TV_NTSC  1
#define OS_TV_MPAL  2

/*
 * Size of buffer the retains contents after NMI
 */
#define OS_APP_NMI_BUFSIZE  64

#if defined(_LANGUAGE_C) || defined(_LANGUAGE_C_PLUS_PLUS)

extern s32   osRomType;     /* Bulk or cartridge ROM. 0=cartridge 1=bulk */
extern void *osRomBase;     /* Rom base address of the game image */
extern s32   osTvType;      /* 0 = PAL, 1 = NTSC, 2 = MPAL */
extern s32   osResetType;   /* 0 = cold reset, 1 = NMI */
extern s32   osCicId;
extern s32   osVersion;
extern u32   osMemSize;     /* Memory Size */
extern s32   osAppNMIBuffer[OS_APP_NMI_BUFSIZE/sizeof(s32)];

extern u64  osClockRate;

extern OSIntMask __OSGlobalIntMask; /* global interrupt mask */

#define osInitialize()          \
    __osInitialize_common();    \
    __osInitialize_autodetect()

void __osInitialize_common(void);
void __osInitialize_autodetect(void);

extern void     osExit(void);
extern u32      osGetMemSize(void);

/* pre-NMI */
extern s32      osAfterPreNMI(void);

#endif  /* defined(_LANGUAGE_C) || defined(_LANGUAGE_C_PLUS_PLUS) */

#ifdef _LANGUAGE_C_PLUS_PLUS
}
#endif

#endif /* !_OS_SYSTEM_H_ */
