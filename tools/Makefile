
.PHONY: all clean distclean

# GCC
GCC_DIR   := gcc
AR        := $(GCC_DIR)/ar
GCC-2.7.2 := $(GCC_DIR)/gcc
STRIP-2.7 := $(GCC_DIR)/strip-2.7

# EGCS
EGCS_DIR := egcs
EGCS-AS := $(EGCS_DIR)/as
EGCS-1.1.2 := $(EGCS_DIR)/gcc

# IDO
IDO_DIR   := ido
IDO-5.3   := $(IDO_DIR)/cc

all: $(AR) $(GCC-2.7.2) $(STRIP-2.7) $(EGCS-AS) $(EGCS-1.1.2) $(IDO-5.3)

clean:

distclean: clean
	$(RM) -rf $(GCC_DIR) $(EGCS_DIR) $(IDO_DIR)

# GCC

$(AR): | $(GCC_DIR)
	wget https://github.com/decompals/mips-binutils-2.6/releases/download/v0.2/binutils-2.6-linux.tar.gz
	tar xf binutils-2.6-linux.tar.gz -C $(GCC_DIR)
	$(RM) binutils-2.6-linux.tar.gz

$(GCC-2.7.2): | $(GCC_DIR)
	wget https://github.com/decompals/mips-gcc-2.7.2/releases/download/main/gcc-2.7.2-linux.tar.gz
	tar xf gcc-2.7.2-linux.tar.gz -C $(GCC_DIR)
	$(RM) gcc-2.7.2-linux.tar.gz

$(STRIP-2.7): | $(GCC_DIR)
	wget https://github.com/decompals/mips-binutils-2.7/releases/download/release/binutils-2.7.tar.gz
	tar xf binutils-2.7.tar.gz -C $(GCC_DIR)
	$(RM) binutils-2.7.tar.gz

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

# IDO

$(IDO-5.3): | $(IDO_DIR)
	wget https://github.com/decompals/ido-static-recomp/releases/latest/download/ido-5.3-recomp-ubuntu-20.04.tar.gz
	tar xf ido-5.3-recomp-ubuntu-20.04.tar.gz -C $(IDO_DIR)
	$(RM) ido-5.3-recomp-ubuntu-20.04.tar.gz

$(IDO_DIR):
	mkdir -p $@
