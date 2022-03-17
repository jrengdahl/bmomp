CROSS=arm-cortexm7-eabi-

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

DEFINE += -D__KERNEL__
DEFINE += -D__UBOOT__
DEFINE += -D__ARM__
DEFINE += -D__LINUX_ARM_ARCH__=7
 

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


all: hello.bin hello_cpp.bin test.bin omp1.bin omp2.bin omp3.bin omp4.bin omp5.bin

%.o: %.cpp
	@echo [C+]  $<
	@$(CROSS)g++ $(OPT) $(CPPOPT) $(INC) $(DEFINE) $(WARN) -c $<

%.o: %.c
	@echo [CC]  $<
	@$(CROSS)gcc $(OPT) $(COPT) $(INC) $(DEFINE) $(WARN) -c $<

stubs.o: ../u-boot/examples/standalone/stubs.c
	@echo [CC]  $<
	@$(CROSS)gcc $(OPT) $(INC) $(UINC) $(DEFINE) $(WARN) -c $<


hello.axf : hello.o stubs.o
	@echo [LD] hello.axf
	@$(CROSS)ld.bfd -Ttext=0x24000000 -g -o hello.axf -e hello $^ ../u-boot/arch/arm/lib/lib.a -Map=$(basename $@).map 

hello_cpp.axf : hello_cpp.o stubs.o
	@echo [LD] hello_cpp.axf
	@$(CROSS)ld.bfd -Ttext=0x24000000 -g -o hello_cpp.axf -e hello $^ ../u-boot/arch/arm/lib/lib.a -Map=$(basename $@).map 

test.axf : test.o thread.o stubs.o
	@echo [LD] test.axf
	@$(CROSS)ld.bfd -T lscript.ld -g -o test.axf -e start $^ ../u-boot/arch/arm/lib/lib.a -Map=$(basename $@).map 

omp1.axf : omp1.o thread.o libgomp.o stubs.o
	@echo [LD] omp1.axf
	@$(CROSS)ld.bfd -T lscript.ld -g -o omp1.axf -e start $^ ../u-boot/arch/arm/lib/lib.a -Map=$(basename $@).map 

omp2.axf : omp2.o thread.o libgomp.o stubs.o
	@echo [LD] $@
	@$(CROSS)ld.bfd -T lscript.ld -g -o $@ -e start $^ ../u-boot/arch/arm/lib/lib.a -Map=$(basename $@).map 

omp3.axf : omp3.o thread.o libgomp.o stubs.o
	@echo [LD] $@
	@$(CROSS)ld.bfd -T lscript.ld -g -o $@ -e start $^ ../u-boot/arch/arm/lib/lib.a -Map=$(basename $@).map 

omp4.axf : omp4.o thread.o libgomp.o stubs.o
	@echo [LD] $@
	@$(CROSS)ld.bfd -T lscript.ld -g -o $@ -e start $^ ../u-boot/arch/arm/lib/lib.a -Map=$(basename $@).map 

omp5.axf : omp5.o thread.o libgomp.o stubs.o
	@echo [LD] $@
	@$(CROSS)ld.bfd -T lscript.ld -g -o $@ -e start $^ ../u-boot/arch/arm/lib/lib.a -Map=$(basename $@).map 

%.bin : %.axf
	@echo [OBJCOPY] $<
	@$(CROSS)objcopy $(SECT) -O binary $< $@


clean:
	@echo [CLEAN}
	@rm -f *.o *.su *.axf *.bin
