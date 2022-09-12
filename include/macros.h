#ifndef __MACROS_H__
#define __MACROS_H__

#define ALIGNED(x) __attribute__((aligned(x)))

#define ALIGN(x, n) (((x) + ((n) - 1)) & ~((n) -1))

#define ARRLEN(x) ((s32)(sizeof(x) / sizeof(x[0])))

#ifndef MAX
#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif

#endif
