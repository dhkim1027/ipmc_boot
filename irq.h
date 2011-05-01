#ifndef __IRQ_H__
#define __IRQ_H__

#include <avr/interrupt.h>

#define barrier() asm volatile("" ::: "memory")
#define cpu_irq_enable()	sei()
#define cpu_irq_disable()	cli()

#define AVR_ENTER_CRITICAL_REGION( ) uint8_t volatile saved_sreg = SREG; \
                                     cli();

#define AVR_LEAVE_CRITICAL_REGION( ) SREG = saved_sreg;

typedef uint8_t irqflags_t;

static inline irqflags_t cpu_irq_save(void)
{
	irqflags_t val = SREG;
	cpu_irq_disable();
	return val;
}

static inline void cpu_irq_restore(irqflags_t flags)
{
	barrier();
	SREG = flags;
}


#endif
