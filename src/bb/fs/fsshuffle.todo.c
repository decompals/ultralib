#include "PR/os_internal.h"
#include "PR/bbfs.h"
#include "PR/os_bbcard.h"

s32 osBbFShuffle(s32 sfd, s32 dfd, s32 release, void* buf, u32 len) {
    BbFat16* temp_s6;
    BbInode* temp_fp;
    BbInode* temp_v0_2;
    s32 sp60;
    s32 temp_t0;
    s32 var_a1;
    s32 var_s1;
    s32 var_s2;
    s32 var_s5;
    u16 temp_a0;
    u16 var_s0;
    s32 var_s4;
    s32 temp_s3;

    u16 sp18[32];

    if (((u32) sfd >= 0x199U) || (dfd < 0) || (dfd >= 0x199)) {
        return -3;
    }

    var_s1 = __osBbFsGetAccess();
    if (var_s1 < 0) {
        return var_s1;
    }

    var_s1 = -3;
    temp_s6 = __osBbFat;
    temp_v0_2 = &temp_s6->inode[sfd];
    temp_fp = &temp_s6->inode[dfd];
    if ((temp_v0_2->type != 0) && (temp_fp->type != 0) && (len != 0)) {
        if (!(len & 0x3FFF) && (temp_v0_2->size >= len)) {
            temp_s3 = len / 0x4000;
            for (sp60 = 0; sp60 < temp_s3; sp60 += 0x20) {
                var_s5 = temp_s3 - sp60;
                if (var_s5 > 0x20) {
                    var_s5 = 0x20;
                }

                var_a1 = temp_fp->block;
                var_s4 = var_a1;
                while (var_a1 != 0xFFFF) {
                    var_s4 = var_a1;
                    var_a1 = temp_s6[var_s4 >> 0xC].entry[var_s4 & 0xFFF];
                }
                var_a1 = var_s4 + 1;
                if (var_s4 == 0xFFFF) {
                    var_a1 = 0x40;
                }
                temp_t0 = var_a1;

                for (var_s2 = 0; var_s2 < var_s5; var_s2++) {
                    while (temp_s6[var_a1 >> 0xC].entry[var_a1 & 0xFFF] != 0) {
                        var_a1 += 1;
                        if (var_a1 >= __osBbFsBlocks) {
                            var_a1 = 0x40;
                        }
                        if (var_a1 == temp_t0) {
                            var_s1 = -7;
                            goto end;
                        }
                    }

                    sp18[var_s2] = var_a1;
                    var_a1 += 1;
                }

                if (((var_s1 = osBbCardEraseBlocks(0U, sp18,  var_s5)) < 0) || (var_s1 = osBbCardWriteBlocks(0U, sp18, var_s5, buf, NULL), (var_s1 < 0))) {
                    if (var_s1 != -2) {
                        goto end;
                    }

                    for (var_s2 = 0; var_s2 < var_s5; var_s2++) {
                        var_s0 = sp18[var_s2];
loop_36:
                        if (((var_s1 = osBbCardEraseBlock(0U, var_s0)) < 0) || ((var_s1 = osBbCardWriteBlock(0U, var_s0, buf + sp60 * 0x4000, NULL)) < 0)) {
                            if (var_s1 != -2) {
                                goto end;
                            }
    
                            if ((var_s0 = __osBbFReallocBlock(temp_fp, var_s0, 0xFFFE)) == 0xFFFE) {
                                goto end;
                            }
                            goto loop_36;
                        }

                        sp18[var_s2] = var_s0;
                    }
                }

                var_s2 = 0;
                if (var_s4 == 0xFFFF) {
                    var_s2 = 1;
                    temp_fp->block = sp18[0];
                    var_s4 = sp18[0];
                }

                while (var_s2 < var_s5) {
                    temp_s6[var_s4 >> 0xC].entry[var_s4 & 0xFFF] = sp18[var_s2];
                    var_s4 = sp18[var_s2];

                    var_s2 += 1;
                }

                temp_s6[var_s4 >> 0xC].entry[var_s4 & 0xFFF] = -1;
                if (release != 0) {
                    u16 var_a1_4;

                    var_a1_4 = temp_v0_2->block;
                    for (var_s2 = 0; var_s2 < var_s5; var_s2++) {
                        temp_a0 = temp_s6[var_a1_4 >> 0xC].entry[var_a1_4 & 0xFFF];
                        temp_s6[var_a1_4 >> 0xC].entry[var_a1_4 & 0xFFF] = 0;
                        var_a1_4 = temp_a0;
                    }
                    temp_v0_2->block = var_a1_4;
                    temp_v0_2->size -= var_s5 * 0x4000;
                }

                buf += var_s5 * 0x4000;
                temp_fp->size += var_s5 * 0x4000;
            }

            var_s1 = __osBbFsSync(0);
        }
    }

end:
    __osBbFsRelAccess();
    return var_s1;
}
