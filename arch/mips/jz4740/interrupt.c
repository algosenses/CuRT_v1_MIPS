#include <stdio.h>
//#include "mips.h
#include <mipsregs.h>
#include <bsp.h>
#include <jz4740.h>

#define NUM_DMA     (6)
#define NUM_GPIO    (128)
#define IRQ_MAX	    (IRQ_GPIO_0 + NUM_GPIO)

static struct {
	void (*handler)(unsigned int);
	unsigned int arg;
} irq_table[IRQ_MAX];

static void default_handler(unsigned int arg)
{
}

static unsigned int dma_irq_mask = 0;
static unsigned int gpio_irq_mask[4] = {0};

void enable_irq(unsigned int irq)
{
	register unsigned int t;
	if ((irq >= IRQ_GPIO_0) && (irq <= IRQ_GPIO_0 + NUM_GPIO)) {
		__gpio_unmask_irq(irq - IRQ_GPIO_0);
		t = (irq - IRQ_GPIO_0) >> 5;
		gpio_irq_mask[t] |= (1 << ((irq - IRQ_GPIO_0) & 0x1f));
		__intc_unmask_irq(IRQ_GPIO0 - t);
	} else if ((irq >= IRQ_DMA_0) && (irq < IRQ_DMA_0 + NUM_DMA)) {
		__dmac_channel_enable_irq(irq - IRQ_DMA_0);
		dma_irq_mask |= (1 << (irq - IRQ_DMA_0));
		__intc_unmask_irq(IRQ_DMAC);
	} else if (irq < 32)
		__intc_unmask_irq(irq);
}

void disable_irq(unsigned int irq)
{
	register unsigned int t;

	if ((irq >= IRQ_GPIO_0) && (irq <= IRQ_GPIO_0 + NUM_GPIO)) {
		__gpio_mask_irq(irq - IRQ_GPIO_0);
		t = (irq - IRQ_GPIO_0) >> 5;
		gpio_irq_mask[t] &= ~(1 << ((irq - IRQ_GPIO_0) & 0x1f));
		if (!gpio_irq_mask[t])
			__intc_mask_irq(IRQ_GPIO0 - t);
	} else if ((irq >= IRQ_DMA_0) && (irq < IRQ_DMA_0 + NUM_DMA)) {
		__dmac_channel_disable_irq(irq - IRQ_DMA_0);
		dma_irq_mask &= ~(1 << (irq - IRQ_DMA_0));
		if (!dma_irq_mask)
			__intc_mask_irq(IRQ_DMAC);
	} else if (irq < 32)
		__intc_mask_irq(irq);
}

void ack_irq(unsigned int irq)
{
	__intc_ack_irq(irq);
	if ((irq >= IRQ_GPIO_0) && (irq <= IRQ_GPIO_0 + NUM_GPIO)) {
		__intc_ack_irq(IRQ_GPIO0 - ((irq - IRQ_GPIO_0)>>5));
		__gpio_ack_irq(irq - IRQ_GPIO_0);
	} else if ((irq >= IRQ_DMA_0) && (irq <= IRQ_DMA_0 + NUM_DMA)) {
		__intc_ack_irq(IRQ_DMAC);
	} else if (irq < 32) {
		__intc_ack_irq(irq);
		if (irq == IRQ_TCU0) {
		  __tcu_clear_full_match_flag(0);   //add by Regen
		}
	}
}

static unsigned long ipl = 0;

#define INTC_IRQ_COMMON_MASK    ((1 << IRQ_DMAC)  | \
    						     (1 << IRQ_GPIO3) | \
    						     (1 << IRQ_GPIO2) | \
    						     (1 << IRQ_GPIO1) | \
    						     (1 << IRQ_GPIO0)   )

static inline int intc_irq(void)
{
	register int irq = 0;

	ipl = REG_INTC_IPR;

	if (ipl == 0) {
		return -1;
	}

	/* find out the real irq defined in irq_xxx.c */
	for (irq = 31; irq >= 0; irq--) {
		if (ipl & (1 << irq)) {
			break;
		}
	}

	if (ipl & INTC_IRQ_COMMON_MASK)
	{
		switch (irq) {
		case IRQ_DMAC:
			irq = __dmac_get_irq() + IRQ_DMA_0;
			break;

		case IRQ_GPIO3:
			irq = __gpio_group_irq(3) + IRQ_GPIO_0 + 96;
			break;

		case IRQ_GPIO2:
			irq = __gpio_group_irq(2) + IRQ_GPIO_0 + 64;
			break;

		case IRQ_GPIO1:
			irq = __gpio_group_irq(1) + IRQ_GPIO_0 + 32;
			break;

		case IRQ_GPIO0:
			irq = __gpio_group_irq(0) + IRQ_GPIO_0;
			break;
		}
    }

	return irq;
}

void interrupt_init(void)
{
	unsigned int i;
	ipl = 0;
	for (i = 0;i < IRQ_MAX; i++) {
		disable_irq(i);
		irq_table[i].handler = default_handler;
		irq_table[i].arg = i;
	}
}

int request_irq(unsigned int irq, void (*handler)(unsigned int), unsigned arg)
{
	if (irq > IRQ_MAX) {
		return -1;
	}

	if (irq_table[irq].handler != default_handler) {
		return -1;
	}

	irq_table[irq].handler = handler;
	irq_table[irq].arg = arg;
	enable_irq(irq);

	return 0;
}

void free_irq(unsigned int irq)
{
	disable_irq(irq);
	irq_table[irq].handler = default_handler;
	irq_table[irq].arg = irq;
}

typedef struct CP0_tstREGS
{
	U32 wStatus;
	U32 wCause;
} CP0_tstREGS;


void interrupt_handler(CP0_tstREGS *pstC0)
{
	register int irq = intc_irq();

	if (irq < 0) {
		return;
    }

	ack_irq(irq);
	irq_table[irq].handler(irq_table[irq].arg);
}

void cli(void)
{
	register unsigned int t;
	t = read_c0_status();
	t &= ~1;
	write_c0_status(t);
}

unsigned int mips_get_sr(void)
{
	unsigned int t = read_c0_status();
	return t;
}

void sti(void)
{
	register unsigned int t;
	t = read_c0_status();
	t |= 1;
	t &= ~2;
	write_c0_status(t);
}

unsigned int spin_lock_irqsave(void)
{
	register unsigned int t;
	t = read_c0_status();
	write_c0_status((t&(~1)));
	return t;
}

void spin_unlock_irqrestore(unsigned int val)
{
	write_c0_status(val);
}
