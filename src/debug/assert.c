#include "os.h"
#include "../os/osint.h"
#include "osint_debug.h"

void __assertBreak(void);

void __assert(const char* exp, const char* filename, int line) {
    osSyncPrintf("\nASSERTION FAULT: %s, %d: \"%s\"\n", filename, line, exp);
    __assertBreak;
}
