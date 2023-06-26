#ifndef __MACROS_H__
#define __MACROS_H__

#ifndef BBPLAYER
# define ALIGNED(x) __attribute__((aligned(x)))
# define BBALIGNED(x) ALIGNED(x)
#else
# define ALIGNED(x)
# define BBALIGNED(x) __attribute__((aligned(x)))
#endif

#if defined(__GNUC__) && (__GNUC__ == 2) && (__GNUC_MINOR__ == 91)
#define __EGCS__
#endif

#define ALIGN(x, n) (((x) + ((n) - 1)) & ~((n) -1))

#define ARRLEN(x) ((s32)(sizeof(x) / sizeof(x[0])))

#define STUBBED_PRINTF(x) ((void)(x))

#define UNUSED __attribute__((unused))

#ifndef __GNUC__
#define __attribute__(x)
#endif

#ifndef MAX
#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif

#endif
