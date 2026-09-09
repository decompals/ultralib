/**
 * @file vimodepallpn1.c
 *
 * PAL LPN1 Video Mode
 *
 * L = Low Resolution
 * P = Point Sampled
 * N = Non-Interlaced
 * 1 = 16-bit Framebuffer
 */
#include "PR/os_version.h"
#if defined(_DEBUG) && BUILD_VERSION <= VERSION_G
#include "G/vimode.h"
#else
#include "PR/os.h"
#include "PR/rcp.h"
#include "PRinternal/viint.h"
#endif

OSViMode osViModePalLpn1 = {
    OS_VI_PAL_LPN1, // type
    {
        // comRegs
        VI_CTRL_TYPE_16 | VI_CTRL_GAMMA_DITHER_ON | VI_CTRL_GAMMA_ON | VI_CTRL_ANTIALIAS_MODE_2 |
            VI_CTRL_PIXEL_ADV_3, // ctrl
        WIDTH(320),              // width
        PAL_BURST,    // burst
        VSYNC(625),              // vSync
        PAL_HSYNC,         // hSync
        PAL_LEAP,        // leap
        HSTART(128, 768),        // hStart
        SCALE(2, 0),             // xScale
        VCURRENT(0),             // vCurrent
    },
    { // fldRegs
      {
          // [0]
          ORIGIN(640),         // origin
          SCALE(1, 0),         // yScale
          HSTART(95, 569),     // vStart
          BURST(107, 2, 9, 0), // vBurst
          VINTR(2),            // vIntr
      },
      {
          // [1]
          ORIGIN(640),         // origin
          SCALE(1, 0),         // yScale
          HSTART(95, 569),     // vStart
          BURST(107, 2, 9, 0), // vBurst
          VINTR(2),            // vIntr
      } },
};
