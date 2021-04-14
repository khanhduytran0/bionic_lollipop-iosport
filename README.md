# bionic_lollipop-iosport
Bionic Lollipop release, but linker was ported to jailbroken iOS.

## Features
- can load ELF .so file (even file with page size 4kb on 16kb devices for unknown reason).
- lib depends are being warned instead of fatal error if not found (workaround ignoring libc.so, libm.so and other libraries).
- symbols lookup will find to Darwin symbols if none found in loaded ELF libraries.
- tweaked to print all of undefined symbols.

## Known issues
- buggy math (maybe because of page size is different?)
- too big shared object can lead to loading segment fails.
- some methods may have constant different compared to Darwin, so I had to wrap them: `mmap()`.

## What’s this for?
- Run Linux/Android binaries on iOS (won’t fully works without emulating /proc. Also, need get a newer bionic clone to support GNU hash style).
— ~~Maybe port PojavLauncher Android to iOS by this way so I don’t have to rewrite almost everything to objc (need port the entire Android graphics lib), also OpenJDK 8 for iOS~~

## To do
- Create a CMakeLists.txt for building linker64.
