PROJECT = os.elf

ARCH = i686

include arch/$(ARCH)/config

DIRS := ./kern ./kern/mem ./arch/$(ARCH) ./arch/$(ARCH)/boot ./arch/$(ARCH)/drivers ./lib/stdlib ./lib/libelf
FILES := $(foreach dir,$(DIRS),$(wildcard $(dir)/*.c $(dir)/*.s $(dir)/*.ld.pre))

C_FILES := $(filter %.c ,$(FILES))
C_OBJS := $(C_FILES:.c=.o)

ASM_FILES := $(filter %.s ,$(FILES))
ASM_OBJS := $(ASM_FILES:.s=.o)

LD_FILES := $(filter %.ld.pre ,$(FILES) )
PROCESSED_LD_FILES := $(LD_FILES:.ld.pre=.ld)


INCLUDEDIR = ./include
CFLAGS += -I $(INCLUDEDIR) -I arch/$(ARCH)/include -I ./
CFLAGS += -Wall -Werror -Wextra -Wno-unused-parameter


ISODIR = ./iso
ISONAME = os.iso
MKRESCUEFLAGS = -d /usr/lib/grub/i386-pc

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o : %.s
	$(ASM) $(ASMFLAGS) $< -o $@

%.ld : %.ld.pre
	$(CC) $(CFLAGS) -E -x c $< | grep -v "^#" > $@

all: LDFLAGS +=  -T $(LDSCRIPT) 
all: $(PROCESSED_LD_FILES) $(ASM_OBJS) $(C_OBJS)
	$(CC) $(CFLAGS)  $(LDFLAGS) -O2 $(ASM_OBJS) $(C_OBJS) -o $(PROJECT)


debug: CFLAGS += -g -DDEBUG -O0
debug: LDSCRIPT = ./arch/$(ARCH)/link.ld.debug
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
	$(QEMU) -m 256M -no-reboot -cdrom $(ISONAME)

qemu-debug: debug iso-debug
	@echo "### STARTING QEMU ###"
	$(QEMU) -m 256M -d int -no-shutdown -no-reboot -cdrom $(ISONAME)

qemu-gdb: debug iso-debug
	@echo "### STARTING QEMU ###"
	$(QEMU) -m 256M -d int -no-shutdown -s -S -no-reboot -cdrom $(ISONAME)


gdb: debug
	cgdb -s $(PROJECT) -ex 'target remote localhost:1234'


clean:
	- rm -f $(PROJECT) $(ISONAME) $(ISODIR)/boot/$(PROJECT) $(C_OBJS) $(ASM_OBJS)
