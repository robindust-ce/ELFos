#include <interrupts.h>
#include <thread_management.h>
#include <tcb_queue.h>
#include <MMU_init.h>

#include <uart.h>
#include <localtimer.h>

#include <kprint.h>

void elf();

extern void app();
extern void main();


void start_kernel(void)
{
	uart_init();
	interrupt_init();
	tcbq_init();
	tcb_init();
	timer_init();
	MMU_init();

	//Startup message, JUST FOR FUN!, auskommentieren wenn nervig
	elf();

	char x = 'x';
	if (thread_create(main, (void *) &x, 0,'f') != 0) {
		kprintf("Error creating application thread %i\n",thread_create(main, (void *) &x, 0, 'f'));
	}

	while(1) {}
}



