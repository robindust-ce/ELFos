#include <bit_op.h>
#include <kprint.h>
#include <linker_variables.h>

#include <tcb_queue.h>

#include <MMU_init.h>

#define AP 10
#define PXN 0
#define XN 4

#define GPU_PERIPHERAL_BASE 0x3E000000
#define DEBUG_ROM_BASE 0x40020000
#define MB_OFFSET (1 << 20)
#define KB4_OFFSET (1 << 12)

unsigned int __attribute__((section (".TT"))) L11tab[4096];
L2table __attribute__((section (".TT2"))) L2tab_process[PROCESS_COUNT];
L2table __attribute__((section (".TT2"))) L2tab_Idle;
L2table __attribute__((section (".TT2"))) L2tab_firstMB;
L2table __attribute__((section (".TT2"))) L2tab_userdata;

unsigned int __attribute__((section (".kernelstacks"))) stack_space[0x140000];

unsigned int stack_size;

void _setupMMU(unsigned int L1addr);

unsigned int get_kernelstack_address () {
	return kernelstacks_end;
}

unsigned int get_userstack_address () {
	return userstacks_end;
}

int set_section_entry (enum TEXAP access, int pxn, int xn, unsigned int Section_addr) {

	if (xn !=0) xn = 1;
	if (pxn !=0) pxn = 1;

	if (Section_addr % (1 << 20) != 0) { //20 bit aligned?
		kprintf("Error: L1 Address not aligned\n");
		return -2;
	}

	int index = (Section_addr >> 20);
	if (index >= 4096) {
		kprintf("Error: L1 Index out of bounds\n");
		return -1;
	}

	unsigned int entry = set_bits(Section_addr, access, AP);
	entry = set_bits(entry, pxn, PXN);
	entry = set_bits(entry, xn, XN);
	entry = set_bits(entry, 1, 1);

	unsigned int * L1tab = (unsigned int *) &L11tab;
	L1tab[index] = entry;

	return 0;
}

int set_L2_entry (unsigned int L2_addr, int Section_addr, int pxn)
{
	if (pxn !=0) pxn = 1;

	if (L2_addr % (1 << 10) != 0) { //10 bit aligned?
		kprintf("Error: L2 Address not aligned\n");
		return -2;
	}

	int index = (Section_addr >> 20);
	if (index >= 4096) {
		kprintf("Error: L1 Index out of bounds\n");
		return -1;
	}

	unsigned int entry = set_bits(L2_addr, 1, 0);
	entry = set_bits(entry, pxn, 2);

	unsigned int * L1tab = (unsigned int *) &L11tab;
	L1tab[index] = entry;

	return 0;
}

int set_L2_page_entry (unsigned int L2_addr, enum TEXAP access, int xn, unsigned int Section_addr)
{
	if (xn !=0) xn = 1;

	int index = (bitslice(Section_addr, 19, 12) >> 12);
	if (index >= 256) {
		kprintf("Error: L2 Index out of bounds\n");
		return -1;
	}

	if (Section_addr % (1 << 12) != 0) { //12 bit aligned?
		kprintf("Error: L2 Page Address not aligned\n");
		return -2;
	}

	unsigned int entry = set_bits(Section_addr, access, 4);
	entry = set_bits(entry, xn, 0);
	entry = set_bits(entry, 1, 1);

	unsigned int * L22tab = (unsigned int *) L2_addr;
	L22tab[index] = entry;

	return 0;
}

int map_sections(unsigned int start_addr, unsigned int end_addr, enum TEXAP access, int pxn, int xn)
{
	int out = 0;
	while (start_addr < end_addr) {
		out += set_section_entry(access,pxn,xn, start_addr);
		start_addr += MB_OFFSET;
	}
	return out;
}

int map_pages(unsigned int L2_addr, unsigned int start_addr, unsigned int end_addr, enum TEXAP access, int xn)
{
	int out = 0;

	while (start_addr < end_addr) {
		out += set_L2_page_entry(L2_addr, access, xn, start_addr);
		start_addr += KB4_OFFSET;
	}
	return out;
}

void stacks_init()
{
	stack_size = bitslice((MB_OFFSET - (userrodata_end - userdata_start))/4, 31, 12);
}

void MMU_init ()
{
	stacks_init();
	//Mit Fault Entries initialisieren
	unsigned int * L1tab = (unsigned int *) &L11tab;
	for (int i = 0; i < 4096; i++) {
		L1tab[i] = 0;
	}
	L2table * L2tab_proc = (L2table *) &L2tab_process;
	L2table * L2tab_usrdata = (L2table *) &L2tab_userdata;
	L2table * L2tab_fMB = (L2table *) &L2tab_firstMB;

	//L2 für erstes MB (TT + ELFELFELF Bereich)
	set_L2_entry((unsigned int) L2tab_fMB, 0, 1);
	map_pages((unsigned int) L2tab_fMB, 0, kernelspace_start, noAccess, 1);
	map_pages((unsigned int) L2tab_fMB, kernelspace_start, TT2_end, RW_sys, 1);
	map_pages((unsigned int) L2tab_fMB, TT2_end, kerneltext_start, noAccess, 1);

	//Kernel text
	map_sections(kerneltext_start, kerneltext_end, R_sys, 0, 0);

	//Kerneldata, -bss, -rodata
	map_sections(kerneldata_start, kernelrodata_start, RW_sys, 1, 1);
	map_sections(kernelrodata_start, kernelrodata_end, R_sys, 1, 1);

	//Kernel stacks
	map_sections(kernelstacks_start, kernelstacks_end, RW_sys, 1, 1);

	//User text
	map_sections(usertext_start, usertext_end, R_all, 1, 0);

	//userdata, -bss, -rodata
	set_L2_entry((unsigned int) L2tab_usrdata, userdata_start, 1);
	map_pages((unsigned int) L2tab_usrdata, userdata_start, userbss_end, RW_all, 1);
	map_pages((unsigned int) L2tab_usrdata, userbss_end, userrodata_end, R_usr, 1);
	map_pages((unsigned int) L2tab_usrdata, userrodata_end, userstacks_start, noAccess, 1);


	//Prozesse voreinander schützen
	map_sections(userstacks_start, userstacks_end, RW_sys, 1, 1);

	//L2 Tabellen für Userstacks initialisieren
	unsigned int x = userstacks_start;
	for (unsigned int i = 0; i < PROCESS_COUNT; i++) {

		map_pages((unsigned int) &L2tab_proc[i], x, x+MB_OFFSET,RW_all, 1);
		x += MB_OFFSET;
	}

	L2table * L2table_Idle = (L2table *) &L2tab_Idle;
	map_pages((unsigned int) L2table_Idle, x, x+KB4_OFFSET, noAccess, 1);
	map_pages((unsigned int) L2table_Idle, x+KB4_OFFSET, x+MB_OFFSET, RW_all, 1);

	//Peripherals
	map_sections(GPU_PERIPHERAL_BASE, DEBUG_ROM_BASE, RW_sys, 1, 1);

	_setupMMU((unsigned int) L11tab);
	return;
}






