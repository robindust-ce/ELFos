SECTIONS
{
	. = 0x00008000;
	.usertext : {
		*(.text .text.*)
	}
	.userdata : {
		*(.data .data.*)
	}
	.userrodata : {
		*(.rodata .rodata.*)
	}
	.userbss : {
		*(.bss .bss.*)
	}
}

