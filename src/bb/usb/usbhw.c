#include "PR/os_internal.h"
#include "PR/bcp.h"

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
    u32 val = IO_READ(USB_REG_10_ALT(which));
    u32 mask = IO_READ(USB_REG_14(which));
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
                IO_WRITE(USB_REG_14(which), 0xAE);
            }
        }
        if ((val & 0x80) != 0) {
            __usbOtgStateChange(which);
        }
        IO_WRITE(USB_REG_10(which), val);
    }

    val = IO_READ(USB_REG_80(which));
    mask = IO_READ(USB_REG_84(which));
    val &= mask;

    if (_usb_ctlr_state[which].ucs_mode == 1) {
        //
    } else if (_usb_ctlr_state[which].ucs_mode == 2) {
        _usb_dci_vusb11_isr(__osArcDeviceHandle[which]);
    }

    IO_WRITE(MI_3C_REG, (which == 0) ? 0x200000 : 0x800000);
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

    IO_WRITE(USB_REG_1C(which), 4);

    __osBbDelay(1000);

    if (which != 0) {
        IO_WRITE(USB_REG_1C(1), 0xB4);
    } else {
        IO_WRITE(USB_REG_1C(0), 0x84);
    }

    __osBbDelay(500);

    IO_WRITE(USB_REG_14(which), 0xFF);

    IO_WRITE(MI_3C_REG, (which == 0) ? 0x200000 : 0x800000);
}

static void __usbOtgStateChange(s32 which) {
    u32 val = IO_READ(USB_REG_18_ALT(which));

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
    u32 addr = USB_REG_80000(which);

    IO_WRITE(MI_3C_REG, (which == 0) ? 0x100000 : 0x400000);

    IO_WRITE(USB_REG_94(which), 0);
    IO_WRITE(USB_REG_14(which), 0);
    IO_WRITE(USB_REG_8C(which), 0);
    IO_WRITE(USB_REG_84(which), 0);
    IO_WRITE(USB_REG_10(which), 0xFF);
    IO_WRITE(USB_REG_88(which), 0xFF);
    IO_WRITE(USB_REG_80(which), 0xFF);
    IO_WRITE(USB_REG_94(which), 2);
    IO_WRITE(USB_REG_98(which), 0);
    IO_WRITE(USB_REG_9C(which), (addr >> 0x08) & 0xFF);
    IO_WRITE(USB_REG_B0(which), (addr >> 0x10) & 0xFF);
    IO_WRITE(USB_REG_B4(which), (addr >> 0x18) & 0xFF);

    __usbOtgStateChange(which);

    __Usb_Reset_Count[which] = 0;
}

s32 __usbHwInit(void) {
    s32 i;

    IO_WRITE(USB_REG_40010(0), 1);
    IO_WRITE(USB_REG_40010(1), 1);

    for (i = 0; i < 2; i++) {
        __usbCtlrTest(i);
        __usbCtlrInit(i);
    }
    return i;
}
