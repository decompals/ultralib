#include "PR/os_internal.h"
#include "PR/rcp.h"

#include "usb.h"

#ident "$Revision: 1.1 $"

_usb_ctlr_state_t _usb_ctlr_state[2] = {
    { 0, 3 },
    { 0, 3 },
};
static int save_im_level = 0;
_usb_host_handle __osArcHostHandle[2];
static _usb_device_handle __osArcDeviceHandle[2];
static OSIntMask save_im;
u32 __Usb_Reset_Count[2];

void __usb_splhigh(void) {
    OSIntMask im = __osDisableInt();

    save_im_level++;
    if (save_im_level == 1) {
        save_im = im;
    }
}

void __usb_splx(void) {
    save_im_level--;
    if (save_im_level == 0) {
        __osRestoreInt(save_im);
    }
}

void _usb_dci_vusb11_isr(_usb_device_handle);

s32 __usbDevInterrupt(s32 which) {
    u32 val = IO_READ((which != 0) ? 0x04A00010 : 0x04900010);
    u32 mask = IO_READ((which == 0) ? 0x04900014 : 0x04A00014);
    val &= mask;

    if (val != 0) {
        if (val & 1) {
            __usbOtgStateChange(which);
        }
        if ((val & 4) != 0) {
            __usbOtgStateChange(which);
        }
        if ((val & 8) != 0) {
            __usbOtgStateChange(which);
        }
        if ((val & 0x20) != 0) {
            __usbOtgStateChange(which);
        }
        if ((val & 0x40) != 0) {
            static int msec_count = 0;

            msec_count++;
            if (msec_count >= 5) {
                IO_WRITE((which == 0) ? 0x04900014 : 0x04A00014, 0xAE);
            }
        }
        if ((val & 0x80) != 0) {
            __usbOtgStateChange(which);
        }
        IO_WRITE((which == 0) ? 0x04900010 : 0x04A00010, val);
    }

    val = IO_READ((which == 0) ? 0x04900080 : 0x04A00080);
    mask = IO_READ((which == 0) ? 0x04900084 : 0x04A00084);
    val &= mask;

    if (_usb_ctlr_state[which].ucs_mode == 1) {
        //
    } else if (_usb_ctlr_state[which].ucs_mode == 2) {
        _usb_dci_vusb11_isr(__osArcDeviceHandle[which]);
    }

    IO_WRITE(MI_BASE_REG + 0x3C, (which == 0) ? 0x00200000 : 0x00800000);
    return 0;
}

void __usbDevRead(_usb_ext_handle* uhp) {
}

void __usbDevWrite(_usb_ext_handle* uhp) {
}

static void __usbCtlrTest(s32 which) {
}

static void __usbHostMode(s32 which) {
}

void __usb_arc_device_setup(s32, _usb_device_handle*);

static void __usbDeviceMode(s32 which) {
    __usb_arc_device_setup(which, &__osArcDeviceHandle[which]);

    IO_WRITE((which == 0) ? 0x0490001C : 0x04A0001C, 4);

    __osBbDelay(1000);

    if (which != 0) {
        IO_WRITE(0x04A0001C, 0xB4);
    } else {
        IO_WRITE(0x0490001C, 0x84);
    }

    __osBbDelay(500);

    IO_WRITE((which == 0) ? 0x04900014 : 0x04A00014, 0xFF);

    IO_WRITE(MI_BASE_REG + 0x3C, (which == 0) ? 0x00200000 : 0x00800000);
}

static void __usbOtgStateChange(s32 which) {
    u32 val = IO_READ((which != 0) ? 0x04A00018 : 0x04900018);

    if (_usb_ctlr_state[which].ucs_mask == 0) {
        _usb_ctlr_state[which].ucs_mode = 0;
    } else if (which != 0) {
        if (_usb_ctlr_state[which].ucs_mask & 2) {
            if (_usb_ctlr_state[which].ucs_mode != 2) {
                _usb_ctlr_state[which].ucs_mode = 2;
                __usbDeviceMode(which);
            }
        }
    } else {
        if (!(val & 0x80)) {
            if (_usb_ctlr_state[which].ucs_mask & 1) {
                if (_usb_ctlr_state[which].ucs_mode != 1) {
                    _usb_ctlr_state[which].ucs_mode = 1;
                    __usbHostMode(which);
                }
            }
        } else {
            if (_usb_ctlr_state[which].ucs_mask & 2) {
                if (_usb_ctlr_state[which].ucs_mode != 2) {
                    _usb_ctlr_state[which].ucs_mode = 2;
                    __usbDeviceMode(which);
                }
            }
        }
    }
}

s32 osBbUsbGetResetCount(s32 which) {
    return __Usb_Reset_Count[which];
}

static void __usbCtlrInit(s32 which) {
    u32 addr = (which != 0) ? 0x04A80000 : 0x04980000;

    IO_WRITE(MI_BASE_REG + 0x3C, (which == 0) ? 0x00100000 : 0x00400000);

    IO_WRITE((which == 0) ? 0x04900094 : 0x04A00094, 0);
    IO_WRITE((which == 0) ? 0x04900014 : 0x04A00014, 0);
    IO_WRITE((which == 0) ? 0x0490008C : 0x04A0008C, 0);
    IO_WRITE((which == 0) ? 0x04900084 : 0x04A00084, 0);
    IO_WRITE((which == 0) ? 0x04900010 : 0x04A00010, 0xFF);
    IO_WRITE((which == 0) ? 0x04900088 : 0x04A00088, 0xFF);
    IO_WRITE((which == 0) ? 0x04900080 : 0x04A00080, 0xFF);
    IO_WRITE((which == 0) ? 0x04900094 : 0x04A00094, 2);
    IO_WRITE((which == 0) ? 0x04900098 : 0x04A00098, 0);
    IO_WRITE((which == 0) ? 0x0490009C : 0x04A0009C, (addr >> 0x08) & 0xFF);
    IO_WRITE((which == 0) ? 0x049000B0 : 0x04A000B0, (addr >> 0x10) & 0xFF);
    IO_WRITE((which == 0) ? 0x049000B4 : 0x04A000B4, (addr >> 0x18) & 0xFF);

    __usbOtgStateChange(which);

    __Usb_Reset_Count[which] = 0;
}

s32 __usbHwInit(void) {
    s32 i;

    IO_WRITE(0x04940010, 1);
    IO_WRITE(0x04A40010, 1);

    for (i = 0; i < 2; i++) {
        __usbCtlrTest(i);
        __usbCtlrInit(i);
    }
    return i;
}
