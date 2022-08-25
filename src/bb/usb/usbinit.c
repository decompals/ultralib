#include "PR/os_internal.h"
#include "PR/rcp.h"
#include "PR/region.h"

#include "macros.h"

#include "usb.h"

#ident "$Revision: 1.1 $"

OSMesgQueue __osBbUsbCtlrQ[2];
void* __usb_svc_callback_reg;
void* __usb_endpt_desc_reg;
static OSThread __osBbUsbMgr[2];
static char __osBbUsbMgrStacks[0x4000];
static OSMesg __osBbUsbMesgs[256];
static __OSBbUsbMesg __osBbUsbEventMesg[2];
static u8 __usb_svc_callback_buffer[0x200];
static u8 __usb_endpt_desc_buffer[0x1800];

static s32 __osBbUsbInterrupt(s32 which, __OSBbUsbMesg* mp) {
    return __usbDevInterrupt(which, mp);
}

static s32 __osBbUsbRead(s32 which, __OSBbUsbMesg* ump) {
    _usb_ext_handle* uhp = (_usb_ext_handle*)ump->u.umrw.umrw_handle;

    if (uhp->uh_host_handle == NULL) {
        return -0xC6;
    }
    if (uhp->uh_rd_msg != NULL) {
        return -0xC7;
    }

    uhp->uh_rd_msg = ump;
    uhp->uh_rd_buffer = ump->u.umrw.umrw_buffer;
    uhp->uh_rd_offset = ump->u.umrw.umrw_offset;
    uhp->uh_rd_len = ump->u.umrw.umrw_len;

    __usbDevRead(uhp);
    return 0;
}

static s32 __osBbUsbWrite(s32 which, __OSBbUsbMesg* ump) {
    _usb_ext_handle* uhp = (_usb_ext_handle*)ump->u.umrw.umrw_handle;
    
    if (uhp->uh_host_handle == NULL) {
        return -0xC6;
    }
    if (uhp->uh_wr_msg != NULL) {
        return -0xC7;
    }

    uhp->uh_wr_msg = ump;
    uhp->uh_wr_buffer = ump->u.umrw.umrw_buffer;
    uhp->uh_wr_offset = ump->u.umrw.umrw_offset;
    uhp->uh_wr_len = ump->u.umrw.umrw_len;

    __usbDevWrite(uhp);
    return 0;
}

static void __osBbUsbMgrProc(char* arg) {
    OSMesg msg;
    __OSBbUsbMesg* mp;
    s32 ret;
    s32 which = osGetThreadId(NULL) - 0xC45;

    while (TRUE) {
        osRecvMesg((OSMesgQueue*)arg, &msg, OS_MESG_BLOCK);
        mp = (__OSBbUsbMesg*)msg;

        switch (mp->um_type) {
            case 0:
            case 1:
                ret = __osBbUsbInterrupt(which, mp);
                break;
            case 4:
                ret = __osBbUsbRead(which, mp);
                break;
            case 5:
                ret = __osBbUsbWrite(which, mp);
                break;
            default:
                ret = 0xC9;
                break;
        }

        if (ret != 0) {
            mp->um_ret = ret;
            mp->um_type |= 0x80;
            osSendMesg(mp->um_rq, mp, OS_MESG_NOBLOCK);
        }
    }
}

static s32 __osBbUsbThreadInit(s32 which) {
    if ((u32)which >= 2) {
        return -1;
    }

    osCreateMesgQueue(&__osBbUsbCtlrQ[which], &__osBbUsbMesgs[which * 128], 128);

    osCreateThread(&__osBbUsbMgr[which], 0xC45 + which, (void*)__osBbUsbMgrProc, 
            &__osBbUsbCtlrQ[which], 
            &__osBbUsbMgrStacks[(which + 1) * 0x2000], 
            (which == 1) ? 0x000000E6 : 0x000000E8);
    osStartThread(&__osBbUsbMgr[which]);

    bzero(&__osBbUsbEventMesg[which], 0x28);
    __osBbUsbEventMesg[which].um_type = which != 0;

    osSetEventMesg((which != 0) ? 0x1C : 0x1B, &__osBbUsbCtlrQ[which], &__osBbUsbEventMesg[which]);
    return 0;
}

s32 osBbUsbSetCtlrModes(s32 which, u32 mask) {
    if ((u32)which >= 2) {
        return -1;
    }
    _usb_ctlr_state[which].ucs_mask = mask;
    _usb_ctlr_state[which].ucs_mode &= ~mask;
    return 0;
}

s32 osBbUsbInit(void) {
    s32 i;
    s32 numctlr = 0;

    __usb_svc_callback_reg = osCreateRegion(__usb_svc_callback_buffer, ARRLEN(__usb_svc_callback_buffer), 0xC, 0);
    __usb_endpt_desc_reg = osCreateRegion(__usb_endpt_desc_buffer, ARRLEN(__usb_endpt_desc_buffer), 0x600, 0);
    
    for (i = 0; i < 2; i++) {
        if (__osBbUsbThreadInit(i) == 0) {
            numctlr++;
        }
    }

    __usbHwInit();

    return numctlr;
}
