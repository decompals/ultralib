NON_MATCHING ?= 0

# One of:
# libgultra_rom, libgultra_d, libgultra
# libultra_rom, libultra_d, libultra
# libultra_rom_iQue
TARGET ?= libultra_rom_iQue
CROSS ?= mips-linux-gnu-

BASE_DIR := base_$(TARGET)
BASE_AR := $(TARGET).a
BUILD_ROOT := build
BUILD_DIR := $(BUILD_ROOT)/$(TARGET)
BUILD_AR := $(BUILD_DIR)/$(TARGET).a

WORKING_DIR := $(shell pwd)

CPP := cpp -P
AR := ar

ifeq ($(findstring _iQue,$(TARGET)),_iQue)
-include Makefile.egcs
else ifeq ($(findstring libgultra,$(TARGET)),libgultra)
-include Makefile.gcc
else ifeq ($(findstring libultra,$(TARGET)),libultra)
-include Makefile.ido
else
$(error Invalid Target)
endif

ifeq ($(findstring _d,$(TARGET)),_d)
CPPFLAGS += -D_DEBUG
else
CPPFLAGS += -DNDEBUG
endif

ifeq ($(findstring _rom,$(TARGET)),_rom)
CPPFLAGS += -D_FINALROM
endif

ifeq ($(findstring _iQue,$(TARGET)),_iQue)
CPPFLAGS += -DBBPLAYER
SRC_DIRS := $(shell find src -type d)
else
SRC_DIRS := $(shell find src -type d -not -path "src/bb/*")
endif

