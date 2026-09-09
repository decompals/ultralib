#include "xstdio.h"
#include "string.h"
#include "PR/os_version.h"
#if defined(_DEBUG) && BUILD_VERSION <= VERSION_G
#include "G/os_globals.h"
#else
#include "os.h"
#endif

// TODO: this comes from a header
#ident "$Revision: 1.23 $"

static void* proutSprintf(void* s, const char* buf, size_t n);

int sprintf(char* s, const char* fmt, ...) {
    int ans;
    va_list ap;
    va_start(ap, fmt);
    ans = _Printf(proutSprintf, s, fmt, ap);
    if (ans >= 0) {
        s[ans] = 0;
    }
    return ans;
}
static void* proutSprintf(void* s, const char* buf, size_t n) {
    return (char*)memcpy(s, buf, n) + n;
}
