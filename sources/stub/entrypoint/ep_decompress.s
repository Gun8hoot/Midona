
%if __BITS__ = 64
BITS 64

section .text
global _start
extern __libc_start_main

_start:

	; ~ __libc_start_main(ptr_main,argc,argv,ptr_init_fn,ptr_fini_fn,rtld_fini,stack_end) ~
	; ptr_main = Pointer to the main program function // Calculate with the decompressed ELF file
	; argc = The number of argumnents
	; argv = The every runtime argumnents
	; ptr_init_fn = pointer to the addresse of the init function __attribute__((constructor)) // Calculate with the decompressed ELF file
	; ptr_init_fn = pointer to the addresse of the fini function __attribute__((destructor)) // Calculate with the decompressed ELF file
	; rtld_fini = pointer to the terminason function of the dynamic linker, 0 when static linked
	; stack_end = highest address of the stack, define the end of the stack
	mov rdi, [rsp + 0]			; Get argc
	mov rsi, [rsp + 8]			; Get argv
	call __libc_start_main
%endif

%if __BITS__ = 32
BITS 32

section .text
global _start
extern __libc_start_main

_start:

	; ~ __libc_start_main(ptr_main,argc,argv,ptr_init_fn,ptr_fini_fn,rtld_fini,stack_end) ~
	; ptr_main = Pointer to the main program function // Calculate with the decompressed ELF file
	; argc = The number of argumnents
	; argv = The every runtime argumnents // Calculate here
	; ptr_init_fn = pointer to the addresse of the init function __attribute__((constructor)) // Calculate with the decompressed ELF file
	; ptr_init_fn = pointer to the addresse of the fini function __attribute__((destructor)) // Calculate with the decompressed ELF file
	; rtld_fini = pointer to the terminason function of the dynamic linker, 0 when static linked
	; stack_end = highest address of the stack, define the end of the stack
	mov rdi, [esp + 0]			; Get argc
	mov rsi, [esp + 4]			; Get argv
	call __libc_start_main
%endif
