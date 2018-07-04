/*
 * mips_port.S
 */

#include "mips.h"
#include "sysdefs.h"
#include "frmwrk.h"
#include "jz4740.h"
#include "context.h"


	.text
	.global CP0_wGetSR
	.global restore_context
	.global context_switch
	.global context_switch_in_interrupt
	.global save_cpu_sr
	.global restore_cpu_sr

LEAF(CP0_wGetSR)
        MFC0(v0,C0_STATUS)
        jr ra
        nop
END(CP0_wGetSR)

/**
 * @brief Save CPU status.  Disable interrupts.
 *
 * IRQ interrupt status register value is returned to be disabled.
 * @param
 * @retval CPU status value
 * Usage: cpu_sr = save_cpu_sr();
 */
LEAF(save_cpu_sr)
    MFC0(v0, C0_STATUS)
    and v1, v0, 0xfffffffe
    MTC0(v1, C0_STATUS)
    jr ra
    nop
END(save_cpu_sr)

/**
 * @brief The state will restore the value of registers
 *
 * Restore the given value of status register.
 * @param CPU status value
 * @retval
 * Usage: restore_cpu_sr(cpu_sr);
 */
LEAF(restore_cpu_sr)
    MTC0(a0, C0_STATUS)
    jr ra
    nop
END(restore_cpu_sr)

/**
 * @brief Restore the context of the current thread.
 *
 * In multi-tasking environment, restore the context of the thread.
 * @param
 * @retval
 */
LEAF(restore_context)
    lw  t2, current_thread
    lw  sp, 0(t2)
    lw  ra, 0(sp);
        lw  fp, 4(sp);
        lw  gp, 8(sp);
        lw  t9,12(sp);
        lw  t8,16(sp);
        lw  s7,20(sp);
        lw  s6,24(sp);
        lw  s5,28(sp);
        lw  s4,32(sp);
        lw  s3,36(sp);
        lw  s2,40(sp);
        lw  s1,44(sp);
        lw  s0,48(sp);
        lw  t7,52(sp);
        lw  t6,56(sp);
        lw  t5,60(sp);
        lw  t4,64(sp);
        lw  t3,68(sp);
        lw  t2,72(sp);
        lw  t1,76(sp);
        lw  t0,80(sp);
        lw  a3,84(sp);
        lw  a2,88(sp);
        lw  a1,92(sp);
        lw  a0,96(sp);
        lw  v1,100(sp);
        lw  v0,104(sp);
        lw  AT,108(sp);
        lw  k0,112(sp);
        MTC0(k0,C0_STATUS);
        lw  k0,120(sp);
        MTLo k0;
        lw  k0,124(sp);
        MTHi k0      ;
        lw  k0,116(sp);
        addu sp,128;
        jr  k0;
        nop;
END(restore_context)

/**
 * @brief IRQ interrupt service routine
 *
 * IRQ interrupt service routine is called when raised.  Execution order:
 *   (1) IRQ registers are stored on the IRQ stack.
 *   (2) Were running to save the context of a thread.
 *   (3) About to issue the interrupt - interrupt_nesting
 *   (4) Stack pointer is stored in the structure of the current thread.
 *   (5) Call interrupt handler (tick increment).
 *   (6) Change interrupt_nesting in context - exit_interrupt,
 *       otherwise, the thread context of the existing restores.
 * @param
 * @retval
 */
    BSS

    /* 32 bit align */
    ALIGN(2)

    .globl exc_stack_low
exc_stack_low:
    .space _EXC_STKSIZE
    .globl exc_stack_hi
exc_stack_hi:

    ALIGN(4) /* 16 bytes align */
exc_context:
    .space    8

LEAF(irq_service_routine)
    MFC0(k0, C0_EPC)
    STORE_REG_RET(k0)

    lw  t2, current_thread
    sw  sp, 0(t2)

    la  k1, exc_context
    MFC0(t0, C0_STATUS)
    sw  t0, 0(k1)
    MFC0(t0, C0_STATUS)
    sw  t0, 4(k1)

    move k0, sp

//    la gp, _gp

    la sp, exc_stack_hi
    jal interrupt_handler
    nop

    jal exit_interrupt
    nop

    move sp, k0
    RESTORE_REG_ERET()

END(irq_service_routine)

/**
 * @brief Context switching
 *
 * During context switching, the following actions are performed:
 *   (1) Save the registers in thecurrent thread.
 *   (2) The stack pointer is stored in the current structure of the thread.
 *   (3) Run the next thread pointed by scheduler.
 *   (4) The stack pointer in the thread is read and executed.
 *   (5) Restore the registers of thread.
 * @param
 * @retval
 */
LEAF(context_switch)
    STORE_REG_RET(ra)
    lw  t2, current_thread
    sw  sp, 0(t2)

    lw  t3, next_thread
    sw  t3, current_thread
    lw  sp, 0(t3)
    RESTORE_REG_ERET()
END(context_switch)


/**
 * @brief Context switching during interruption.
 *
 * Interrupt the context occurs later due to preemptive threads.
 * @param
 * @retval
 */
LEAF(context_switch_in_interrupt)
    lw  t3, next_thread
    sw  t3, current_thread
    lw  sp, 0(t3)
    RESTORE_REG_ERET()
END(context_switch_in_interrupt)