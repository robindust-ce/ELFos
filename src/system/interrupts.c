#define INTERRUPT_BASE (0x7E00B000 - 0x3F000000 + 0x200)

#define UART_INT (1 << 25)

struct interrupt_reg {
	unsigned int irq_basic_pend;
	unsigned int irq_pend_1;
	unsigned int irq_pend_2;
	unsigned int fiq_ctrl;
	unsigned int irq_enable_1;
	unsigned int irq_enable_2;
	unsigned int irq_basic_enable;
	unsigned int irq_disable_1;
	unsigned int irq_disable_2;
	unsigned int irq_basic_disable;
};

static volatile
struct interrupt_reg * const interrupt_setup = (struct interrupt_reg *)INTERRUPT_BASE;

unsigned int get_pending ()
{
	return interrupt_setup->irq_basic_pend;
}

void interrupt_init()
{
	interrupt_setup->irq_enable_2 |= UART_INT;
}
