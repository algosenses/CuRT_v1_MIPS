#ifndef CURT_MACH_JZ4740_PORT_H
#define CURT_MACH_JZ4740_PORT_H

#include "arch/mips/jz4740/jz4740.h"
#include "kernel/types.h"
#include "kernel/thread.h"

#define OS_TICKS_PER_SEC        (1)

stk_t *init_thread_stack(THREAD_FUNC func,
                         void *pdata,
                         stk_t *stk_top_ptr);

void init_os_timer();

void interrupt_handler();

void init_interrupt_control();

/* context switching routines : architecture implementations */
extern void restore_context();
extern void context_switch();
extern int context_switch_in_interrupt();
extern cpu_sr_t save_cpu_sr();
extern void restore_cpu_sr(cpu_sr_t cpu_sr);

#endif /* ! CURT_MACH_PXA255_PORT_H */

