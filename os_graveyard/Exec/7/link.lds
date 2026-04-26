OUTPUT_FORMAT("elf64-x86-64")
ENTRY(start)

SECTIONS
{
    . = 0xffff800000200000;
	
    .text : {
        *(.text)
    }

    .rodata : {
        *(.rodata)
    }

    . = ALIGN(16);
    .data : {
        *(.data)
    }

    .bss : {
        PROVIDE(bss_start = .);
        *(.bss)
        PROVIDE(bss_end = .);
    }
    
    PROVIDE(end = .);
}
