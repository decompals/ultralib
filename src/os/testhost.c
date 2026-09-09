#include "PR/os_version.h"
#if defined(_DEBUG) && BUILD_VERSION <= VERSION_G
#include "G/os_internal_compat.h"
#define _ULTRALIB_G_RMONBRK
#include "PRinternal/rmonint.h"
#undef _ULTRALIB_G_RMONBRK
#else
#include "PR/os_internal.h"
#endif

// TODO: this comes from a header
#if BUILD_VERSION >= VERSION_J
#ident "$Revision: 1.4 $"
#else
#ident "$Revision: 3.70 $"
#ident "$Revision: 1.5 $"
#ident "$Revision: 1.2 $"
#ident "$Revision: 1.4 $"
#ident "$Revision: 1.3 $"
#endif
