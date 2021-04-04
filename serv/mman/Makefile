OUTPUT = mman.elf

FILES := main.c

C_FILES := $(filter %.c ,$(FILES))
C_OBJS := $(C_FILES:.c=.o)

ASM_FILES := $(filter %.s ,$(FILES))
ASM_OBJS := $(ASM_FILES:.s=.o)

LDSCRIPT = ./arch/$(ARCH)/link.ld


%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o : %.s
	$(NASM) $(NASMFLAGS) $< -o $@


all: LDFLAGS +=  -T $(LDSCRIPT)
all: $(PROCESSED_LD_FILES) $(ASM_OBJS) $(C_OBJS)
	ld -n $(ASM_OBJS) $(C_OBJS) -o $(PROJECT) $(LDFLAGS)


debug: CFLAGS += -g -DDEBUG -O0
debug: LDSCRIPT = ./arch/$(ARCH)/link.ld.debug
debug: NASMFLAGS+= -g -O0
debug: all

clean:
	- rm -f $(OUTPUT) $(C_OBJS) $(ASM_OBJS)
