NON_MATCHING ?= 0

TARGET := libgultra_rom
BASE_DIR := base_$(TARGET)
BASE_AR := $(TARGET).a
BUILD_DIR := build
BUILD_AR := $(BUILD_DIR)/$(TARGET).a

CPP := cpp -P
AR := ar
AS := tools/kmc-gcc-wrapper/as
CC := tools/kmc-gcc-wrapper/gcc
AR_OLD := tools/ar

SRC_DIRS := $(shell find src -type d)
ASM_DIRS := $(shell find asm -type d -not -path "asm/non_matchings*")
C_FILES  := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))
S_FILES  := $(foreach dir,$(ASM_DIRS),$(wildcard $(dir)/*.s))
O_FILES  := $(foreach f,$(S_FILES:.s=.o),$(BUILD_DIR)/$f) \
            $(foreach f,$(C_FILES:.c=.o),$(BUILD_DIR)/$f) \
            $(foreach f,$(wildcard $(BASE_DIR)/*),$(BUILD_DIR)/$f)

ifneq ($(NON_MATCHING),1)
COMPARE_OBJ = cmp $(BASE_DIR)/$(@F) $@ && echo "$@: OK"
COMPARE_AR = cmp $(BASE_AR) $@ && echo "$@: OK"
else
COMPARE_OBJ :=
COMPARE_AR :=
AR_OLD := $(AR)
endif

# Try to find a file corresponding to an archive file in any of src/ asm/ or the base directory, prioritizing src then asm then the original file
AR_ORDER = $(foreach f,$(shell $(AR) t $(BASE_AR)),$(shell find $(BUILD_DIR)/src $(BUILD_DIR)/asm $(BUILD_DIR)/$(BASE_DIR) -name $f -type f -print -quit))

$(shell mkdir -p $(BASE_DIR) $(BUILD_DIR)/$(BASE_DIR) $(foreach dir,$(ASM_DIRS) $(SRC_DIRS),$(BUILD_DIR)/$(dir)))

.PHONY=all clean distclean setup
all: $(BUILD_AR)

$(BUILD_AR): $(O_FILES)
	$(AR_OLD) rcs $@ $(AR_ORDER)
ifneq ($(NON_MATCHING),1)
# patch archive creation time and individual files' ownership & permissions
	dd bs=1 skip=24 seek=24 count=12 conv=notrunc if=$(BASE_AR) of=$@ status=none
	python3 tools/patch_ar_meta.py $@
	@$(COMPARE_AR)
endif

clean:
	$(RM) -rf $(BUILD_DIR)

distclean: clean
	$(MAKE) -C tools distclean
	$(RM) -rf $(BASE_DIR)

setup:
	cd $(BASE_DIR) && $(AR) xo ../$(BASE_AR)
	chmod -R +rw $(BASE_DIR)

$(BUILD_DIR)/$(BASE_DIR)/%.o: $(BASE_DIR)/%.o
	cp $< $@
ifneq ($(NON_MATCHING),1)
#	@$(COMPARE_OBJ)
# change file timestamps to match original
	touch -r $(BASE_DIR)/$(@F) $@
endif

$(BUILD_DIR)/%.o: %.c
	cd $(<D) && ../$(CC) -nostdinc -c -G 0 -mgp32 -mfp32 -mips3 -O3 -I ../include $(<F) -o ../$@
ifneq ($(NON_MATCHING),1)
# patch corrupted bytes
	python3 tools/fix_objfile.py $@ $(BASE_DIR)/$(@F)
#	@$(COMPARE_OBJ)
# change file timestamps to match original
	touch -r $(BASE_DIR)/$(@F) $@
endif
