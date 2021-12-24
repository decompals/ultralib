#ifndef _ULTRATYPES_H
#define _ULTRATYPES_H

#if defined(_LANGUAGE_C) || defined(_LANGUAGE_C_PLUS_PLUS)

typedef unsigned char       u8;     /* unsigned  8-bit */
typedef unsigned short      u16;    /* unsigned 16-bit */
typedef unsigned long       u32;    /* unsigned 32-bit */
typedef unsigned long long  u64;    /* unsigned 64-bit */

typedef signed char         s8;     /* signed  8-bit */
typedef short               s16;    /* signed 16-bit */
typedef long                s32;    /* signed 32-bit */
typedef long long           s64;    /* signed 64-bit */

typedef volatile unsigned char      vu8;    /* unsigned  8-bit */
typedef volatile unsigned short     vu16;   /* unsigned 16-bit */
typedef volatile unsigned long      vu32;   /* unsigned 32-bit */
typedef volatile unsigned long long vu64;   /* unsigned 64-bit */

typedef volatile signed char        vs8;    /* signed  8-bit */
typedef volatile short              vs16;   /* signed 16-bit */
typedef volatile long               vs32;   /* signed 32-bit */
typedef volatile long long          vs64;   /* signed 64-bit */

typedef float   f32;    /* single prec floating point */
typedef double  f64;    /* double prec floating point */

#if !defined(_SIZE_T) && !defined(_SIZE_T_) && !defined(_SIZE_T_DEF)
#define _SIZE_T
#define _SIZE_T_DEF         /* exeGCC size_t define label */
#if (_MIPS_SZLONG == 32)
typedef unsigned int    size_t;
#endif
#if (_MIPS_SZLONG == 64)
typedef unsigned long   size_t;
#endif
#endif

#endif  /* _LANGUAGE_C */

#ifndef TRUE
#define TRUE    1
#endif

#ifndef FALSE
#define FALSE   0
#endif

#ifndef NULL
#define NULL    0
#endif

#endif  /* _ULTRATYPES_H_ */
