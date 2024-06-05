#include "PR/os_internal.h"
#include "PR/osint.h"

OSId osGetThreadId(OSThread* thread) {
    if (thread == NULL) {
        thread = __osRunningThread;
    }

    return thread->id;
}
