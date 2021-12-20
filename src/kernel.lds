ENTRY(_start)
SECTIONS
{
	. = 0x00008000;
	_kernelspace_start = .;
	.TT :
	{
		_TT_start = .;
		KEEP (*(.TT))
		_TT_end = .;
	}

	.TT2 :
	{
		_TT2_start = .;
		KEEP (*(.TT2))
	}
	. = ALIGN(0x1000);
	_TT2_end = .;

	. = ALIGN(0x100000);
	_kerneltext_start = .;
	.init : { *(.init) }

	.kerneltext : {
		*(.text .text.*)
	}
	_kerneltext_end = .;


	. = ALIGN(0x100000);

	.kerneldata : {
		_kerneldata_start = .;
		*(.data .data.*)
	}

	.kernelbss : {
		*(.bss .bss.*)
	}

	. = ALIGN(0x10000);
	.kernelrodata : {
		_kernelrodata_start = .;
		*(.rodata .rodata.*)
	}
	_kernelrodata_end = .;

	. = ALIGN(0x100000);
	_kernelstacks_start = .;
	.kernelstacks : 	{
		KEEP (*(.kernelstacks))
		_kernelstacks_end = .;
	}

	_kernelspace_end = .;


	. = ALIGN(0x100000);
	_userspace_start = .;

	.usertext : {
		_usertext_start = .;
		*(.usertext)
		_usertext_end = .;
	}

	. = ALIGN(0x100000);

	.userdata : {
		_userdata_start = .;
		*(.userdata)
	}

	.userbss : {
		*(.bss)
	}

	. = ALIGN(0x1000);
	_userbss_end = .;

	.userrodata : {
		_userrodata_start = .;
		*(.userrodata)
	}
	. = ALIGN(0x1000);
	_userrodata_end = .;

	. = ALIGN(0x100000);
	.userstacks : 	{
		_userstacks_start = .;
		KEEP (*(.userstacks))
		_userstacks_end = .;
	}

	_userspace_end = .;
}

