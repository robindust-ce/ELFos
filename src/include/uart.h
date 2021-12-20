#ifndef UART_H
#define UART_H

void uart_init();
int send_available();
int read_available();
void uart_send (unsigned char c);
int uart_read ();
int get_from_buffer();
char save_to_buffer();
int save_elem_to_buffer();

#endif

