  global arch_syscall_entry
  extern kcall_handler
  bits 32

ret_esp:
  dd 0
ret_eip:
  dd 0

arch_syscall_entry:

  call kcall_handler

  ;; set EIP and ESP, can differ from the original
  mov ecx, [dword ret_esp]
  mov edx, [dword ret_eip]

  sysexit


section .usr.sysenter

global 	arch_usr_ipc_trampoline

arch_usr_ipc_trampoline:
  push edx
  push ecx
  mov ebp, esp

  sysenter

