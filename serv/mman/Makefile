OUTPUT = mman.elf

FILES := main.c

C_FILES := $(filter %.c ,$(FILES))
C_OBJS := $(C_FILES:.c=.o)

ASM_FILES := $(filter %.s ,$(FILES))
ASM_OBJS := $(ASM_FILES:.s=.o)

LDSCRIPT := ./link.ld
LDFLAGS :=  -T $(LDSCRIPT)

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o : %.s
	$(ASM) $(ASMFLAGS) $< -o $@


all: $(ASM_OBJS) $(C_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -O2 $(ASM_OBJS) $(C_OBJS) -o $(OUTPUT) 


debug: CFLAGS += -g -DDEBUG -O0
debug: ASMFLAGS+= -g -O0
debug: all

clean:
	- rm -f $(OUTPUT) $(C_OBJS) $(ASM_OBJS)
