PROJECT=hello.elf

CFLAGS = -m64 -march=x86-64 -Wall -Werror -ffreestanding -nostdlib -fno-stack-protector -fno-pie -mcmodel=large -mno-red-zone -mno-mmx -mno-sse -mno-sse2
LDFLAGS =  -T link.ld
ISODIR = ./iso
ISONAME = os.iso
MKRESCUEFLAGS = -d /usr/lib/grub/i386-pc

all:
	gcc -c main.c $(CFLAGS)
	nasm -felf64 boot.s
	nasm -felf64 long_mode_init.s
	gcc -c multiboot.s $(CFLAGS)
	ld -n *.o -o $(PROJECT) $(LDFLAGS) 
qemu: all
	@cp $(PROJECT) $(ISODIR)/boot
	@echo "### BUILDING BOOTABLE ISO ###"
	grub-mkrescue $(MKRESCUEFLAGS) -o $(ISONAME) $(ISODIR)
	@echo "### STARTING QEMU ###"
	qemu-system-x86_64 -no-reboot -cdrom $(ISONAME)
clean:
	- rm -f *.o $(PROJECT) $(ISONAME) $(ISODIR)/boot/$(PROJECT)
