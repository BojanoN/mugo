PROJECT = os.elf

ARCH = amd64

DIRS := ./kern ./arch/$(ARCH)/boot ./arch/$(ARCH)/drivers ./lib
FILES := $(foreach dir,$(DIRS),$(wildcard $(dir)/*.c $(dir)/*.s))

C_FILES := $(filter %.c ,$(FILES))
C_OBJS := $(C_FILES:.c=.o)

ASM_FILES := $(filter %.s ,$(FILES))
ASM_OBJS := $(ASM_FILES:.s=.o)

LDSCRIPT = ./arch/$(ARCH)/link.ld

INCLUDEDIR = ./include

NASM = nasm
NASMFLAGS = -felf64

CC = gcc
CFLAGS = -I $(INCLUDEDIR) -m64 -march=x86-64 -Wall -Werror -ffreestanding -nostdlib -fno-stack-protector -fno-pie -mcmodel=large -mno-red-zone -mno-mmx -mno-sse -mno-sse2
LDFLAGS =  -T $(LDSCRIPT)

ISODIR = ./iso
ISONAME = os.iso

MKRESCUEFLAGS = -d /usr/lib/grub/i386-pc

%.o : %.c
	$(CC) -c $< $(CFLAGS) -o $@

%.o : %.s
	$(NASM) $(NASMFLAGS) $< -o $@

all: $(ASM_OBJS) $(C_OBJS)
	ld -n $(ASM_OBJS) $(C_OBJS) -o $(PROJECT) $(LDFLAGS)

qemu: all
	@cp $(PROJECT) $(ISODIR)/boot
	@echo "### BUILDING BOOTABLE ISO ###"
	grub-mkrescue $(MKRESCUEFLAGS) -o $(ISONAME) $(ISODIR)
	@echo "### STARTING QEMU ###"
	qemu-system-x86_64 -no-reboot -cdrom $(ISONAME)

clean:
	- rm -f $(PROJECT) $(ISONAME) $(ISODIR)/boot/$(PROJECT) $(C_OBJS) $(ASM_OBJS)
