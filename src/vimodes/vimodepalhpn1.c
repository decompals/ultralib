#include "PR/os.h"
#include "PR/rcp.h"
#include "../io/viint.h"

OSViMode osViModePalHpn1 = { OS_VI_PAL_HPN1, // type
                             {
                                 // comRegs
                                 VI_CTRL_TYPE_16 | VI_CTRL_GAMMA_DITHER_ON | VI_CTRL_GAMMA_ON | VI_CTRL_SERRATE_ON |
                                     VI_CTRL_ANTIALIAS_MODE_2 | VI_CTRL_PIXEL_ADV_3, // ctrl
                                 WIDTH(1280),                                        // width
                                 BURST(58, 30, 4, 69),                               // burst
                                 VSYNC(624),                                         // vSync
                                 HSYNC(3177, 23),                                    // hSync
                                 LEAP(3183, 3181),                                   // leap
                                 HSTART(128, 768),                                   // hStart
                                 SCALE(1, 0),                                        // xScale
                                 VCURRENT(0),                                        // vCurrent
                             },
                             { // fldRegs
                               {
                                   // [0]
                                   ORIGIN(1280),        // origin
                                   SCALE(1, 0),         // yScale
                                   HSTART(93, 567),     // vStart
                                   BURST(107, 2, 9, 0), // vBurst
                                   VINTR(2),            // vIntr
                               },
                               {
                                   // [1]
                                   ORIGIN(2560),         // origin
                                   SCALE(1, 0),          // yScale
                                   HSTART(95, 569),      // vStart
                                   BURST(105, 2, 13, 0), // vBurst
                                   VINTR(2),             // vIntr
                               } } };
