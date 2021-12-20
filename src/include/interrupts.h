#ifndef INTERRUPTS_H
#define INTERRUPTS_H
#define UART_INT_POS 19

void interrupt_init();
unsigned int get_pending ();

#endif
