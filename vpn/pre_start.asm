; 2023-03-15 by T. P. K. Healy
; This file contains x86_64 assembler for NASM, also known as x64.
; This file contains two functions:
;     static void print8bytes(uint64_t eight_chars,uint64_t new_line);
;     extern void pre_start(int argc);

;order of registers for a syscall: rdi, rsi, rdx, r10, r8, r9

%define count_regs 14

%macro backup_all_registers 0
	push rax
	push rbx
	push rcx
	push rdx
	push rsi
	push rdi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
%endmacro

%macro restore_all_registers 0
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rdi
	pop rsi
	pop rdx
	pop rcx
	pop rbx
	pop rax
%endmacro

extern _start
extern load_libs

section .text

	dlopen_libc:
       call load_libs
       ret

    print8bytes:    ; This is a function that returns void
       ; Two parameters:
       ;               r8: If true, prints a trailing new line
       ;     Top of stack: The 8-byte string to print
       enter 0, 0

       ; save all the register values we're going to use
       push rdi
       push rax
       push rsi
       push rdx

       ;write(int fd, char *msg, unsigned int len)
       mov     rax, 1              ; system call 1 is write
       mov     rdi, 1              ; file handle 1 is stdout
       mov     rsi, rbp            ; Address of string = top of stack + 16
       add     rsi, 16
       mov     rdx, 8              ; number of bytes
       syscall

       cmp     r8, 1               ; check if rdi is true or false
       jl no_new_line
       ;write(int fd, char *msg, unsigned int len)
       mov     rax, 1              ; system call 1 is write
       mov     rdi, 1              ; file handle 1 is stdout
       push    0x0a
       mov     rsi, rsp            ; Address of string = top of stack
       mov     rdx, 1              ; number of bytes
       syscall
       add rsp,8                   ; Pop item off top of stack and discard
    no_new_line:                   ; just a jump label - not a function name
       pop rdx
       pop rsi
       pop rax
       pop rdi
       leave
       ret

    global pre_start:function
    pre_start:
		endbr64
		backup_all_registers

       ; The 'argc' argument to 'main' is on the top of the stack so
       ; we will use the frame pointer 'rbp' to keep track of it.
       enter 0,0

       call dlopen_libc

       mov   r8, 0                  ; false = don't put trailing new line
       ; We can't push a 64-Bit constant onto the stack, so we load
       ; the 64-Bit constant into a register, and then push the reg
       mov  rdi, 0x3d3d3d3d3d3d3d3d ; "========"
       push rdi
       call print8bytes
       call print8bytes
       call print8bytes
       add rsp,8                    ; Pop item off top of stack and discard

       mov rdi, 0x6174735f65727020  ; " pre_sta"
       push rdi
       call print8bytes
       add rsp,8                    ; Pop item off top of stack and discard

       cmp qword[rbp+8*(count_regs+1)], 2 ; check if argc < 2
       jl gui_only                 ; if argc < 2 then we want GUI mode
       mov rdi, 0x646d63202d207472 ; "rt - cmd"
       push rdi
       call print8bytes
       mov rdi, qword[rbp+8*(count_regs+1)]
       add rdi, 48
       push rdi
       call print8bytes
       add  rsp, 8
       jmp both                    ; skip the next 10-byte instruction
     gui_only:
       mov rdi, 0x495547202d207472  ; "rt - GUI"
       push rdi
       call print8bytes
       ;call Load_GUI_Libraries  -  no needed here so moved to main
     both:
       add rsp,8                   ; Pop item off top of stack and discard
       mov rdi, 0x3d3d3d3d3d3d3d3d ; "========"
       push rdi
       call print8bytes
       call print8bytes
       mov   r8, 1                 ; true = put trailing new line
       call print8bytes
       add rsp,8                   ; Pop item off top of stack and discard

       leave
       restore_all_registers
       jmp _start
