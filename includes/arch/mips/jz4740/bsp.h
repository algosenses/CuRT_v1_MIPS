#ifndef __bsp_h__
#define __bsp_h__

#define CFG_EXTAL       12000000

#define EXTAL_CLK       CFG_EXTAL

//#define JZ_EXTAL        EXTAL_CLK  //use in clock.h
#define JZ_EXTAL2	32768      //use in clock.h

#define dprintf(x...) printf(x)

#define USE_NAND  0

#define USE_UART0 1
#define USE_UART1 1

#define USE_MSC   1

#define USE_LCD16 1
#define USE_LCD18 0

#define USE_AIC   1

#define USE_CIM   1

/*-----------------------------------------------------------------------
 * Cache Configuration
 */

#define CFG_DCACHE_SIZE		16384
#define CFG_ICACHE_SIZE		16384
#define CFG_CACHELINE_SIZE	32
/*-----------------------------------------------------------------------
 * SDRAM Info.
 */
#define CONFIG_NR_DRAM_BANKS	1

// SDRAM paramters
#define SDRAM_BW16		0	/* Data bus width: 0-32bit, 1-16bit */
#define SDRAM_BANK4		1	/* Banks each chip: 0-2bank, 1-4bank */
#define SDRAM_ROW		13	/* Row address: 11 to 13 */
#define SDRAM_COL		9	/* Column address: 8 to 12 */
#define SDRAM_CASL		2	/* CAS latency: 2 or 3 */

// SDRAM Timings, unit: ns
#define SDRAM_TRAS		45	/* RAS# Active Time */
#define SDRAM_RCD		20	/* RAS# to CAS# Delay */
#define SDRAM_TPC		20	/* RAS# Precharge Time */
#define SDRAM_TRWL		7	/* Write Latency Time */
#define SDRAM_TREF	        15625	/* Refresh period: 4096 refresh cycles/64ms */

#define CPU_NORMAL_SPEED 1

#ifdef CPU_NORMAL_SPEED
#define CFG_CPU_SPEED		360000000	/* CPU clock: 360 MHz */
#endif

#ifdef CPU_HIGH_SPEED
#define CFG_CPU_SPEED		384000000	/* CPU clock: 384 MHz */
#endif

#define GPIO_PW_I         97
#define GPIO_PW_O         66
#define GPIO_LED_EN       92
#define GPIO_DISP_OFF_N   93
#define GPIO_RTC_IRQ      96
#define GPIO_USB_CLK_EN   29
#define GPIO_CHARG_STAT   125


#endif
