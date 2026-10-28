
%if __BITS__ = 64
BITS 64

section .text
global _start

_start:
	%ifdef decryption
		; Do decryption things here
	%endif
	%ifdef decomp
		; Do decompression things here
	%endif

	; Note : Dont forget to correctly restore the stack
	call _start
%endif
