#ifndef _IDO_G_OS_GLOBALS_H
#define _IDO_G_OS_GLOBALS_H

/*
 * Reconstructed declaration surface from the monolithic 2.0G os.h. It is
 * kept separate for translation units whose historical include order cannot
 * include the complete header. This is not an original SDK header.
 */
extern unsigned long long osClockRate;
#ifdef _ULTRALIB_OS_VI_MODE_TYPE
extern OSViMode osViModeTable[];
extern OSViMode osViModeNtscLpn1;
extern OSViMode osViModeNtscLpf1;
extern OSViMode osViModeNtscLan1;
extern OSViMode osViModeNtscLaf1;
extern OSViMode osViModeNtscLpn2;
extern OSViMode osViModeNtscLpf2;
extern OSViMode osViModeNtscLan2;
extern OSViMode osViModeNtscLaf2;
extern OSViMode osViModeNtscHpn1;
extern OSViMode osViModeNtscHpf1;
extern OSViMode osViModeNtscHan1;
extern OSViMode osViModeNtscHaf1;
extern OSViMode osViModeNtscHpn2;
extern OSViMode osViModeNtscHpf2;
extern OSViMode osViModePalLpn1;
extern OSViMode osViModePalLpf1;
extern OSViMode osViModePalLan1;
extern OSViMode osViModePalLaf1;
extern OSViMode osViModePalLpn2;
extern OSViMode osViModePalLpf2;
extern OSViMode osViModePalLan2;
extern OSViMode osViModePalLaf2;
extern OSViMode osViModePalHpn1;
extern OSViMode osViModePalHpf1;
extern OSViMode osViModePalHan1;
extern OSViMode osViModePalHaf1;
extern OSViMode osViModePalHpn2;
extern OSViMode osViModePalHpf2;
extern OSViMode osViModeMpalLpn1;
extern OSViMode osViModeMpalLpf1;
extern OSViMode osViModeMpalLan1;
extern OSViMode osViModeMpalLaf1;
extern OSViMode osViModeMpalLpn2;
extern OSViMode osViModeMpalLpf2;
extern OSViMode osViModeMpalLan2;
extern OSViMode osViModeMpalLaf2;
extern OSViMode osViModeMpalHpn1;
extern OSViMode osViModeMpalHpf1;
extern OSViMode osViModeMpalHan1;
extern OSViMode osViModeMpalHaf1;
extern OSViMode osViModeMpalHpn2;
extern OSViMode osViModeMpalHpf2;
#else
extern char osViModeTable[];
extern char osViModeNtscLpn1[80];
extern char osViModeNtscLpf1[80];
extern char osViModeNtscLan1[80];
extern char osViModeNtscLaf1[80];
extern char osViModeNtscLpn2[80];
extern char osViModeNtscLpf2[80];
extern char osViModeNtscLan2[80];
extern char osViModeNtscLaf2[80];
extern char osViModeNtscHpn1[80];
extern char osViModeNtscHpf1[80];
extern char osViModeNtscHan1[80];
extern char osViModeNtscHaf1[80];
extern char osViModeNtscHpn2[80];
extern char osViModeNtscHpf2[80];
extern char osViModePalLpn1[80];
extern char osViModePalLpf1[80];
extern char osViModePalLan1[80];
extern char osViModePalLaf1[80];
extern char osViModePalLpn2[80];
extern char osViModePalLpf2[80];
extern char osViModePalLan2[80];
extern char osViModePalLaf2[80];
extern char osViModePalHpn1[80];
extern char osViModePalHpf1[80];
extern char osViModePalHan1[80];
extern char osViModePalHaf1[80];
extern char osViModePalHpn2[80];
extern char osViModePalHpf2[80];
extern char osViModeMpalLpn1[80];
extern char osViModeMpalLpf1[80];
extern char osViModeMpalLan1[80];
extern char osViModeMpalLaf1[80];
extern char osViModeMpalLpn2[80];
extern char osViModeMpalLpf2[80];
extern char osViModeMpalLan2[80];
extern char osViModeMpalLaf2[80];
extern char osViModeMpalHpn1[80];
extern char osViModeMpalHpf1[80];
extern char osViModeMpalHan1[80];
extern char osViModeMpalHaf1[80];
extern char osViModeMpalHpn2[80];
extern char osViModeMpalHpf2[80];
#endif
extern int osRomType;
extern void* osRomBase;
extern int osTvType;
extern int osResetType;
extern int osCicId;
extern int osVersion;
extern unsigned int osMemSize;
extern int osAppNMIBuffer[];
extern unsigned int __OSGlobalIntMask;
extern void* __osPiTable;
extern void* __osDiskHandle;
extern unsigned int osDiskExist;

#endif
