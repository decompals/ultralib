#include <PR/os_internal.h>

void __assert(const char *arg0, const char *filename, int line) {
    osSyncPrintf("\nASSERTION FAULT: %s, %d: \"%s\"\n", filename, line, arg0);
}
