#include "PR/os_internal.h"
#include "PR/bbfs.h"
#include "PR/os_bbcard.h"

// https://decomp.me/scratch/EHu6s

s32 osBbFRepairBlock(s32 fd, u32 off, void* buf, u32 len) {



    BbInode* temp_s6;
    s32 var_s1;
    s32 var_s2;
    s32 var_v0;
    u16 temp_v0;
    u32 var_s0;
    u32 var_s3;
    u32 var_a0;

    BbFat16* temp_s4;

    var_s1 = 0;
    if ((u32) fd >= 0x199U) {
        return -3;
    }


    var_v0 = __osBbFsGetAccess();
    if (var_v0 < 0) {
        return var_v0;

    }

    temp_s4 = __osBbFat;
    temp_s6 = &temp_s4->inode[fd];
    var_s2 = -3;
    if ((temp_s6->type != 0) && !(off & 0x3FFF)&& (off < temp_s6->size)) {
        if ((len == 0x4000) && (off + 0x4000 >= off) && (temp_s6->size >= off + 0x4000)) {
            var_s0 = temp_s6->block;

            for (var_a0 = 0; var_a0 < off / 0x4000; var_a0++) {
                var_s0 = temp_s4[var_s0 / 0x1000].entry[var_s0 % 0x1000];
            }

            if (var_s0 != 0) {
                if (var_s0 < (u32) (__osBbFsBlocks - 0x10)) {
                    var_s2 = osBbCardReadBlock(0U, var_s0, buf, NULL);
                    var_s3 = var_s0;
                    if (var_s2 >= 0) {

                    loop_14:
                        temp_v0 = __osBbFReallocBlock(temp_s6, var_s0, 0U);
                        if (temp_v0 == 0xFFFE) {
                            goto loop_14_end;
                        }
                        var_s0 = temp_v0;
                        var_s2 = osBbCardEraseBlock(0U, temp_v0);
                        if (var_s2 >= 0) {
                            var_s2 = osBbCardWriteBlock(0U, temp_v0, buf, NULL);
                            if (var_s2 >= 0) {
                                var_s2 = 0x4000;
                                var_s3 = 0xFFFE;
                                goto loop_14_end;
                            }
                        }

                        if (var_s2 == -2) {
                            temp_s4[var_s0 / 0x1000].entry[var_s0 % 0x1000] = -2;
                            goto loop_14;
                        }
                    
                        loop_14_end:;

                        if ((var_s3 != 0xFFFE) && (var_s0 != var_s3)) {
                            temp_s4[var_s3 / 0x1000].entry[var_s3 % 0x1000] = temp_s4[var_s0 / 0x1000].entry[var_s0 % 0x1000];
                            temp_s4[var_s0 / 0x1000].entry[var_s0 % 0x1000] = 0;

                            if (off == 0) {
                                temp_s6->block = var_s3;
                            } else {
                                var_s0 = temp_s6->block;

                                for (var_a0 = 0; var_a0 < off / 0x4000 - 1; var_a0++) {
                                    var_s0 = temp_s4[var_s0 / 0x1000].entry[var_s0 % 0x1000];
                                    
                                }
                                temp_s4[var_s0 / 0x1000].entry[var_s0 % 0x1000] = var_s3;
                            }
                        }
                        var_s1 = __osBbFsSync(0);
                    }
                }
            }
        }
    }
    __osBbFsRelAccess();
    var_v0 = var_s2;
    if (var_v0 == 0) {
        var_v0 = var_s1;
    }

    return var_v0;
}
