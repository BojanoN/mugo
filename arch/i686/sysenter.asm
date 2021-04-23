  global arch_kcall_entry
  extern kcall_handler
  bits 32

caller_esp:
  dd 0
caller_eip:
  dd 0

arch_kcall_entry:

  push caller_esp
  push caller_eip

  call kcall_handler

  ;; set EIP and ESP, can differ from the original
  mov ecx, [dword caller_esp] 
  mov edx, [dword caller_eip]


  sysexit


section .usr.sysenter

global 	arch_usr_ipc_trampoline

arch_usr_ipc_trampoline:
  push edx
  push ecx
  mov ebp, esp

  sysenter

