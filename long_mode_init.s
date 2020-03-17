extern kernel_start
global long_mode_init


	section .text
	bits 64
long_mode_init:
      call kernel_start
	    hlt
