#include "stdarg.h"
#include "PR/os.h"

void __osSyncVPrintf(const char *fmt, va_list args) {
    // these functions intentionally left blank.  ifdeffed out in rom release
}

void osSyncPrintf(const char *fmt, ...) {
    int ans;
    va_list ap;
    // these functions intentionally left blank.  ifdeffed out in rom release
}

void rmonPrintf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    _Printf(osSyncPrintf, NULL, fmt, args);
    va_end(args);
}
