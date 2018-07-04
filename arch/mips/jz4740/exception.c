#include <mips.h>
#include <mipsregs.h>

extern void except_common_entry(void);

const static char *regstr[] = {
	"zero", "at", "v0", "v1", "a0", "a1", "a2", "a3",
	"t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
	"s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
	"t8", "t9", "k0", "k1", "gp", "sp", "fp", "ra"
};

void exception_handler(unsigned int *sp)
{
	int i;

	printf("CAUSE = %08x EPC = %08x\n", read_c0_cause(), read_c0_epc());

	for (i = 0; i < 32; i++) {
		if ((i % 4) == 0) {
			printf("\n");
		}
		printf("%4s %08x ", regstr[i], sp[i]);
	}

	printf("\n");

	while(1);
}

void exception_init(void)
{
    write_c0_status(0x10000400);

    memcpy((void *)(A_K0BASE        ), except_common_entry, 0x20);
    memcpy((void *)(A_K0BASE + 0x180), except_common_entry, 0x20);
    memcpy((void *)(A_K0BASE + 0x200), except_common_entry, 0x20);

    __dcache_writeback_all();
    __icache_invalidate_all();
}
