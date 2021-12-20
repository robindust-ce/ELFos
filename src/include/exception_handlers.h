#ifndef _EXCEPTION_HANDLERS_H_
#define _EXCEPTION_HANDLERS_H_

//Stack Offsets
#define MODE_SPECIFIC_SIZE 18
#define USR_SPECIFIC_SIZE 3
#define LR_CURRENT_STACK 34
#define R0_CURRENT_STACK (MODE_SPECIFIC_SIZE+USR_SPECIFIC_SIZE)
#define R1_CURRENT_STACK (R0_CURRENT_STACK+1)
#define R2_CURRENT_STACK (R1_CURRENT_STACK+1)

//Modusbits zur Erkennung im PSR
#define USR_MOD 	0b10000
#define FIQ_MOD 	0b10001
#define IRQ_MOD 	0b10010
#define SVC_MOD 	0b10011
#define ABT_MOD 	0b10111
#define UND_MOD 	0b11011
#define SYS_MOD 	0b11111

//EX_Offsets für link register
#define DATA_ABT_OFF 	8
#define UND_OFF		4
#define SWI_OFF		4
#define PREF_ABT_OFF	4
#define INTERRUPT_OFF	4

//DFSR Bit
#define RW_DFSR	11

//EX-Codes
enum ex_codes {
	data_abt = 1,
	und,
	swi,
	pref_abt,
	inter
};


void reset_handler();

void undef_handler();

void swi_handler();

void prefetch_handler();

void data_handler();

void notused_handler();

void irq_handler();

void fiq_handler();

void toggle_debug_mode();


#endif

