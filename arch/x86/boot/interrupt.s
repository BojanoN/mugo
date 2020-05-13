%macro IRH 1  ; define a macro, taking one parameter
	[GLOBAL irh%1]        ; %1 accesses the first parameter.
irh%1:
	cli
	push byte 0
	push byte %1
	jmp irh_common
%endmacro

%macro IRH_ERRCODE 1
	[GLOBAL irh%1]
irh%1:
	cli
	push byte %1
	jmp irh_common
%endmacro

extern interrupt_handler

irh_common:
	   pusha                    ; Save context

	   mov ax, ds
	   push eax                 ; save the data segment descriptor

	   mov ax, 0x10               ; Load kernel segment descriptors
	   mov ds, ax
	   mov es, ax
	   mov fs, ax
	   mov gs, ax

	   call interrupt_handler

	   pop eax        ; reload the original data segment descriptor
	   mov ds, ax
	   mov es, ax
	   mov fs, ax
	   mov gs, ax

	   popa                     ; Pops edi,esi,ebp...
	   add esp, 8     ; Cleans up the pushed error code and pushed ISR number
	   sti
	   iret


	IRH 0
	IRH 1
	IRH 2
	IRH 3
	IRH 4
	IRH 5
	IRH 6
	IRH 7
	IRH_ERRCODE 8
	IRH 9
	IRH_ERRCODE 10
	IRH_ERRCODE 11
	IRH_ERRCODE 12
	IRH_ERRCODE 13
	IRH_ERRCODE 14
	IRH 15
	IRH 16
	IRH 17
	IRH 18
	IRH 19
	IRH 20
	IRH 21
	IRH 22
	IRH 23
	IRH 24
	IRH 25
	IRH 26
	IRH 27
	IRH 28
	IRH 29
	IRH 30
	IRH 31
