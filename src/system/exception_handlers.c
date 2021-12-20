#include <interrupts.h>
#include <thread_management.h>
#include <tcb_queue.h>

#include <localtimer.h>
#include <uart.h>

#include <kprint.h>
#include <bit_op.h>
#include <exception_handlers.h>
#include <svc_handler.h>
#include <regdump.h>


void _stop_system();

extern void user_thread(void * x);

enum debug_modes {
	off = 0,
	on
};

void uart_interrupt(unsigned int * sp);

int debug_mode = off;

void toggle_debug_mode ()
{
	debug_mode = (debug_mode + 1) % 2;
}

void undef_handler(unsigned int * sp, int lr, int cpsr, int spsr)
{
	int mode = bitslice(spsr, 4, 0);
	if (mode == USR_MOD) {
		//Thread anhalten
		change_thread(sp+MODE_SPECIFIC_SIZE, finished);
		reset_timer();
	} else {
		//system anhalten
		kprintf("\n");
		sp[LR_CURRENT_STACK] = (int) (&_stop_system);
	}

	handler_output(und, sp, lr, cpsr, spsr, mode);
};

void swi_handler(unsigned int * sp, int lr, int cpsr, int spsr)
{
	int * svc_addr = ((int *) (lr-4));

	int imm = bitslice(*svc_addr, 15, 0);
	int mode = bitslice(spsr, 4, 0);

	if (mode != USR_MOD) {
		//system anhalten
		kprintf("\n");
		sp[LR_CURRENT_STACK] = (int) (&_stop_system);
		handler_output(swi, sp, lr, cpsr, spsr, mode);
	} else {
		if (usr_svc(sp, imm) != 0) { //unknown SVC
			change_thread(sp+MODE_SPECIFIC_SIZE, finished);
			reset_timer();
			handler_output(swi, sp, lr, cpsr, spsr, mode);
		}
	}
};

void prefetch_handler(unsigned int * sp, int lr, int cpsr, int spsr)
{
	int mode = bitslice(spsr, 4, 0);
	if (mode == USR_MOD) {
		//Thread anhalten
		change_thread(sp+MODE_SPECIFIC_SIZE, finished);
		reset_timer();
	} else {
		//system anhalten
		kprintf("\n");
		sp[LR_CURRENT_STACK] = (int) (&_stop_system);
	}
	handler_output(pref_abt, sp, lr, cpsr, spsr, mode);
};

void data_handler(unsigned int * sp, int lr, int cpsr, int spsr)
{
	int mode = bitslice(spsr, 4, 0);
	if (mode == USR_MOD) {
		//Thread anhalten
		change_thread(sp+MODE_SPECIFIC_SIZE, finished);
		reset_timer();
	} else {
		//system anhalten
		kprintf("\n");
		sp[LR_CURRENT_STACK] = (int) (&_stop_system); //+4
	}
	handler_output(data_abt, sp, lr, cpsr, spsr, mode);
};

void irq_handler(unsigned int * sp, int lr, int cpsr, int spsr)
{

	if (BITSET(get_pending(),UART_INT_POS)) {
		uart_interrupt(sp);

	} else if (get_interrupt_flag()) {
		clr_interrupt_flag();
// 		kprintf("!");
		schedule_timer(sp+MODE_SPECIFIC_SIZE);
		reset_timer();
	}

	if (debug_mode == on) {
		handler_output(inter, sp, lr, cpsr, spsr, IRQ_MOD);
	}
};

void fiq_handler(unsigned int * sp, int lr, int cpsr, int spsr)
{
	if (debug_mode == on) {
		handler_output(inter, sp, lr, cpsr, spsr,FIQ_MOD);
	}
};

void uart_interrupt(unsigned int * sp)
{
	int l;
	if ((l = uart_read()) == -1) {
		return;
	}

	save_elem_to_buffer((char) l);
	if (get_running() == IDLE_ID) {
		check_waitcq();
		change_thread(sp+MODE_SPECIFIC_SIZE, finished);
		reset_timer();
	}
}
