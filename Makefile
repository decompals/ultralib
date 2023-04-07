# Makefile to build libmus

include util.mk

# Preprocessor definitions

DEFINES :=

SRC_DIRS :=

# Whether to hide commands or not
VERBOSE ?= 0
ifeq ($(VERBOSE),0)
  V := @
endif

# Whether to colorize build messages
COLOR ?= 1

# VERSION 	   - selects the version of the library to build
#   libultra	 - standard library
#   libultra_d   - debug library
#   libultra_rom - debug n_audio library
VERSION ?= libultra_rom
$(eval $(call validate-option,VERSION,libultra libultra_d libultra_rom))

ifeq      ($(VERSION),libultra)
	OPT_FLAGS := -Os
	DEFINES += NDEBUG=1
else ifeq ($(VERSION),libultra_d)
	OPT_FLAGS := -O0 -g -ggdb
	DEFINES += _DEBUG=1
else ifeq ($(VERSION),libultra_rom)
	OPT_FLAGS := -Os
	DEFINES += NDEBUG=1
	DEFINES += _FINALROM=1
endif

TARGET := $(VERSION)

ifeq ($(filter clean,$(MAKECMDGOALS)),)
  $(info ==== Build Options ====)
  $(info Version:        $(VERSION))
  $(info =======================)
endif

#==============================================================================#
# Target Executable and Sources                                                #
#==============================================================================#
BUILD_DIR_BASE := build
# BUILD_DIR is the location where all build artifacts are placed
BUILD_DIR      := $(BUILD_DIR_BASE)/$(VERSION)
LIB            := $(BUILD_DIR)/$(TARGET).a

# Directories containing source files
SRC_DIRS += $(shell find src -type d)

C_FILES           := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))
S_FILES           := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.s))

# Object files
O_FILES := $(foreach file,$(C_FILES),$(BUILD_DIR)/$(file:.c=.o)) \
           $(foreach file,$(S_FILES),$(BUILD_DIR)/$(file:.s=.o))

# Automatic dependency files
DEP_FILES := $(O_FILES:.o=.d) $(ASM_O_FILES:.o=.d)

#==============================================================================#
# Compiler Options                                                             #
#==============================================================================#

AS        := mips-n64-as
CC        := mips-n64-gcc
CPP       := cpp
LD        := mips-n64-ld
AR        := mips-n64-ar

# Do NOT depend on system-installed headers! If you need to make a header change,
# test it in your source first!
INCLUDE_DIRS += include include/PR include/internal $(BUILD_DIR) $(BUILD_DIR)/include src .

GBIDEFINE := -DF3DEX_GBI_2

C_DEFINES = $(foreach d,$(DEFINES),-D$(d)) $(GBIDEFINE)
DEF_INC_CFLAGS = $(foreach i,$(INCLUDE_DIRS),-I$(i)) $(C_DEFINES)

CFLAGS = -G 0 $(OPT_FLAGS) -mabi=32 -ffreestanding -mfix4300 -fno-stack-protector -mno-check-zero-division $(DEF_INC_CFLAGS) -Wall -fwrapv
ASFLAGS     := -march=vr4300 -mabi=32 $(foreach i,$(INCLUDE_DIRS),-I$(i)) $(foreach d,$(DEFINES),--defsym $(d))

# C preprocessor flags
CPPFLAGS := -P -Wno-trigraphs $(DEF_INC_CFLAGS)

# tools
PRINT = printf

ifeq ($(COLOR),1)
NO_COL  := \033[0m
RED     := \033[0;31m
GREEN   := \033[0;32m
BLUE    := \033[0;34m
YELLOW  := \033[0;33m
BLINK   := \033[33;5m
endif

# Common build print status function
define print
  @$(PRINT) "$(GREEN)$(1) $(YELLOW)$(2)$(GREEN) -> $(BLUE)$(3)$(NO_COL)\n"
endef

#==============================================================================#
# Main Targets                                                                 #
#==============================================================================#

# Default target
default: $(LIB)

clean:
	$(RM) -r $(BUILD_DIR_BASE)

ALL_DIRS := $(BUILD_DIR) $(addprefix $(BUILD_DIR)/,$(SRC_DIRS))

# Make sure build directory exists before compiling anything
DUMMY != mkdir -p $(ALL_DIRS)

$(BUILD_DIR)/src/nintendo/voice/%.o: DEFINES += LANG_JAPANESE=1
$(BUILD_DIR)/src/monegi/gu/parse_gbi.o: GBIDEFINE = 
$(BUILD_DIR)/src/monegi/gu/us2dex_emu.o: GBIDEFINE = -DF3DEX_GBI
$(BUILD_DIR)/src/monegi/sprite/sprite.o: GBIDEFINE = 
$(BUILD_DIR)/src/monegi/sprite/spriteex.o: GBIDEFINE = 
$(BUILD_DIR)/src/monegi/sprite/spriteex2.o: GBIDEFINE = 

#==============================================================================#
# Compilation Recipes                                                          #
#==============================================================================#

# Compile C code
$(BUILD_DIR)/src/nintendo/voice/%.o: src/nintendo/voice/%.c
	$(call print,Compiling:,$<,$@)
	$(V)tools/compile_sjis.py -D__CC=$(CC) -D__BUILD_DIR=$(BUILD_DIR) -c $(CFLAGS)  -D_LANGUAGE_C -Isrc -Isrc/nintendo/voice -MMD -MF $(BUILD_DIR)/src/nintendo/voice/$*.d  -o $@ $<
#	$(V)$(CC) -c $(CFLAGS) -MMD -MF $(BUILD_DIR)/$*.d  -o $@ $<

$(BUILD_DIR)/%.o: %.c
	$(call print,Compiling:,$<,$@)
	$(V)$(CC) -c $(CFLAGS) -D_LANGUAGE_C -MMD -MF $(BUILD_DIR)/$*.d  -o $@ $<
$(BUILD_DIR)/%.o: $(BUILD_DIR)/%.c
	$(call print,Compiling:,$<,$@)
	$(V)$(CC) -c $(CFLAGS) -D_LANGUAGE_C -MMD -MF $(BUILD_DIR)/$*.d  -o $@ $<

# Assemble assembly code
$(BUILD_DIR)/%.o: %.s
	$(call print,Assembling:,$<,$@)
	$(V)$(CC) -c $(CFLAGS) $(foreach i,$(INCLUDE_DIRS),-Wa,-I$(i)) -x assembler-with-cpp -MMD -MF $(BUILD_DIR)/$*.d  -o $@ $<

# Link final ELF file
$(LIB): $(O_FILES)
	@$(PRINT) "$(GREEN)Linking $(VERSION):  $(BLUE)$@ $(NO_COL)\n"
	$(V)$(AR) rcs -o $@ $(O_FILES)

all:
	$(MAKE) VERSION=libultra
	cp $(BUILD_DIR_BASE)/libultra/libultra.a $(BUILD_DIR_BASE)
	$(MAKE) VERSION=libultra_d
	cp $(BUILD_DIR_BASE)/libultra_d/libultra_d.a $(BUILD_DIR_BASE)
	$(MAKE) VERSION=libultra_rom
	cp $(BUILD_DIR_BASE)/libultra_rom/libultra_rom.a $(BUILD_DIR_BASE)

.PHONY: clean default all
# with no prerequisites, .SECONDARY causes no intermediate target to be removed
.SECONDARY:

# Remove built-in rules, to improve performance
MAKEFLAGS += --no-builtin-rules

-include $(DEP_FILES)

print-% : ; $(info $* is a $(flavor $*) variable set to [$($*)]) @true
