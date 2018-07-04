COMPILER_PATH = /workshop/jz/toolchain/mipseltools-nopic/bin
COMPILE_PREFIX = mipsel-linux-
CURT_IMAGE = curt_image

CC = $(COMPILE_PREFIX)gcc
LD = $(COMPILE_PREFIX)ld
OBJCOPY = $(COMPILE_PREFIX)objcopy
NM = $(COMPILE_PREFIX)nm
OBJDUMP	= $(COMPILE_PREFIX)objdump

CFLAGS = \
	-I../../includes/arch/mips/jz4740 \
	-I../../includes \
	-Wall \
	-mips32 -O2 -mno-abicalls -fno-pic -fno-builtin \
	-fno-exceptions -ffunction-sections \
	-fomit-frame-pointer -msoft-float -G 0 -g

LDSCRIPT := link.xn
LDFLAGS = \
	-nostdlib \
	-T $(LDSCRIPT)

OBJCOPYFLAGS = \
	-O binary

SHELL_SRC = ./main.c

HW_DEP_ASM_SRC = \
	../../arch/mips/jz4740/start.S \
	../../arch/mips/jz4740/mips_port.S


HW_DEP_C_SRC = \
	../../lib/stdio.c \
	../../arch/mips/jz4740/port.c \
	../../arch/mips/jz4740/cache.c \
	../../arch/mips/jz4740/clock.c \
	../../arch/mips/jz4740/exception.c \
	../../arch/mips/jz4740/gpio.c \
	../../arch/mips/jz4740/interrupt.c \
	../../arch/mips/jz4740/jz_serial.c \
	../../arch/mips/jz4740/timer.c

KERNEL_SRC = \
	../../kernel/kernel.c \
	../../kernel/thread.c \
	../../kernel/list.c   \
	../../kernel/sync.c  \
	../../kernel/ipc.c

SHELL_OBJ = $(SHELL_SRC:.c=.o)
HW_DEP_C_OBJ = $(HW_DEP_C_SRC:.c=.o)
HW_DEP_ASM_OBJ = $(HW_DEP_ASM_SRC:.S=.o)
KERNEL_OBJ = $(KERNEL_SRC:.c=.o)

$(CURT_IMAGE).bin: $(CURT_IMAGE).elf
	$(OBJCOPY) $(OBJCOPYFLAGS) $< $@
	$(OBJDUMP) -d $< > $(CURT_IMAGE).dump
	$(NM) $< | sort > $(CURT_IMAGE).sym
	$(OBJDUMP) -h $< > $(CURT_IMAGE).map
	cat ../../nandboot/spl-pad.bin curt_image.bin > curt.bin

$(CURT_IMAGE).elf: $(SHELL_OBJ) $(HW_DEP_C_OBJ) $(HW_DEP_ASM_OBJ) $(KERNEL_OBJ)
	$(LD) $(LDFLAGS) \
		-o $(CURT_IMAGE).elf \
		$(SHELL_OBJ) $(HW_DEP_C_OBJ) $(HW_DEP_ASM_OBJ) $(KERNEL_OBJ)

$(SHELL_OBJ) : %.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@

$(HW_DEP_C_OBJ) : %.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@

$(HW_DEP_ASM_OBJ) : %.o : %.S
	$(CC) -c $(CFLAGS) -D_ASSEMBLER_ -D__ASSEMBLY__ $< -o $@

$(KERNEL_OBJ) : %.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@


clean:
	rm -f ../../arch/mips/jz4740/*.o \
	      ../../kernel/*.o \
	      ../../device/*.o \
	      ../../lib/*.o \
	      ./*.o \
	      $(CURT_IMAGE).bin $(CURT_IMAGE).elf \
