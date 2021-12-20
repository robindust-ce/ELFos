#include <MMU_init.h>
#include <linker_variables.h>
#include <bit_op.h>

extern void _invalidate_tlb();

extern L2table L2tab_process[PROCESS_COUNT];
extern L2table L2tab_Idle;
extern unsigned int stack_size;

void memcopy (unsigned int * src_start, unsigned int * src_end, unsigned int * dest)
{
	while (src_start < src_end) {
		*dest = *src_start;
		src_start++;
		dest++;
	}
}

void copy_data_section (unsigned int processID)
{
	L2table * L2tab_proc = (L2table *) &L2tab_process;

	//get 4kB aligned base address of L2tab
	int base_address = bitslice(L2tab_proc[processID].sec[0], 31, 12);

	memcopy((unsigned int *) userdata_start, (unsigned int *) userrodata_end, (unsigned int *) base_address);

	unsigned int start = base_address;
	unsigned int offset = userbss_end - userdata_start;
	map_pages((unsigned int) &L2tab_proc[processID], start, start+offset, RW_all, 1);

	start = start+offset;
	offset = userrodata_end- userbss_end;
	map_pages((unsigned int) &L2tab_proc[processID], start, start+offset, R_usr, 1);

	start = base_address+MB_OFFSET;

	for (int i = 1; i <= 4; i++) {
		map_pages((unsigned int) &L2tab_proc[processID], start-(i*stack_size), start-(i*stack_size)+KB4_OFFSET,noAccess, 1);
	}
}

void change_context(int processID)
{
	_invalidate_tlb();

	L2table * L2tab_proc;
	if (processID == -1) {
		L2tab_proc = (L2table *) &L2tab_Idle;
		set_L2_entry((unsigned int) L2tab_proc, userdata_start, 1);
	} else {
		L2tab_proc = (L2table *) &L2tab_process;
		set_L2_entry((unsigned int) &L2tab_proc[processID], userdata_start, 1);
	}
}