ASM_DIRS := $(shell find asm -type d -not -path "asm/non_matchings*")
C_FILES  := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))
S_FILES  := $(foreach dir,$(SRC_DIRS) $(ASM_DIRS),$(wildcard $(dir)/*.s))
O_FILES  := $(foreach f,$(S_FILES:.s=.o),$(BUILD_DIR)/$f) \
            $(foreach f,$(C_FILES:.c=.o),$(BUILD_DIR)/$f) \
            $(foreach f,$(wildcard $(BASE_DIR)/*),$(BUILD_DIR)/$f)
# Because we patch the object file timestamps, we can't use them as the targets since they'll always be older than the C file
# Therefore instead we use marker files that have actual timestamps as the dependencies for the archive
MARKER_FILES := $(O_FILES:.o=.marker)

ifneq ($(NON_MATCHING),1)

ifneq ($(COMPILER),gcc)
COMPARE_OBJ = $(OBJCOPY) -p --strip-debug $(WORKING_DIR)/$(@:.marker=.o) $(WORKING_DIR)/$(@:.marker=.cmp.o) && \
              cmp $(BASE_DIR)/.cmp/$(@F:.marker=.cmp.o) $(WORKING_DIR)/$(@:.marker=.cmp.o) && echo "$(@:.marker=.o): OK"
COMPARE_AR = echo "$@: Cannot compare archive currently"
else
COMPARE_OBJ = cmp $(BASE_DIR)/$(@F:.marker=.o) $(WORKING_DIR)/$(@:.marker=.o) && echo "$(@:.marker=.o): OK"
COMPARE_AR = cmp $(BASE_AR) $@ && echo "$@: OK"
endif

else

COMPARE_OBJ :=
COMPARE_AR :=
AR_OLD := $(AR)
endif

BASE_OBJS := $(shell find $(BASE_DIR) -type f -not -name "*.cmp.o")

# Try to find a file corresponding to an archive file in any of src/ asm/ or the base directory, prioritizing src then asm then the original file
AR_ORDER = $(foreach f,$(shell $(AR) t $(BASE_AR)),$(shell find $(BUILD_DIR)/src $(BUILD_DIR)/asm $(BUILD_DIR)/$(BASE_DIR) -iname $f -type f -print -quit))
MATCHED_OBJS = $(filter-out $(BUILD_DIR)/$(BASE_DIR)/%,$(AR_ORDER))
UNMATCHED_OBJS = $(filter-out $(MATCHED_OBJS),$(AR_ORDER))
NUM_OBJS = $(words $(AR_ORDER))
NUM_OBJS_MATCHED = $(words $(MATCHED_OBJS))
NUM_OBJS_UNMATCHED = $(words $(UNMATCHED_OBJS))

$(shell mkdir -p asm $(BASE_DIR) src $(BUILD_DIR)/$(BASE_DIR) $(foreach dir,$(ASM_DIRS) $(SRC_DIRS),$(BUILD_DIR)/$(dir)))

.PHONY: all clean distclean setup
all: $(BUILD_AR)

$(BUILD_AR): $(MARKER_FILES)
	$(AR_OLD) rcs $@ $(AR_ORDER)
ifneq ($(NON_MATCHING),1)
# patch archive creation time and individual files' ownership & permissions
	dd bs=1 skip=24 seek=24 count=12 conv=notrunc if=$(BASE_AR) of=$@ status=none
	python3 tools/patch_ar_meta.py $@ $(BASE_AR) $(PATCH_AR_FLAGS)
	@$(COMPARE_AR)
	@echo "Matched: $(NUM_OBJS_MATCHED)/$(NUM_OBJS)"
endif

clean:
	$(RM) -rf $(BUILD_ROOT)

distclean: clean
	$(MAKE) -C tools distclean
	$(RM) -rf $(BASE_DIR)

setup:
	$(MAKE) -C tools
	cd $(BASE_DIR) && $(AR) xo ../$(BASE_AR)
	chmod -R +rw $(BASE_DIR)
ifneq ($(COMPILER),gcc)
	export CROSS=$(CROSS) && ./tools/strip_debug.sh $(BASE_DIR)
endif

$(BUILD_DIR)/$(BASE_DIR)/%.marker: $(BASE_DIR)/%.o
	cp $< $(@:.marker=.o)
ifneq ($(NON_MATCHING),1)
# change file timestamps to match original
	@touch -r $(BASE_DIR)/$(@F:.marker=.o) $(@:.marker=.o)
	@touch $@
endif

GBIDEFINE := -DF3DEX_GBI_2

$(BUILD_DIR)/src/gu/parse_gbi.marker: GBIDEFINE := -DF3D_GBI
$(BUILD_DIR)/src/gu/us2dex_emu.marker: GBIDEFINE :=
$(BUILD_DIR)/src/gu/us2dex2_emu.marker: GBIDEFINE :=
$(BUILD_DIR)/src/sp/sprite.marker: GBIDEFINE := -DF3D_GBI
$(BUILD_DIR)/src/sp/spriteex.marker: GBIDEFINE :=
$(BUILD_DIR)/src/sp/spriteex2.marker: GBIDEFINE :=
$(BUILD_DIR)/src/voice/%.marker: OPTFLAGS += -DLANG_JAPANESE -I$(WORKING_DIR)/src -I$(WORKING_DIR)/src/voice
$(BUILD_DIR)/src/voice/%.marker: CC := tools/compile_sjis.py -D__CC=$(WORKING_DIR)/$(CC) -D__BUILD_DIR=$(BUILD_DIR)

$(BUILD_DIR)/%.marker: %.c
	cd $(<D) && $(WORKING_DIR)/$(CC) $(CFLAGS) $(MIPS_VERSION) $(REG_SIZES) $(CPPFLAGS) $(OPTFLAGS) $(<F) $(IINC) -o $(WORKING_DIR)/$(@:.marker=.o)
ifneq ($(NON_MATCHING),1)
# check if this file is in the archive; patch corrupted bytes and change file timestamps to match original if so
	@$(if $(findstring $(BASE_DIR)/$(@F:.marker=.o), $(BASE_OBJS)), \
	 python3 tools/fix_objfile.py $(@:.marker=.o) $(BASE_DIR)/$(@F:.marker=.o) $(STRIP) && \
	 $(COMPARE_OBJ) && \
	 touch -r $(BASE_DIR)/$(@F:.marker=.o) $(@:.marker=.o), \
	 echo "Object file $(@F:.marker=.o) is not in the current archive" \
	)
# create or update the marker file
	@touch $@
endif

$(BUILD_DIR)/%.marker: %.s
	cd $(<D) && $(WORKING_DIR)/$(CC) $(ASFLAGS) $(MIPS_VERSION) $(REG_SIZES) $(CPPFLAGS) $(ASOPTFLAGS) $(<F) $(IINC) -o $(WORKING_DIR)/$(@:.marker=.o)
ifneq ($(NON_MATCHING),1)
	python3 tools/patch_64bit_compile.py $(WORKING_DIR)/$(@:.marker=.o)
# check if this file is in the archive; patch corrupted bytes and change file timestamps to match original if so
	@$(if $(findstring $(BASE_DIR)/$(@F:.marker=.o), $(BASE_OBJS)), \
	 python3 tools/fix_objfile.py $(@:.marker=.o) $(BASE_DIR)/$(@F:.marker=.o) $(STRIP) && \
	 $(COMPARE_OBJ) && \
	 touch -r $(BASE_DIR)/$(@F:.marker=.o) $(@:.marker=.o), \
	 echo "Object file $(@F:.marker=.o) is not in the current archive" \
	)
# create or update the marker file
	@touch $@
endif

# Rule for building files that require specific file paths in the mdebug section
$(MDEBUG_FILES): $(BUILD_DIR)/src/%.marker: src/%.s
	cp $(<:.marker=.s) $(dir $@)
	mkdir -p $(@:.marker=)
	export USR_INCLUDE=$(WORKING_DIR)/include && cd $(@:.marker=) && $(WORKING_DIR)/$(CC) $(ASFLAGS) $(MIPS_VERSION) $(REG_SIZES) $(CPPFLAGS) ../$(<F) -I/usr/include -o $(notdir $(<:.s=.o))
	mv $(@:.marker=)/$(<F:.s=.o) $(@:.marker=)/..
ifneq ($(NON_MATCHING),1)
	python3 tools/patch_64bit_compile.py $(WORKING_DIR)/$(@:.marker=.o)
# check if this file is in the archive; patch corrupted bytes and change file timestamps to match original if so
	$(if $(findstring $(BASE_DIR)/$(@F:.marker=.o), $(BASE_OBJS)), \
	 python3 tools/fix_objfile.py $(@:.marker=.o) $(BASE_DIR)/$(@F:.marker=.o) && \
	 $(COMPARE_OBJ) && \
	 touch -r $(BASE_DIR)/$(@F:.marker=.o) $(@:.marker=.o), \
	 echo "Object file $(@F:.marker=.o) is not in the current archive" \
	)
# create or update the marker file
	@touch $@
endif

# Disable built-in rules
.SUFFIXES:
print-% : ; $(info $* is a $(flavor $*) variable set to [$($*)]) @true
