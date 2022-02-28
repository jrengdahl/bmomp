arm-cortexm7-eabi-gcc ^
-march=armv7-m ^
-mthumb ^
-ffixed-r9 ^
-msoft-float ^
-mgeneral-regs-only ^
-mabi=aapcs-linux ^
-Os ^
-g ^
-nostdinc ^
-isystem c:/cross/arm-cortexm7-eabi/lib/gcc/arm-cortexm7-eabi/11.2.0/include ^
-I ../u-boot/include ^
-I ../u-boot/arch/arm/thumb1/include ^
-I ../u-boot/arch/arm/include ^
-include ../u-boot/include/linux/kconfig.h ^
-I ../u-boot/arch/arm/mach-stm32/include ^
-D__KERNEL__ ^
-D__UBOOT__ ^
-D__ARM__ ^
-D__LINUX_ARM_ARCH__=7 ^
-DKBUILD_BASENAME='"hello"' ^
-DKBUILD_MODNAME='"hello"' ^
-Wall ^
-Wstrict-prototypes ^
-Wno-format-security ^
-fno-builtin ^
-ffreestanding ^
-std=gnu11 ^
-fshort-wchar ^
-fno-strict-aliasing ^
-fno-PIE ^
-fno-stack-protector ^
-fno-delete-null-pointer-checks ^
-Wno-pointer-sign ^
-Wno-stringop-truncation ^
-Wno-zero-length-bounds ^
-Wno-array-bounds ^
-Wno-stringop-overflow ^
-Wno-maybe-uninitialized ^
-fmacro-prefix-map=./= ^
-fstack-usage ^
-Wno-format-nonliteral ^
-Wno-address-of-packed-member ^
-Wno-unused-but-set-variable ^
-Werror=date-time ^
-Wno-packed-not-aligned ^
-fno-inline ^
-fno-toplevel-reorder ^
-mword-relocations ^
-fno-pic ^
-mno-unaligned-access ^
-ffunction-sections ^
-fdata-sections ^
-fno-common ^
-c ^
hello.c


arm-cortexm7-eabi-gcc ^
-march=armv7-m ^
-mthumb ^
-ffixed-r9 ^
-msoft-float ^
-mgeneral-regs-only ^
-mabi=aapcs-linux ^
-Os ^
-g ^
-nostdinc ^
-isystem c:/cross/arm-cortexm7-eabi/lib/gcc/arm-cortexm7-eabi/11.2.0/include ^
-I ../u-boot/include ^
-I ../u-boot/arch/arm/thumb1/include ^
-I ../u-boot/arch/arm/include ^
-include ../u-boot/include/linux/kconfig.h ^
-I ../u-boot/arch/arm/mach-stm32/include ^
-D__KERNEL__ ^
-D__UBOOT__ ^
-D__ARM__ ^
-D__LINUX_ARM_ARCH__=7 ^
-DKBUILD_BASENAME='"hello"' ^
-DKBUILD_MODNAME='"hello"' ^
-Wall ^
-Wstrict-prototypes ^
-Wno-format-security ^
-fno-builtin ^
-ffreestanding ^
-std=gnu11 ^
-fshort-wchar ^
-fno-strict-aliasing ^
-fno-PIE ^
-fno-stack-protector ^
-fno-delete-null-pointer-checks ^
-Wno-pointer-sign ^
-Wno-stringop-truncation ^
-Wno-zero-length-bounds ^
-Wno-array-bounds ^
-Wno-stringop-overflow ^
-Wno-maybe-uninitialized ^
-fmacro-prefix-map=./= ^
-fstack-usage ^
-Wno-format-nonliteral ^
-Wno-address-of-packed-member ^
-Wno-unused-but-set-variable ^
-Werror=date-time ^
-Wno-packed-not-aligned ^
-fno-inline ^
-fno-toplevel-reorder ^
-mword-relocations ^
-fno-pic ^
-mno-unaligned-access ^
-ffunction-sections ^
-fdata-sections ^
-fno-common ^
-c ^
../u-boot/examples/standalone/stubs.c



arm-cortexm7-eabi-ld.bfd ^
-Ttext=0x24000000 ^
-g ^
-o hello.axf ^
-e hello ^
hello.o ^
stubs.o ^
../u-boot/arch/arm/lib/lib.a

arm-cortexm7-eabi-objcopy ^
-j .text ^
-j .secure_text ^
-j .secure_data ^
-j .rodata ^
-j .hash ^
-j .data ^
-j .got ^
-j .got.plt ^
-j .u_boot_list ^
-j .rel.dyn ^
-j .binman_sym_table ^
-j .text_rest ^
-j .dtb.init.rodata ^
-O binary ^
hello.axf ^
hello.bin


