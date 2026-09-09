#ifndef _IDO_G_VIMODE_H
#define _IDO_G_VIMODE_H

#define _ULTRALIB_G_NO_OS_INTERNAL_DECLS
#include "PR/rcp.h"
#undef _ULTRALIB_G_NO_OS_INTERNAL_DECLS

typedef struct {
    u32 ctrl;
    u32 width;
    u32 burst;
    u32 vSync;
    u32 hSync;
    u32 leap;
    u32 hStart;
    u32 xScale;
    u32 vCurrent;
} OSViCommonRegs;

typedef struct {
    u32 origin;
    u32 yScale;
    u32 vStart;
    u32 vBurst;
    u32 vIntr;
} OSViFieldRegs;

typedef struct {
    u8 type;
    OSViCommonRegs comRegs;
    OSViFieldRegs fldRegs[2];
} OSViMode;

#define OS_VI_NTSC_LPN1 0
#define OS_VI_NTSC_LPF1 1
#define OS_VI_NTSC_LAN1 2
#define OS_VI_NTSC_LAF1 3
#define OS_VI_NTSC_LPN2 4
#define OS_VI_NTSC_LPF2 5
#define OS_VI_NTSC_LAN2 6
#define OS_VI_NTSC_LAF2 7
#define OS_VI_NTSC_HPN1 8
#define OS_VI_NTSC_HPF1 9
#define OS_VI_NTSC_HAN1 10
#define OS_VI_NTSC_HAF1 11
#define OS_VI_NTSC_HPN2 12
#define OS_VI_NTSC_HPF2 13

#define OS_VI_PAL_LPN1 14
#define OS_VI_PAL_LPF1 15
#define OS_VI_PAL_LAN1 16
#define OS_VI_PAL_LAF1 17
#define OS_VI_PAL_LPN2 18
#define OS_VI_PAL_LPF2 19
#define OS_VI_PAL_LAN2 20
#define OS_VI_PAL_LAF2 21
#define OS_VI_PAL_HPN1 22
#define OS_VI_PAL_HPF1 23
#define OS_VI_PAL_HAN1 24
#define OS_VI_PAL_HAF1 25
#define OS_VI_PAL_HPN2 26
#define OS_VI_PAL_HPF2 27

#define OS_VI_MPAL_LPN1 28
#define OS_VI_MPAL_LPF1 29
#define OS_VI_MPAL_LAN1 30
#define OS_VI_MPAL_LAF1 31
#define OS_VI_MPAL_LPN2 32
#define OS_VI_MPAL_LPF2 33
#define OS_VI_MPAL_LAN2 34
#define OS_VI_MPAL_LAF2 35
#define OS_VI_MPAL_HPN1 36
#define OS_VI_MPAL_HPF1 37
#define OS_VI_MPAL_HAN1 38
#define OS_VI_MPAL_HAF1 39
#define OS_VI_MPAL_HPN2 40
#define OS_VI_MPAL_HPF2 41

#define BURST(hsync_width, color_width, vsync_width, color_start) \
    (((u32)(hsync_width) & 0xFF) | (((u32)(color_width) & 0xFF) << 8) | (((u32)(vsync_width) & 0xF) << 16) | \
     (((u32)(color_start) & 0xFFFF) << 20))
#define WIDTH(v) (v)
#define VSYNC(v) (v)
#define HSYNC(duration, leap) (((u32)(leap) << 16) | ((u32)(duration) & 0xFFFF))
#define LEAP(upper, lower) (((u32)(upper) << 16) | ((u32)(lower) & 0xFFFF))
#define PAL_BURST BURST(58, 35, 4, 64)
#define PAL_HSYNC HSYNC(3177, 21)
#define PAL_LEAP LEAP(3183, 3182)
#define START(start, end) (((u32)(start) << 16) | ((u32)(end) & 0xFFFF))
#define FTOFIX(val, i, f) ((u32)((val) * (f32)(1 << (f))) & ((1 << ((i) + (f))) - 1))
#define F210(val) FTOFIX(val, 2, 10)
#define SCALE(scaleup, off) (F210((1.0f / (f32)(scaleup))) | (F210((f32)(off)) << 16))
#define VCURRENT(v) v
#define ORIGIN(v) v
#define VINTR(v) v
#define HSTART START

#define _ULTRALIB_OS_VI_MODE_TYPE
#include "G/os_globals.h"
#undef _ULTRALIB_OS_VI_MODE_TYPE

#endif
