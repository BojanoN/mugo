PROJECT = os.elf

ARCH = x86

include arch/$(ARCH)/config

DIRS := ./kern ./arch/$(ARCH) ./arch/$(ARCH)/boot ./arch/$(ARCH)/drivers ./lib
FILES := $(foreach dir,$(DIRS),$(wildcard $(dir)/*.c $(dir)/*.s $(dir)/*.ld.pre))

C_FILES := $(filter %.c ,$(FILES))
C_OBJS := $(C_FILES:.c=.o)

ASM_FILES := $(filter %.s ,$(FILES))
ASM_OBJS := $(ASM_FILES:.s=.o)

LD_FILES := $(filter %.ld.pre ,$(FILES))
PROCESSED_LD_FILES := $(LD_FILES:.ld.pre=.ld)

# Compilation settings
NASM = nasm

CC = gcc
INCLUDEDIR = ./include
CFLAGS += -I $(INCLUDEDIR) -I arch/$(ARCH)/include -I ./

LDSCRIPT = ./arch/$(ARCH)/link.ld
LDFLAGS +=  -T $(LDSCRIPT)

ISODIR = ./iso
ISONAME = os.iso
MKRESCUEFLAGS = -d /usr/lib/grub/i386-pc

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o : %.s
	$(NASM) $(NASMFLAGS) $< -o $@

%.ld : %.ld.pre
	$(CC) $(CFLAGS) -E -x c $< | grep -v "^#" > $@


all: $(PROCESSED_LD_FILES) $(ASM_OBJS) $(C_OBJS)
	ld -n $(ASM_OBJS) $(C_OBJS) -o $(PROJECT) $(LDFLAGS)

debug: CFLAGS += -g -DDEBUG -O0
debug: NASMFLAGS+= -g -O0
debug: all

iso: all
	@cp $(PROJECT) $(ISODIR)/boot
	@echo "### BUILDING BOOTABLE ISO ###"
	grub-mkrescue $(MKRESCUEFLAGS) -o $(ISONAME) $(ISODIR)

iso-debug: debug
	@cp $(PROJECT) $(ISODIR)/boot
	@echo "### BUILDING BOOTABLE ISO ###"
	grub-mkrescue $(MKRESCUEFLAGS) -o $(ISONAME) $(ISODIR)

qemu: all iso
	@echo "### STARTING QEMU ###"
	$(QEMU) -no-reboot -cdrom $(ISONAME)

qemu-debug: debug iso-debug
	@echo "### STARTING QEMU ###"
	$(QEMU) -s -S -no-reboot -cdrom $(ISONAME)

gdb: debug
	cgdb -s $(PROJECT) -ex 'target remote localhost:1234'


clean:
	- rm -f $(PROJECT) $(ISONAME) $(ISODIR)/boot/$(PROJECT) $(C_OBJS) $(ASM_OBJS)
