#include <kprint.h>
#include <config.h>
#include <bit_op.h>

#define UART_BASE (0x7E201000 - 0x3F000000)

//Flag Register
#define TXFE	7
#define RXFF	6
#define TXFF	5
#define RXFE	4
#define BUSY	3

//Line Ctrl Register
#define FEN	(1 << 4)

//IMSC Register
#define RXIM	(1 << 4)
#define TXIM	(1 << 5)

//Ctrl Register
#define UARTEN	(1 << 0)
#define TXE	(1 << 8)
#define RXE	(1 << 9)

struct uart {
	unsigned int dr;
	unsigned int rsrecr;
	unsigned int unused0[4];
	unsigned int fr;
	unsigned int unused1;
	unsigned int ilpr;
	unsigned int ibrd;
	unsigned int fbrd;
	unsigned int lcrh;
	unsigned int cr;
	unsigned int ifls;
	unsigned int imsc;
	unsigned int  ris;
	unsigned int mis;
	unsigned int icr;
	unsigned int dmacr;
	unsigned int unused2[13];
	unsigned int itip;
	unsigned int itop;
	unsigned int tdr;
};

struct uart_buffer {
	char buffer[UART_INPUT_BUFFER_SIZE];
	int write_head;
	int read_head;
	int length;
} uart_buffer = {{},0,0,0};


static volatile
struct uart * const uart_port = (struct uart *)UART_BASE;


void uart_init()
{
	uart_port->lcrh &= (~FEN);
	uart_port->imsc |= RXIM;

	uart_port->cr |= UARTEN;
	uart_port->cr |= TXE;
	uart_port->cr |= RXE;
}

int send_available()
{
	return !BITSET(uart_port->fr, TXFF);
}

int read_available()
{
	return !BITSET(uart_port->fr,RXFE);
}

int uart_send (unsigned char c)
{
	int j = 0;
	while (!send_available() && (j < 0xFFFF)) {
		//waiting for FIFO
		j++;
		if (j == 0xFFFF)
			return -1;
	}

	uart_port->dr = c;
	return 0;
}

int uart_read ()
{
	if (read_available()) {
		return (uart_port->dr);
	}
	return -1;
}

int save_to_buffer ()
{
	if(read_available()){
		if(uart_buffer.length == UART_INPUT_BUFFER_SIZE){
			uart_read();
			return -1; //full
		}
		uart_buffer.buffer[uart_buffer.write_head] = uart_read();
		uart_buffer.write_head++;
		uart_buffer.length++;
		uart_buffer.write_head %= UART_INPUT_BUFFER_SIZE;
		return 0;
	}
	return 1;
}

int save_elem_to_buffer (char c)
{
	if(uart_buffer.length == UART_INPUT_BUFFER_SIZE){
		return -1; //full
	}
	uart_buffer.buffer[uart_buffer.write_head] = c;
	uart_buffer.write_head++;
	uart_buffer.length++;
	uart_buffer.write_head %= UART_INPUT_BUFFER_SIZE;
	return 0;
}

int get_from_buffer()
{
	if (uart_buffer.length <= 0){
		return -1;
	} else {
		char out = uart_buffer.buffer[uart_buffer.read_head];
		uart_buffer.read_head++;
		uart_buffer.read_head %= UART_INPUT_BUFFER_SIZE;
		uart_buffer.length--;
		return out;
	}
}
