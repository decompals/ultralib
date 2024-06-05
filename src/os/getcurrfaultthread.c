#include "PR/os_internal.h"
#include "PR/osint.h"

OSThread* __osGetCurrFaultedThread() {
    return __osFaultedThread;
}
