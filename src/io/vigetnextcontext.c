#include "PR/os_internal.h"
#include "PR/viint.h"

__OSViContext* __osViGetNextContext(void) {
    return __osViNext;
}
