/*---------------------------------------------------------------------*
      
        $RCSfile: os_internal_exception.h,v $
        $Revision: 1.1 $
        $Date: 1998/10/09 08:01:10 $
 *---------------------------------------------------------------------*/

#ifndef _OS_INTERNAL_EXCEPTION_H_
#define	_OS_INTERNAL_EXCEPTION_H_

#ifdef _LANGUAGE_C_PLUS_PLUS
extern "C" {
#endif

#include <PR/os.h>

#if defined(_LANGUAGE_C) || defined(_LANGUAGE_C_PLUS_PLUS)

/* Routine for HW interrupt "handler" */
extern void 		__osSetHWIntrRoutine(OSHWIntr interrupt,
					     s32 (*handler)(void), void *stackEnd);
extern void 		__osGetHWIntrRoutine(OSHWIntr interrupt,
					     s32 (**handler)(void), void **stackEnd);

/* Routine for global interrupt mask */
extern void		__osSetGlobalIntMask(OSHWIntr);
extern void		__osResetGlobalIntMask(OSHWIntr);


#endif /* _LANGUAGE_C */

#ifdef _LANGUAGE_C_PLUS_PLUS
}
#endif

#endif /* !_OS_INTERNAL_EXCEPTION_H */
