#include "PR/os_version.h"

#if BUILD_VERSION <= VERSION_G
void rmonPrintf(const char* fmt, ...) {
}
#endif
