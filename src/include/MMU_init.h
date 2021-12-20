#ifndef _MMU_INIT_H_
#define _MMU_INIT_H_

#define PROCESS_COUNT 8

#define MB_OFFSET (1 << 20)
#define KB4_OFFSET (1 << 12)

//AP and TEX Bits --> Bit 10 to 15 in section entry
enum TEXAP {
	noAccess = 0b000000,
	RW_sys = 0b000001,
	R_sys = 0b100001,
	R_all = 0b100011,
	R_usr = 0b000010,
	RW_all = 0b000011
};


typedef struct L2table {
	unsigned int sec[256];
} L2table;

void MMU_init();
int map_pages(unsigned int L2_addr, unsigned int start_addr, unsigned int end_addr, enum TEXAP access, int xn);
int set_L2_entry (unsigned int L2_addr, int Section_addr, int pxn);
#endif
