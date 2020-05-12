PROJECT = os.elf

ARCH = x86

include arch/$(ARCH)/config

DIRS := ./kern ./arch/$(ARCH)/boot ./arch/$(ARCH)/drivers ./lib
FILES := $(foreach dir,$(DIRS),$(wildcard $(dir)/*.c $(dir)/*.s))

C_FILES := $(filter %.c ,$(FILES))
C_OBJS := $(C_FILES:.c=.o)

ASM_FILES := $(filter %.s ,$(FILES))
ASM_OBJS := $(ASM_FILES:.s=.o)

# Compilation settings
NASM = nasm

CC = clang
INCLUDEDIR = ./include
CFLAGS += -I $(INCLUDEDIR)

LDSCRIPT = ./arch/$(ARCH)/link.ld
LDFLAGS +=  -T $(LDSCRIPT)

ISODIR = ./iso
ISONAME = os.iso
MKRESCUEFLAGS = -d /usr/lib/grub/i386-pc

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o : %.s
	$(NASM) $(NASMFLAGS) $< -o $@

all: $(ASM_OBJS) $(C_OBJS)
	ld -n $(ASM_OBJS) $(C_OBJS) -o $(PROJECT) $(LDFLAGS)

qemu: all
	@cp $(PROJECT) $(ISODIR)/boot
	@echo "### BUILDING BOOTABLE ISO ###"
	grub-mkrescue $(MKRESCUEFLAGS) -o $(ISONAME) $(ISODIR)
	@echo "### STARTING QEMU ###"
	$(QEMU) -no-reboot -cdrom $(ISONAME)

clean:
	- rm -f $(PROJECT) $(ISONAME) $(ISODIR)/boot/$(PROJECT) $(C_OBJS) $(ASM_OBJS)
