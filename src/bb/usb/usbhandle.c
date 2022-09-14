#include "PR/os_internal.h"
#include "PR/rcp.h"

#include "usb.h"

#ident "$Revision: 1.1 $"

_usb_ext_handle _usb_ext_handles[4];

s32 osBbUsbDevGetHandle(s32 which, OSBbUsbInfo* ip, OSBbUsbHandle* hp) {
    return -1;
}

s32 osBbUsbDevFreeHandle(OSBbUsbHandle h) {
    _usb_ext_handle* uhp = (_usb_ext_handle*)h;

    bzero(uhp, sizeof(*uhp));
    return 0;
}

void __usbInvalidateHandles(s32 which) {
    int i;

    if (which >= 2u) {
        return;
    }

    for (i = 0; i < 4; i++) {
        if (_usb_ext_handles[i].uh_host_handle == __osArcHostHandle[which]) {
            _usb_ext_handles[i].uh_host_handle = NULL;
        }
    }
}
