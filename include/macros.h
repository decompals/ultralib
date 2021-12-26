#ifndef __MACROS_H__
#define __MACROS_H__

#define ALIGNED(x) __attribute__((aligned(x)))

#define ARRLEN(x) ((s32)(sizeof(x) / sizeof(x[0])))

#endif
