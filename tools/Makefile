
.PHONY: all clean distclean

GCC_DIR   := gcc
AR        := $(GCC_DIR)/ar
GCC-2.7.2 := $(GCC_DIR)/gcc

EGCS_DIR := egcs
EGCS-AS := $(EGCS_DIR)/as
EGCS-1.1.2 := $(EGCS_DIR)/gcc

all: $(AR) $(GCC-2.7.2) $(EGCS-AS) $(EGCS-1.1.2)

clean:
distclean: clean
	$(RM) -rf $(GCC_DIR) $(EGCS_DIR)

# GCC

$(AR): | $(GCC_DIR)
	wget https://github.com/decompals/mips-binutils-2.6/releases/download/v0.2/binutils-2.6-linux.tar.gz
	tar xf binutils-2.6-linux.tar.gz -C $(GCC_DIR)
	$(RM) binutils-2.6-linux.tar.gz

$(GCC-2.7.2): | $(GCC_DIR)
	wget https://github.com/decompals/mips-gcc-2.7.2/releases/download/main/gcc-2.7.2-linux.tar.gz
	tar xf gcc-2.7.2-linux.tar.gz -C $(GCC_DIR)
	$(RM) gcc-2.7.2-linux.tar.gz

$(GCC_DIR):
	mkdir -p $@

# EGCS

$(EGCS-AS): | $(EGCS_DIR)
	wget https://github.com/decompals/mips-binutils-egcs-2.9.5/releases/download/0.4/mips-binutils-egcs-2.9.5-linux.tar.gz
	tar xf mips-binutils-egcs-2.9.5-linux.tar.gz -C $(EGCS_DIR)
	$(RM) mips-binutils-egcs-2.9.5-linux.tar.gz

$(EGCS-1.1.2): | $(EGCS_DIR)
	wget https://github.com/decompals/mips-gcc-egcs-2.91.66/releases/download/0.2/mips-gcc-egcs-2.91.66-linux.tar.gz
	tar xf mips-gcc-egcs-2.91.66-linux.tar.gz -C $(EGCS_DIR)
	$(RM) mips-gcc-egcs-2.91.66-linux.tar.gz

$(EGCS_DIR):
	mkdir -p $@
