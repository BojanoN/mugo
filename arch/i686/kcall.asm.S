
  #include "include/arch_conf.h"

  global arch_sysenter_stub
  extern kcall_handler, kstub_ret_vaddr, kcall_table, NO_KCALLS
  bits 32

  kstub_ret_vaddr equ CONF_SYSENTER_STUB_RET_VADDR

;; SYSENTER kernel entry stub
;  %eax kcall number
;  %ebx arg1
;  %ecx arg2
;  %edx arg3
;  %esi arg4
;  %edi arg5
;  %ebp user stack
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
  ;; pop the ebp we saved before
  pop ecx
  mov edx, kstub_ret_vaddr

  sysexit
