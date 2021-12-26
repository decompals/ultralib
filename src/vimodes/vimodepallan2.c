#include "PR/os.h"
#include "PR/rcp.h"
#include "../io/viint.h"

OSViMode osViModePalLan2 = {
    OS_VI_PAL_LAN2,  // type
    { // comRegs
        VI_CTRL_TYPE_32 | VI_CTRL_GAMMA_DITHER_ON | VI_CTRL_GAMMA_ON |
            VI_CTRL_DIVOT_ON | 0x3000,  // ctrl
        WIDTH(320),                     // width
        BURST(58, 30, 4, 69),           // burst
        VSYNC(625),                     // vSync
        HSYNC(3177, 23),                // hSync
        LEAP(3183, 3181),               // leap
        HSTART(128, 768),               // hStart
        SCALE(2, 0),                    // xScale
        VCURRENT(0),                    // vCurrent
    },
    { // fldRegs
        { // [0]
            ORIGIN(1280),         // origin
            SCALE(1, 0),          // yScale
            HSTART(95, 569),      // vStart
            BURST(107, 2, 9, 0),  // vBurst
            VINTR(2),             // vIntr
        },
        { // [1]
            ORIGIN(1280),         // origin
            SCALE(1, 0),          // yScale
            HSTART(95, 569),      // vStart
            BURST(107, 2, 9, 0),  // vBurst
            VINTR(2),             // vIntr
        }
    }
};
