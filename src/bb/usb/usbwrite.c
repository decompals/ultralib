#include "PR/os_internal.h"
#include "PR/rcp.h"

#include "usb.h"

#ident "$Revision: 1.1 $"

s32 osBbUsbDevWrite(OSBbUsbHandle h, u8* buf, s32 len, u64 off) {
    _usb_ext_handle* uhp = (_usb_ext_handle*)h;
    __OSBbUsbMesg um;
    OSMesg msg = (OSMesg)&um;
    OSMesg rmsg;
    OSMesgQueue rq;
    s32 wtotal;

    if (uhp->uh_host_handle == NULL) {
        return -0xC6;
    }

    bzero(&um, sizeof(um));
    um.u.umrw.umrw_handle = h;
    um.um_rq = &rq;

    osCreateMesgQueue(&rq, &rmsg, 1);
    osWritebackDCache(buf, len);

    wtotal = 0;
    while (len > 0) {
        um.u.umrw.umrw_buffer = buf;
        um.u.umrw.umrw_len = len;
        um.u.umrw.umrw_offset = off;
        um.um_type = 5;

        osSendMesg(uhp->uh_mq, msg, OS_MESG_BLOCK);
        osRecvMesg(&rq, &msg, OS_MESG_BLOCK);

        wtotal = um.um_ret;
        if (um.um_ret >= 0) {
            wtotal = len;
        }
        len = 0;
    }
    return wtotal;
}
