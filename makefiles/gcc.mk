COMPILER_DIR := $(WORKING_DIR)/tools/gcc
AS := $(COMPILER_DIR)/gcc -x assembler-with-cpp
CC := $(COMPILER_DIR)/gcc
AR_OLD := $(COMPILER_DIR)/ar
PATCH_AR_FLAGS := 0 0 37777700
STRIP =

CFLAGS := -w -nostdinc -c -G 0 -mgp32 -mfp32 -D_LANGUAGE_C
ASFLAGS := -w -nostdinc -c -G 0 -mgp32 -mfp32 -DMIPSEB -D_LANGUAGE_ASSEMBLY -D_MIPS_SIM=1 -D_ULTRA64
CPPFLAGS = -D_MIPS_SZLONG=32 -D__USE_ISOC99 $(GBIDEFINE) $(VERSION_DEFINE) $(DEBUGFLAG)
IINC = -I . -I $(WORKING_DIR)/include -I $(WORKING_DIR)/include/compiler/gcc -I $(WORKING_DIR)/include/PR
MIPS_VERSION := -mips3
ASOPTFLAGS :=

ifneq ($(filter $(VERSION),D E F G H I J),)
CFLAGS += -funsigned-char
endif

# 2.0I libgultra_rom was not compiled with -DNDEBUG and instead libgultra had -DNDEBUG
ifneq ($(filter $(VERSION),I),)
ifeq ($(findstring _rom,$(TARGET)),_rom)
DEBUGFLAG :=
else ifeq ($(findstring _d,$(TARGET)),_d)
DEBUGFLAG := -D_DEBUG
else
DEBUGFLAG := -DNDEBUG
endif
endif

ifeq ($(findstring _d,$(TARGET)),_d)
OPTFLAGS := -O0
else
OPTFLAGS := -O3
endif

ifeq ($(findstring _d,$(TARGET)),_d)
$(BUILD_DIR)/src/rmon/%.marker: OPTFLAGS := -O0
endif

# KMC gcc has a custom flag, N64ALIGN, which forces 8 byte alignment on arrays. This can be used to match, but
# an explicit aligned(8) attribute can be used instead. We opted for the latter for better compatibilty with
# other versions of GCC that do not have this flag.
# export N64ALIGN := ON
export VR4300MUL := ON

$(BUILD_DIR)/src/os/initialize_isv.marker: OPTFLAGS := -O2
$(BUILD_DIR)/src/os/initialize_isv.marker: STRIP = && $(COMPILER_DIR)/strip-2.7 -N initialize_isv.c $(WORKING_DIR)/$(@:.marker=.o)
$(BUILD_DIR)/src/os/assert.marker: OPTFLAGS := -O0
ifeq ($(filter $(VERSION),D E F G H I),)
$(BUILD_DIR)/src/os/seterrorhandler.marker: OPTFLAGS := -O0
endif
$(BUILD_DIR)/src/mgu/%.marker: export VR4300MUL := OFF
$(BUILD_DIR)/src/mgu/rotate.marker: export VR4300MUL := ON
$(BUILD_DIR)/src/debug/%.marker: ASFLAGS += -P
$(BUILD_DIR)/src/error/%.marker: ASFLAGS += -P
$(BUILD_DIR)/src/log/%.marker: ASFLAGS += -P
$(BUILD_DIR)/src/os/%.marker: ASFLAGS += -P
$(BUILD_DIR)/src/gu/%.marker: ASFLAGS += -P
$(BUILD_DIR)/src/libc/%.marker: ASFLAGS += -P
$(BUILD_DIR)/src/rmon/%.marker: ASFLAGS += -P
$(BUILD_DIR)/src/host/host_ptn64.marker: CFLAGS += -fno-builtin # Probably a better way to solve this

MDEBUG_FILES := $(BUILD_DIR)/src/monutil.marker
MDEBUG_COMPILER_DIR := $(WORKING_DIR)/tools/ido
$(MDEBUG_FILES): AS := $(MDEBUG_COMPILER_DIR)/cc
$(MDEBUG_FILES): ASFLAGS := -non_shared -mips2 -fullwarn -verbose -Xcpluscomm -G 0 -woff 516,649,838,712 -Wab,-r4300_mul -nostdinc -o32 -c
