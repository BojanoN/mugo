 extern kstack_top, __current_thread_execution_ctx 

 global arch_return_to_user

;;  Exception handlers
%macro EH 1
	[GLOBAL eh%1]
eh%1:
  ;; 	cli
	push byte 0
	push byte %1
	jmp eh_common
%endmacro

%macro EH_ERRCODE 1
	[GLOBAL eh%1]
eh%1:
  ;; 	cli
	push byte %1
	jmp eh_common
%endmacro

extern exception_handler

eh_common:
	   pusha

	   mov ax, ds
	   push eax

	   mov ax, 0x10
	   mov ds, ax
	   mov es, ax
	   mov fs, ax
	   mov gs, ax

	   call exception_handler

	   pop eax
	   mov ds, ax
	   mov es, ax
	   mov fs, ax
	   mov gs, ax

	   popa
	   add esp, 8
	 
	   iret

;; Interrupt handlers
%macro IRQ 2
	[GLOBAL irq%1]
	irq%1:
  ;; 	cli
	push byte 0
	push byte %2
	jmp irq_common
	%endmacro

extern interrupt_handler

irq_common:
  ;;      mov esp, kstack_top
	   pusha

   	 mov ax, ds
	   push eax

	   mov ax, 0x10
	   mov ds, ax
	   mov es, ax
	   mov fs, ax
	   mov gs, ax

  	 call interrupt_handler

arch_return_to_user:
     mov esp, dword [__current_thread_execution_ctx]

	   pop eax
	   mov ds, ax
	   mov es, ax
	   mov fs, ax
	   mov gs, ax

	   popa
     add esp, 4

	   iret


	EH 0
	EH 1
	EH 2
	EH 3
	EH 4
	EH 5
	EH 6
	EH 7
	EH_ERRCODE 8
	EH 9
	EH_ERRCODE 10
	EH_ERRCODE 11
	EH_ERRCODE 12
	EH_ERRCODE 13
	EH_ERRCODE 14
	EH 15
	EH 16
	EH 17
	EH 18
	EH 19
	EH 20
	EH 21
	EH 22
	EH 23
	EH 24
	EH 25
	EH 26
	EH 27
	EH 28
	EH 29
	EH 30
	EH 31

	IRQ 0, 32
	IRQ 1, 33
	IRQ 2, 34
	IRQ 3, 35
	IRQ 4, 36
	IRQ 5, 37
	IRQ 6, 38
	IRQ 7, 39
	IRQ 8, 40
	IRQ 9, 41
	IRQ 10, 42
	IRQ 11, 43
	IRQ 12, 44
	IRQ 13, 45
	IRQ 14, 46
	IRQ 15, 47
