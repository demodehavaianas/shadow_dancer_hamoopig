del /Q C:\GENDK\SHADOW_DANCER\out\res\*.*
del /Q C:\GENDK\SHADOW_DANCER\out\src\*.*
del /Q C:\GENDK\SHADOW_DANCER\out\watchers\*.*
del /Q C:\GENDK\SHADOW_DANCER\out\rom.bin
del /Q C:\GENDK\SHADOW_DANCER\out\rom.out
del /Q C:\GENDK\SHADOW_DANCER\out\rom_head.bin
del /Q C:\GENDK\SHADOW_DANCER\out\rom_head.o
del /Q C:\GENDK\SHADOW_DANCER\out\sega.o
del /Q C:\GENDK\SHADOW_DANCER\out\sysbol.txt
C:\sgdk\bin\make -f C:\sgdk\makefile.gen
C:\GAMES\Bizhawk\EmuHawk C:\GENDK\SHADOW_DANCER\out\rom.bin