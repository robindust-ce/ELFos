#include <config.h>

#define TIMER_ADDR 0x40000034
#define TIMER_CLR_ADDR 0x40000038

//Timer reg
#define TIMER_ENABLE (1 << 28)
#define INTERRUPT_ENABLE (1 << 29)

//Timer clr reg
#define INTERRUPT_FLAG (1 << 31)
#define TIMER_RELOAD (1 << 30)

static volatile int * const local_timer = (int *) TIMER_ADDR;
static volatile int * const timer_clr = (int *) TIMER_CLR_ADDR;

int timer_enable ()
{
	*local_timer |= TIMER_ENABLE;
	return 0;
}

int interrupt_enable()
{
	*local_timer |= INTERRUPT_ENABLE;
	return 0;
}

int clr_interrupt_flag ()
{
	*timer_clr |= INTERRUPT_FLAG;
	return 0;
}

int reset_timer ()
{
	*timer_clr |= TIMER_RELOAD;
	return 0;
}

int timer_init ()
{
	*local_timer = LOCAL_TIMER_US;
	timer_enable();
	interrupt_enable();
	clr_interrupt_flag();
	return 0;
}

unsigned int get_interrupt_flag ()
{
	return (*local_timer & INTERRUPT_FLAG);
}











