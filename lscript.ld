ENTRY(start)

MEMORY
    {
  RAM (xrw)       : ORIGIN = 0x24000000, LENGTH = 512K
  RAM_D1 (xrw)    : ORIGIN = 0x20000000, LENGTH = 128K
  RAM_D2 (xrw)    : ORIGIN = 0x30000000, LENGTH = 288K
  RAM_D3 (xrw)    : ORIGIN = 0x38000000, LENGTH = 64K
  ITCMRAM (xrw)   : ORIGIN = 0x00000000, LENGTH = 64K
    }

SECTIONS
    {
    .text :
        {
         _text_start = .;
        *(.text)
        *(.text.*)
        KEEP (*(.init))
        KEEP (*(.fini))
         _text_end = .;
        } > RAM

    .rodata ALIGN(16): 
        { 
        _rodata_start = .;
        *(.rodata)
        *(.rodata.*)
        _rodata_end = .;
        } > RAM

    .ctors ALIGN(16):
        {
        _ctors_start = . ;
        KEEP(*(.preinit_array))
        KEEP(*(.init_array))
        KEEP (*(SORT(.ctors.*)))
        KEEP (*(.ctors))
       _ctors_end = . ;
        } > RAM

        
  .data ALIGN(16):
        {
        _data_start = .;
        *(.data)
        *(.data.*)
        _data_end = .;
        } > RAM
  
    .bss ALIGN(16):
        {
        _bss_start = .;
        __bss_start = .;
        *(.bss)
        *(.bss.*)
        *(COMMON)
        _bss_end = .;
        _end = .;
        } > RAM
    }
