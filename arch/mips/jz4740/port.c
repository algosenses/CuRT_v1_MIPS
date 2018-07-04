/*
 * port.c
 */

#include "port.h"
#include "lib/stdio.h"
#include "kernel/list.h"
#include "kernel/thread.h"
#include "kernel/kernel.h"

#include <mipsregs.h>
#include <bsp.h>
#include <jz4740.h>
#include "context.h"

typedef unsigned int cpu_reg_t;

typedef struct {
    cpu_reg_t regs[32];
} context_t;

unsigned int kernel_stack[4096];

/**
 * @brief Initialize the thread stack.
 *
 * Initialize the thread stack value with the given thread function, and
 * then save registers.
 * @param func
 * @param pdata
 * @param stk_top_ptr
 * @retval
 */

u32_t CP0_wGetSR(void);

register U32 $GP __asm__ ("$28");

static u32_t wSR = 0;
static u32_t wGP = 0;
stk_t *init_thread_stack(THREAD_FUNC func, void *pdata, stk_t *stk_top_ptr)
{
    cpu_reg_t *regs_ptr = (cpu_reg_t *)stk_top_ptr;
    int i;

    regs_ptr -= sizeof(context_t);

    if (wSR == 0) {
        wSR = CP0_wGetSR();
        wSR &= 0xfffffffe;
        wSR |= 0x001;

        wGP = $GP;
    }

//    for (i = 0; i < sizeof(context_t); i++) {
//        regs_ptr[i] = 0x55AA55AAL;
//    }

    regs_ptr[0]  = 0x0;
    regs_ptr[1]  = 0x01010101L;
    regs_ptr[2]  = (stk_t) wGP;     // gp
    regs_ptr[3]  = 0x03030303L;
    regs_ptr[4]  = 0x04040404L;
    regs_ptr[5]  = 0x05050505L;
    regs_ptr[6]  = 0x06060606L;
    regs_ptr[7]  = 0x07070707L;
    regs_ptr[8]  = 0x08080808L;
    regs_ptr[9]  = 0x09090909L;
    regs_ptr[10]  = 0x10101010L;
    regs_ptr[11]  = 0x11111111L;
    regs_ptr[12]  = 0x12121212L;
    regs_ptr[13]  = 0x13131313L;
    regs_ptr[14]  = 0x14141414L;
    regs_ptr[15]  = 0x15151515L;
    regs_ptr[16]  = 0x16161616L;
    regs_ptr[17]  = 0x17171717L;
    regs_ptr[18]  = 0x18181818L;
    regs_ptr[19]  = 0x19191919L;
    regs_ptr[20]  = 0x20202020L;
    regs_ptr[21]  = 0x21212121L;
    regs_ptr[22]  = 0x22222222L;
    regs_ptr[23]  = 0x23232323L;
    regs_ptr[24] = (stk_t) pdata;
    regs_ptr[25]  = 0x25252525L;
    regs_ptr[26]  = 0x26262626L;
    regs_ptr[27]  = 0x27272727L;
    regs_ptr[28] = (stk_t) wSR; /* C0_SR: HW0 = En , IE = En */
    regs_ptr[29] = (stk_t) func;
    regs_ptr[30]  = 0x30303030L;
    regs_ptr[31]  = 0x31313131L;

    return (regs_ptr);
}

/**
 * @brief Initialize CuRT Timer
 *
 * Once OS Timer is initialized, the timer is about to work.
 * Note: the multi-tasking environment (setup by start_curt) will be invoked
 * after this routine.
 */
void init_os_timer()
{
    printf("init_os_timer.\n");
    JZ_StartTicker(OS_TICKS_PER_SEC);
}

/**
 * @brief Interrupt handler
 *
 * This function is invoked in the IRQ service routine.
 * source: when calling advance_time_tick, ticks advance.
 */

/*
void interrupt_handler()
{
}
*/

/**
 * @brief Initialize CPU interrupt control
 *
 * Enable the issue of interrupts.
 */
void init_interrupt_control()
{
    interrupt_init();
}
