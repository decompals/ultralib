/**************************************************************************
 *
 *  $Revision: 1.18 $
 *  $Date: 1997/02/11 08:26:14 $
 *  $Source: /hosts/liberte/disk6/Master/cvsmdev2/PR/include/os_internal.h,v $
 *
 **************************************************************************/

#ifndef _OS_INTERNAL_H_
#define	_OS_INTERNAL_H_

#ifdef _LANGUAGE_C_PLUS_PLUS
extern "C" {
#endif

#include "os.h"

#if defined(_LANGUAGE_C) || defined(_LANGUAGE_C_PLUS_PLUS)

#include "os_internal_reg.h"
#include "os_internal_exception.h"
#include "os_internal_flash.h"

/* Routine for global interrupt mask */
extern s32		__osLeoInterrupt(void);

/* Routines for fetch TLB info */

extern u32		__osGetTLBASID(void);
extern u32		__osGetTLBPageMask(s32);
extern u32		__osGetTLBHi(s32);
extern u32		__osGetTLBLo0(s32);
extern u32		__osGetTLBLo1(s32);

/* Serial interface (Si) */

extern u32 		__osSiGetStatus(void);
extern s32		__osSiRawWriteIo(u32, u32);
extern s32		__osSiRawReadIo(u32, u32 *);
extern s32		__osSiRawStartDma(s32, void *);

/* Signal processor interface (Sp) */

extern u32 		__osSpGetStatus(void);
extern void		__osSpSetStatus(u32);
extern s32		__osSpSetPc(u32);
extern s32		__osSpRawWriteIo(u32, u32);
extern s32		__osSpRawReadIo(u32, u32 *);
extern s32		__osSpRawStartDma(s32, u32, void *, u32);

/* Error handling */

extern void		__osError(s16, s16, ...);
extern OSThread *	__osGetCurrFaultedThread(void);
extern OSThread *	__osGetNextFaultedThread(OSThread *);

/* Development board functions */

extern void		__osGIOInit(s32);
extern void		__osGIOInterrupt(s32);
extern void		__osGIORawInterrupt(s32);

/* For debugger use */

extern OSThread *	__osGetActiveQueue(void);

/* Debug port */
extern void		__osSyncPutChars(int, int, const char *);
extern int		__osSyncGetChars(char *);
extern void		__osAsyncPutChars(int, int, const char *);
extern int		__osAsyncGetChars(char *);
extern int		__osAtomicInc(unsigned int *p);
extern int		__osAtomicDec(unsigned int *p);

/* routine for rdb port */
extern u32             __osRdbSend(u8 *buf, u32 size, u32 type);


#endif /* _LANGUAGE_C */

#ifdef _LANGUAGE_C_PLUS_PLUS
}
#endif

#endif /* !_OS_INTERNAL_H */
