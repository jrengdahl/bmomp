CROSS=arm-cortexm7-eabi-

OPT += -march=armv7-m
OPT += -ffixed-r9
OPT += -msoft-float
OPT += -mgeneral-regs-only
OPT += -mabi=aapcs-linux
OPT += -Os
OPT += -g

INC += -nostdinc
INC += -isystem c:/cross/arm-cortexm7-eabi/lib/gcc/arm-cortexm7-eabi/11.2.0/include
INC += -I ../u-boot/include
INC += -I ../u-boot/arch/arm/thumb1/include
INC += -I ../u-boot/arch/arm/include
INC += -include ../u-boot/include/linux/kconfig.h
INC += -I ../u-boot/arch/arm/mach-stm32/include

DEFINE += -D__KERNEL__
DEFINE += -D__UBOOT__
DEFINE += -D__ARM__
DEFINE += -D__LINUX_ARM_ARCH__=7
DEFINE += -DKBUILD_BASENAME='"hello"'
DEFINE += -DKBUILD_MODNAME='"hello"'
 
WARN += -Wall
WARN += -Wstrict-prototypes
WARN += -Wno-format-security
WARN += -Wno-pointer-sign
WARN += -Wno-stringop-truncation
WARN += -Wno-zero-length-bounds
WARN += -Wno-array-bounds
WARN += -Wno-stringop-overflow
WARN += -Wno-maybe-uninitialized
WARN += -Wno-format-nonliteral
WARN += -Wno-address-of-packed-member
WARN += -Wno-unused-but-set-variable
WARN += -Werror=date-time
WARN += -Wno-packed-not-aligned

OPT += -fno-builtin
OPT += -ffreestanding
OPT += -std=gnu11
OPT += -fshort-wchar
OPT += -fno-strict-aliasing
OPT += -fno-PIE
OPT += -fno-stack-protector
OPT += -fno-delete-null-pointer-checks
OPT += -fmacro-prefix-map=./=
OPT += -fstack-usage
OPT += -fno-inline
OPT += -fno-toplevel-reorder
OPT += -mword-relocations
OPT += -fno-pic
OPT += -mno-unaligned-access
OPT += -ffunction-sections
OPT += -fdata-sections
OPT += -fno-common

SECT += -j .text
SECT += -j .secure_text
SECT += -j .secure_data
SECT += -j .rodata
SECT += -j .hash
SECT += -j .data
SECT += -j .got
SECT += -j .got.plt
SECT += -j .u_boot_list
SECT += -j .rel.dyn
SECT += -j .binman_sym_table
SECT += -j .text_rest
SECT += -j .dtb.init.rodata


all: hello.bin

%.o: %.c
	@echo [CC]  $<
	@$(CROSS)gcc $(OPT) $(INC) $(DEFINE) $(WARN) -c $<

stubs.o: ../u-boot/examples/standalone/stubs.c
	@echo [CC]  $<
	@$(CROSS)gcc $(OPT) $(INC) $(DEFINE) $(WARN) -c $<


hello.axf : hello.o stubs.o
	@echo [LD] hello.axf
	@$(CROSS)ld.bfd -Ttext=0x24000000 -g -o hello.axf -e hello $^ ../u-boot/arch/arm/lib/lib.a

%.bin : %.axf
	@echo [OBJCOPY] $<
	@$(CROSS)objcopy $(SECT) -O binary $< $@


clean:
	@echo [CLEAN}
	@rm -f *.o *.su *.axf *.bin
