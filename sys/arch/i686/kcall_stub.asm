
global arch_invoke_kcall

bits 32

arch_invoke_kcall:
  jmp usr_sysenter_stub


usr_sysenter_stub:
  push ebp
  push edi

  mov ebp, esp
  mov edi, usr_sysenter_stub_ret

  sysenter
usr_sysenter_stub_ret: 
  nop

  pop edi
  pop ebp


  ret

