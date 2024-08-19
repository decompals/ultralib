
COMPILER_DIR := $(dir $(which $(CROSS)gcc))
AS := $(CROSS)gcc -x assembler-with-cpp
CC := $(CROSS)gcc
AR_OLD := $(CROSS)ar

WARNINGS := -Wall -Wextra -Wno-format-security -Wno-unknown-pragmas -Wno-unused-parameter -Wno-unused-variable -Wno-missing-braces -Wno-builtin-declaration-mismatch
CFLAGS := -G 0 -c -nostdinc -march=vr4300 -mfix4300 -mabi=32 -mno-abicalls -mdivide-breaks -fno-PIC -fno-common -ffreestanding -fbuiltin -fno-builtin-sinf -fno-builtin-cosf -funsigned-char $(WARNINGS)
ASFLAGS := -w -nostdinc -c -G 0 -march=vr4300 -mgp32 -mfp32 -DMIPSEB -D_LANGUAGE_ASSEMBLY -D_MIPS_SIM=1 -D_ULTRA64
CPPFLAGS = -DMODERN -D_MIPS_SZLONG=32 -D__USE_ISOC99 $(GBIDEFINE) $(VERSION_DEFINE) $(DEBUGFLAG)
IINC = -I . -I $(WORKING_DIR)/include -I $(WORKING_DIR)/include/gcc -I $(WORKING_DIR)/include/PR
MIPS_VERSION := -mips3
ASOPTFLAGS :=
OPTFLAGS := -Os -ffast-math -fno-unsafe-math-optimizations
