
.PHONY: all clean distclean
all: ar
	$(MAKE) -C kmc-gcc-wrapper

ar:
	wget https://github.com/decompals/mips-binutils-2.7/releases/download/main/binutils-2.7.tar.gz
	tar xf binutils-2.7.tar.gz
	$(RM) binutils-2.7.tar.gz

clean:
	$(MAKE) -C kmc-gcc-wrapper clean

distclean: clean
	$(RM) ar
