/*
 * clock.c
 *
 * Detect PLL out clk frequency and implement a udelay routine.
 *
 * Author: Seeger Chin
 * e-mail: seeger.chin@gmail.com
 *
 * Copyright (C) 2006 Ingenic Semiconductor Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#include <bsp.h>
#include <jz4740.h>


const static int FR2n[] = {
    1, 2, 3, 4, 6, 8, 12, 16, 24, 32
};

static unsigned int iclk;

void detect_clock(void)
{
    unsigned int cfcr, pllout;
    cfcr = REG_CPM_CPCCR;
    pllout = (__cpm_get_pllm() + 2)* EXTAL_CLK / (__cpm_get_plln() + 2);
    iclk = pllout / FR2n[__cpm_get_cdiv()];
//  dprintf("EXTAL_CLK = %dM PLL = %d iclk = %d\r\n",EXTAL_CLK / 1000 /1000,pllout,iclk);
}

void udelay(unsigned int usec)
{
    unsigned int i = usec * (iclk / 2000000);
    __asm__ __volatile__ (
        "\t.set noreorder\n"
        "1:\n\t"
        "bne\t%0, $0, 1b\n\t"
        "addi\t%0, %0, -1\n\t"
        ".set reorder\n"
        : "=r" (i)
        : "0" (i)
    );
}

void mdelay(unsigned int msec)
{
    int i;
    for (i = 0; i < msec; i++) {
        udelay(1000);
    }

}
