#ifndef USB_H_
#define USB_H_

typedef char* char_ptr;
typedef unsigned char uchar;
typedef unsigned char* uchar_ptr;
typedef signed char int_8;
typedef signed char* int_8_ptr;
typedef unsigned char uint_8;
typedef unsigned char* uint_8_ptr;
typedef short int int_16;
typedef short int* int_16_ptr;
typedef short unsigned int uint_16;
typedef short unsigned int* uint_16_ptr;
typedef long int int_32;
typedef long int* int_32_ptr;
typedef long unsigned int uint_32;
typedef long unsigned int* uint_32_ptr;
typedef long unsigned int boolean;
typedef void* pointer;
typedef float ieee_single;
typedef double ieee_double;

typedef pointer _usb_host_handle;
typedef pointer _usb_device_handle;

typedef struct _usb_ctlr_state_s /* size=0x8 */ {
    /* 0x0000 */ int ucs_mode;
    /* 0x0004 */ int ucs_mask;
} _usb_ctlr_state_t;

typedef struct OSBbUsbInfo_s /* size=0x1C */ {
    /* 0x0000 */ u8 ua_type;
    /* 0x0001 */ u8 ua_class;
    /* 0x0002 */ u8 ua_subclass;
    /* 0x0003 */ u8 ua_protocol;
    /* 0x0004 */ u16 ua_vendor;
    /* 0x0006 */ u16 ua_product;
    /* 0x0008 */ u8 ua_cfg;
    /* 0x0009 */ u8 ua_ifc;
    /* 0x000A */ u8 ua_ep;
    /* 0x000B */ u8 ua_speed;
    /* 0x000C */ u8 ua_mode;
    /* 0x000D */ u8 ua_blksize;
    /* 0x000E */ u8 ua_support;
    /* 0x000F */ u8 ua_state;
    /* 0x0010 */ u8* ua_mfr_str;
    /* 0x0014 */ u8* ua_prod_str;
    /* 0x0018 */ u8* ua_driver_name;
} OSBbUsbInfo;

typedef void* OSBbUsbHandle;

typedef struct __OSBbUsbMesg_s /* size=0x28 */ {
    /* 0x0000 */ u8 um_type;
    /* 0x0008 */ union /* size=0x18 */ {
        /* 0x0000 */ struct /* size=0x8 */ {
            /* 0x0000 */ OSBbUsbInfo* umq_info;
            /* 0x0004 */ s32 umq_ninfo;
        } umq;
        /* 0x0000 */ struct /* size=0x8 */ {
            /* 0x0000 */ OSBbUsbInfo* umh_info;
            /* 0x0004 */ OSBbUsbHandle umh_handle;
        } umh;
        /* 0x0000 */ struct /* size=0x18 */ {
            /* 0x0000 */ OSBbUsbHandle umrw_handle;
            /* 0x0004 */ u8* umrw_buffer;
            /* 0x0008 */ s32 umrw_len;
            /* 0x0010 */ u64 umrw_offset;
        } umrw;
    } u;
    /* 0x0020 */ s32 um_ret;
    /* 0x0024 */ OSMesgQueue* um_rq;
} __OSBbUsbMesg;

typedef struct _usb_ext_handle_s /* size=0x40 */ {
    /* 0x0000 */ s32 uh_which;
    /* 0x0004 */ s32 uh_blksize;
    /* 0x0008 */ OSMesgQueue* uh_mq;
    /* 0x000C */ _usb_host_handle uh_host_handle;
    /* 0x0010 */ __OSBbUsbMesg* uh_wr_msg;
    /* 0x0014 */ u8* uh_wr_buffer;
    /* 0x0018 */ u64 uh_wr_offset;
    /* 0x0020 */ s32 uh_wr_len;
    /* 0x0024 */ __OSBbUsbMesg* uh_rd_msg;
    /* 0x0028 */ u8* uh_rd_buffer;
    /* 0x0030 */ u64 uh_rd_offset;
    /* 0x0038 */ s32 uh_rd_len;
} _usb_ext_handle;

typedef struct /* size=0x18 */ {
    /* 0x0000 */ void (*reset_ep0)();
    /* 0x0004 */ void (*get_desc)();
    /* 0x0008 */ void (*vendor)();
    /* 0x000C */ void (*initeps)();
    /* 0x0010 */ void (*query)(void*);
    /* 0x0014 */ void (*stall_ep)();
} usbdevfuncs;

typedef struct dev_global_struct_s /* size=0x8 */ {
    /* 0x0000 */ uint_8 dev_state;
    /* 0x0001 */ uint_8 FIRST_SOF;
    /* 0x0002 */ uint_8 num_ifcs;
    /* 0x0004 */ usbdevfuncs* funcs;
} DEV_GLOBAL_STRUCT, *DEV_GLOBAL_STRUCT_PTR;

extern _usb_ctlr_state_t _usb_ctlr_state[2];

extern DEV_GLOBAL_STRUCT dev_global_struct;

extern _usb_host_handle __osArcHostHandle[2];

#endif
