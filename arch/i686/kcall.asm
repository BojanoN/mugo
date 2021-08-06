# 1 "/home/bojan/Documents/projects/mugo/arch/i686/kcall.asm.S"
# 1 "<built-in>" 1
# 1 "/home/bojan/Documents/projects/mugo/arch/i686/kcall.asm.S" 2

# 1 "/home/bojan/Documents/projects/mugo/arch/i686/include/arch_conf.h" 1
# 3 "/home/bojan/Documents/projects/mugo/arch/i686/kcall.asm.S" 2

  global arch_sysenter_stub

  extern kcall_handler, kcall_table, NO_KCALLS
  bits 32

;; SYSENTER kernel entry stub
; %eax kcall number
; %ebx arg1
; %ecx arg2
; %edx arg3
; %esi arg4
; %edi return eip
; %ebp user stack
;; note: sysenter masks interrupts by default
arch_sysenter_stub:
  push ebp
  push edi

  push esi
  push edx
  push ecx
  push ebx
  push eax

  push esp

  call kcall_handler



.ret:
  ;; set return EIP and ESP
  ;; pop the esp we saved before
  add esp, 0x18
  pop edx
  pop ecx

  sti
  sysexit
