; 2023-03-09 by T. P. K. Healy
; This file contains x86_64 assembler for NASM, also known as x64.
; This file contains two functions:
;     static void print8bytes(uint64_t eight_chars,uint64_t new_line);
;     extern void pre_start(int argc);

extern _start
extern load_libs

section .text

	dlopen_libc:
       call load_libs
       ret

    print8bytes:    ; This is a function that returns void
       ; Two parameters:
       ;              rdi: If true, prints a trailing new line
       ;     Top of stack: The 8-byte string to print
       enter 0, 0

       ; save all the register values we're going to use
       push rdi
       push rax
       push rsi
       push rdx

       ;write(int fd, char *msg, unsigned int len)
       mov     rax, 1              ; system call 1 is write
       ;order of registers for a syscall: rdi, rsi, rdx, r10, r8, r9
       mov     rdi, 1              ; file handle 1 is stdout
       mov     rsi, rbp            ; Address of string = top of stack
       add     rsi, 16
       mov     rdx, 8              ; number of bytes
       syscall

       cmp qword [rbp-8], 1        ; check if rdi is true or false
       jl no_new_line

       ;write(int fd, char *msg, unsigned int len)
       ; write(1, message, 13)
       mov     rax, 1              ; system call 1 is write
       ;order of registers for a syscall: rdi, rsi, rdx, r10, r8, r9
       mov     rdi, 1              ; file handle 1 is stdout
       push 0x0a
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

       ; The 'argc' argument to 'main' is on the top of the stack so
       ; we will use the frame pointer 'rbp' to keep track of it.
       push rbp
       mov rbp, rsp

       call dlopen_libc

       push rdi

       mov rdi, 0x3d3d3d3d3d3d3d3d ; false = don't put trailing new line
       push rdi ; "========"
       mov rdi, 0                  ; false = don't put trailing new line
       call print8bytes
       call print8bytes
       call print8bytes
       add rsp,8                   ; Pop item off top of stack and discard

       mov rdi, 0x6174735f65727020 ; " pre_sta"
       push rdi
       mov rdi, 0                  ; false = don't put trailing new line
       call print8bytes
       add rsp,8                   ; Pop item off top of stack and discard

       cmp qword[rbp+8], 2         ; check if argc < 2
       jl gui_only                 ; if argc < 2 then we want GUI mode
       mov rdi, 0x646d63202d207472     ; "rt - cmd"
       push rdi
       mov rdi, 0                  ; false = don't put trailing new line
       call print8bytes
       jmp both                    ; skip the next 10-byte instruction
     gui_only:
       mov rdi, 0x495547202d207472  ; "rt - GUI"
       push rdi
       mov rdi, 0
       call print8bytes
       ;call Load_GUI_Libraries  -  no needed here so moved to main
     both:
       add rsp,8                   ; Pop item off top of stack and discard
       mov rdi, 0x3d3d3d3d3d3d3d3d ; "========"
       push rdi
       mov rdi, 0
       call print8bytes
       call print8bytes
       mov rdi, 1                  ; true = put trailing new line
       call print8bytes
       add rsp,8                   ; Pop item off top of stack and discard

       pop rdi

       mov rsp, rbp
       pop rbp

       jmp _start
