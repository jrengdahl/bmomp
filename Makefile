CROSS=arm-cortexm7-eabi-

BINARIES += hello_c.bin
BINARIES += hello_cpp.bin
BINARIES += threadtest.bin
BINARIES += omp1.bin
BINARIES += omp2.bin
BINARIES += omp3.bin
BINARIES += omp4.bin
BINARIES += omp5.bin
BINARIES += omp6.bin
BINARIES += omp7.bin
BINARIES += omp8.bin
BINARIES += omp9.bin
BINARIES += omp10.bin
BINARIES += hello.bin
BINARIES += permute.bin
BINARIES += linpack.bin
BINARIES += calibrate.bin


OPT += -march=armv7-m
OPT += -ffixed-r9
OPT += -ffixed-r11
OPT += -mabi=aapcs-linux
OPT += -fopenmp
OPT += -Os
OPT += -g

INC += -I .

UINC += -nostdinc
UINC += -isystem c:/cross/arm-cortexm7-eabi/lib/gcc/arm-cortexm7-eabi/11.2.0/include
UINC += -I ../u-boot/include
UINC += -I ../u-boot/arch/arm/thumb1/include
UINC += -I ../u-boot/arch/arm/include
UINC += -include ../u-boot/include/linux/kconfig.h
UINC += -I ../u-boot/arch/arm/mach-stm32/include

DEFINES += -D__KERNEL__
DEFINES += -D__UBOOT__
DEFINES += -D__ARM__
DEFINES += -D__LINUX_ARM_ARCH__=7

DEFINES.linpack.cpp += -D__unix__


OPT     += -ffreestanding
COPT    += -std=gnu11
CPPOPT  += -std=gnu++17
OPT     += -fshort-wchar
CPPOPT  += -fno-strict-aliasing
OPT     += -fno-stack-protector
OPT     += -fno-delete-null-pointer-checks
OPT     += -fmacro-prefix-map=./=
OPT     += -fstack-usage
COPT    += -fno-inline
OPT     += -fno-toplevel-reorder
COPT    += -mno-unaligned-access
OPT     += -ffunction-sections
OPT     += -fdata-sections
OPT     += -fno-common
CPPOPT  += -fno-exceptions
OPT     += -fno-optimize-sibling-calls

OPT += -Wall
COPT += -Wstrict-prototypes

OPT.linpack.cpp += -O3

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


LIB += -L c:/cross/arm-cortexm7-eabi/usr/arm-cortexm7-eabi/lib

all: $(BINARIES)

%.o: %.cpp
	@echo [C+]  $<
	@$(CROSS)g++ $(OPT) $(CPPOPT) $(OPT.$<) $(INC) $(DEFINES) $(DEFINES.$<) $(WARN) -c $<

%.o: %.c
	@echo [CC]  $<
	@$(CROSS)gcc $(OPT) $(COPT) $(OPT.$<) $(INC) $(DEFINES) $(DEFINES.$<) $(WARN) -c $<

stubs.o: ../u-boot/examples/standalone/stubs.c
	@echo [CC]  $<
	@$(CROSS)gcc $(OPT) $(INC) $(UINC) $(DEFINES) $(WARN) -c $<

hello_c.axf : hello_c.o stubs.o
	@echo [LD] $@
	@$(CROSS)ld.bfd -Ttext=0x24000000 -g -o $@ -e hello $^ ../u-boot/arch/arm/lib/lib.a -Map=$(basename $@).map 

hello_cpp.axf : hello_cpp.o stubs.o
	@echo [LD] $@
	@$(CROSS)ld.bfd -Ttext=0x24000000 -g -o $@ -e hello $^ ../u-boot/arch/arm/lib/lib.a -Map=$(basename $@).map 

threadtest.axf : threadtest.o thread.o stubs.o
	@echo [LD] $@
	@$(CROSS)ld.bfd -T lscript0.ld -g -o $@ -e start $^ ../u-boot/arch/arm/lib/lib.a -Map=$(basename $@).map 

omp%.axf : omp%.o thread.o libgomp.o atoi.o stubs.o
	@echo [LD] $@
	@$(CROSS)ld.bfd -T lscript0.ld -g -o $@ -e start $^ ../u-boot/arch/arm/lib/lib.a $(LIB) -lm -Map=$(basename $@).map --no-warn-mismatch

%.axf : startup.o %.o thread.o libgomp.o atoi.o clock.o stubs.o
	@echo [LD] $@
	@$(CROSS)ld.bfd -T lscript.ld -g -o $@ -e start $^ ../u-boot/arch/arm/lib/lib.a $(LIB) -lm -Map=$(basename $@).map --no-warn-mismatch

%.bin : %.axf
	@echo [OBJCOPY] $<
	@$(CROSS)objcopy $(SECT) -O binary $< $@


.PRECIOUS : %.axf omp%.axf %.o


clean:
	@echo [CLEAN}
	@rm -f *.o *.su *.axf *.bin *.map
