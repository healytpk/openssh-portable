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
       ;     r9: The 8-byte string to print
       ;     r8: If true, prints a trailing new line

       ; save all the register values we're going to use
       push rax
       push rsi
       push rdi
       push rdx

       ;zero out the registers we are going to need
       xor rax, rax
       xor rsi, rsi
       xor rdi, rdi
       xor rdx, rdx

       ;write(int fd, char *msg, unsigned int len)
       mov al, 1
       add di, 1
       mov rsi, r9
       push rsi
       mov rsi, rsp
       mov dl, 8     ; Print 8 bytes at a time
       syscall
       add rsp,8     ; Pop item off top of stack and discard

       cmp r8, 1     ; check if r8 is true or false
       jl no_new_line
       ;zero out the registers we are going to need
       xor rax, rax
       xor rsi, rsi
       xor rdi, rdi
       xor rdx, rdx
       ;write(int fd, char *msg, unsigned int len)
       mov al, 1
       add di, 1
       mov rsi, 0x000000000000000a  ; new line
       push rsi
       mov rsi, rsp
       mov dl, 1     ; Print just one byte
       syscall
       add rsp,8     ; Pop item off top of stack and discard
    no_new_line:     ; just a jump label - not a function name
       pop rdx
       pop rdi
       pop rsi
       pop rax
       ret

    global pre_start:function
    pre_start:
		endbr64

       ; The 'argc' argument to 'main' is on the top of the stack so
       ; we will use the frame pointer 'rbp' to keep track of it.
       push rbp
       mov rbp, rsp

       call dlopen_libc

       push r9    ; save because we'll use it - pop it back later
       push r8    ; save because we'll use it - pop it back later

       mov r8, 0                   ; false = don't put trailing new line
       mov r9, 0x3d3d3d3d3d3d3d3d  ; "========"
       call print8bytes
       call print8bytes
       call print8bytes

       mov r9, 0x6174735f65727020  ; " pre_sta"
       call print8bytes

       cmp qword[rbp+8], 2         ; check if argc < 2
       jl gui_only                 ; if argc < 2 then we want GUI mode
       mov r9, 0x646d63202d207472  ; "rt - cmd"
       call print8bytes
       jmp both                    ; skip the next 10-byte instruction
     gui_only:
       mov r9, 0x495547202d207472  ; "rt - GUI"
       call print8bytes
       ;call Load_GUI_Libraries  -  no needed here so moved to main
     both:
       mov r9, 0x3d3d3d3d3d3d3d3d  ; "========"
       call print8bytes
       call print8bytes
       mov r8, 1                   ; true = put trailing new line
       call print8bytes

       pop r8
       pop r9

       mov rsp, rbp
       pop rbp

       jmp _start
