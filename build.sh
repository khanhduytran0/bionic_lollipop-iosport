set -e

if [ -z "$SDKROOT" ]
then
  echo "Warning: SDKROOT environment variable was not set, defaulting to /usr/share/SDKs/iPhoneOS14.4.sdk"
  export SDKROOT="/usr/share/SDKs/iPhoneOS14.4.sdk"
fi

export CFLAGS="\
  -Ilibc \
  -Ilibc/arch-arm64/include \
  -Ilibc/include \
  -Ilibc/kernel/uapi \
  -Ilibc/kernel/uapi/asm-arm64 \
  -Ilinker \
"

clang++ -dynamiclib $CFLAGS -o linker64 \
  -undefined dynamic_lookup \
\
  -D__errno=errno \
\
  patches/patches.cpp \
  libc/bionic/libc_logging.cpp \
  libc/arch-arm64/bionic/crtbegin.c \
  linker/arch/arm64/begin.S \
  linker/*.cpp

clang -isysroot /var/mobile/theos/sdks/iPhoneOS13.4.sdk $CFLAGS -o linker_test \
  patches/linker_test.c

ldid -S linker64
ldid -S linker_test

echo BUILD SUCCESS