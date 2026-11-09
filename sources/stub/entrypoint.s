
%if __BITS__ = 64
BITS 64

section .text
global _stub_start

_stub_start:
	push rsi
	push rdi
	push rax

	mov rax, 1
	mov rdi, 0x41
	mov rsi, 1

	syscall

	%ifdef decryption
		; Do decryption things here
	%endif
	%ifdef decomp
		; Do decompression things here
	%endif

	pop rax
	pop rdi
	pop rsi
	; Note : Dont forget to correctly restore the stack
	db 0xE9
	dd 0x ; jump to the original _start location, patched during the build time
%endif
