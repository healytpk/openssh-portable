section .text

    global pre_start:function
    pre_start:
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
	mov rsi, 0x3d3d3d3d3d3d3d3d  ;========
	push rsi
	mov rsi, rsp
	mov dl, 8                    ; Always print 8 bytes at a time
	syscall
	mov al, 1
	add di, 1
	syscall
	mov al, 1
	add di, 1
	syscall
	pop rsi
	
	;clear out the registers we are going to need
	xor rax, rax
	xor rsi, rsi
        xor rdi, rdi
	xor rdx, rdx

	;write(int fd, char *msg, unsigned int len)
	mov al, 1
	add di, 1
	mov rsi, 0x6174735f65727020  ; pre_sta
	push rsi
	mov rsi, rsp
	mov dl, 8
	syscall
	pop rsi

	;clear out the registers we are going to need
	xor rax, rax
	xor rsi, rsi
        xor rdi, rdi
	xor rdx, rdx

	;write(int fd, char *msg, unsigned int len)
	mov al, 1
	add di, 1
	mov rsi, 0x3d3d3d3d3d207472  ;rt =====
	push rsi	
	mov rsi, rsp
	mov dl, 8
	syscall
        pop rsi

	;clear out the registers we are going to need
	xor rax, rax
	xor rsi, rsi
        xor rdi, rdi
	xor rdx, rdx

	;write(int fd, char *msg, unsigned int len)
	mov al, 1
	add di, 1
	mov rsi, 0x3d3d3d3d3d3d3d3d  ;========
	push rsi
	mov rsi, rsp
	mov dl, 8                    ; Always print 8 bytes at a time
	syscall
	mov al, 1
	add di, 1
	syscall
	mov al, 1
	add di, 1
	syscall
	pop rsi

	;clear out the registers we are going to need
	xor rax, rax
	xor rsi, rsi
        xor rdi, rdi
	xor rdx, rdx

	;write(int fd, char *msg, unsigned int len)
	mov al, 1
	add di, 1
	mov rsi, 0xa  ; new line
	push rsi	
	mov rsi, rsp
	mov dl, 1
	syscall
	pop rsi

	pop rdx
	pop rdi
	pop rsi
	pop rax

	extern _start
	jmp _start

	;exit(int ret)
	;mov al, 60
	;xor rdi, rdi
	;syscall
