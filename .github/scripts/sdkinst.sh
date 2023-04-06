# Variables and PATH
echo "N64_LIBGCCDIR=/opt/crashsdk/lib/gcc/mips64-elf/11.2.0" >> $GITHUB_ENV
echo "ROOT=/etc/n64" >> $GITHUB_ENV
echo "/opt/crashsdk/bin" >> $GITHUB_PATH

# Add apt repo
echo "deb [trusted=yes] https://crashoveride95.github.io/apt/ ./" | sudo tee /etc/apt/sources.list.d/n64sdk.list

sudo dpkg --add-architecture i386
