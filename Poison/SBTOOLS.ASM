SBport  dw 0
SBIRQ	db 0
SBDMA	db 0

DMAtbl  db 00, 83h, 81h, 82h
intmask db 0
oldIRQ3	dw 0, 0
oldIRQ5	dw 0, 0
oldIRQ7 dw 0, 0

getSBport:
	mov bx, 210h
	mov cx, 6
_again:
	push cx
	mov dx, bx
	add dx, 6h
	mov al, 1
	out dx, al
	mov cx, 0
_loop:
	loop _loop
	mov al, 0
	out dx, al
	add dx, 8
jump1:
	in al, dx
	rol al, 1
	jnc jump1
	sub dx, 4
	in al, dx
	pop cx
	cmp al, 0aah
	je _end2
	add bx, 16
	loop _again
	mov bx, 0
_end2:	mov SBport, bx
	ret
comment #
; ********************* GET IRQ & DMA ************************
getIRQ&DMA:
	mov ax, 350bh		; IRQ3	
	int 21h
	mov oldIRQ3, es
	mov oldIRQ3 + 2, bx
	mov ax, 350dh		; IRQ5
	int 21h
	mov oldIRQ5, es
	mov oldIRQ5 + 2, bx
	mov ax, 350fh		; IRQ7
	int 21h
	mov oldIRQ7, es
	mov oldIRQ7 + 2, bx

	mov ax, 250bh
	mov dx, offset newIRQ3
	int 21h
	mov ax, 250dh
	mov dx, offset newIRQ5
	int 21h
	mov ax, 250fh
	mov dx, offset newIRQ7
	int 21h

	in al, 21h
	mov intmask, al
	jmp $ + 2
	and al, 01010111b
	out 21h, al

	mov bx, 0
jmp2:
	inc bl
	call setDMA
	mov cx, 0
dmaloop:loop dmaloop
	cmp SBIRQ, 0
	jnz jmp1
	cmp bl, 3
	jnz jmp2
jmp1:	mov SBDMA, bl
	mov al, intmask
	out 21h, al

	mov ax, [oldIRQ3]
	mov dx, [oldIRQ3 + 2]
	push ds
	mov ds, ax
	mov ax, 250bh
	int 21h
	pop ds

	mov ax, [oldIRQ5]
	mov dx, [oldIRQ5 + 2]
	push ds
	mov ds, ax
	mov ax, 250dh
	int 21h
	pop ds

	mov ax, [oldIRQ7]
	mov dx, [oldIRQ7 + 2]
	push ds
	mov ds, ax
	mov ax, 250fh
	int 21h
	pop ds

	ret
; ************* SET DMA & FIND IRQ *******************
setDMA:
	mov al, 4
	or al, bl
	out 0ah, al

	mov al, 0
	out 0ch, al

	mov al, 58h
	or al, bl
	out 0bh, al

	xor al, al
	mov dx, 0
	add dl, bl
	shl dl, 1
	out dx, al
	out dx, al

	mov si, offset DMAtbl
	mov dl, [DMAtbl +  bx]
	xor al, al
	out dx, al

	mov dl, bl
	shl dl, 1
	inc dl
	mov al, 0h
	out dx, al
	mov al, 0h
	out dx, al

	mov al, bl
	out 0ah, al

	mov dx, SBport
	add dl, 0ch
	mov al, 40h
	out dx, al
	call wait4sb
	mov al, 0d5h
	out dx, al
	call wait4sb

	mov al, 14h
	out dx, al
	call wait4sb
	mov al, 0h
	out dx, al
	call wait4sb
	mov al, 0h
	out dx, al

	ret
; ******************** NEW IRQ'S *********************
newIRQ3:
	mov SBIRQ, 3
	call endofIRQ
	iret
newIRQ5:
	mov SBIRQ, 5
	call endofIRQ
	iret
newIRQ7:
	mov SBIRQ, 7
	call endofIRQ
	iret
endofIRQ:
	push ax
	push dx
	mov dx, SBport
	add dl, 0eh
	in al, dx
	sub dx, 2
_end:
	in al, dx
	or al, al
	js _end
	mov al, 20h
	out 20h, al
	pop dx
	pop ax
	ret
; ************************************************
wait4sb:
	push ax
	push cx
	push dx
	mov dx, SBport
	xor cx, cx
	add dl, 0ch
_wait:
	in al, dx
	inc cx
	cmp cx, 0
;	nop
	jz endwait
	or al, al
	js _wait
endwait:
	pop dx
	pop cx
	pop ax
	ret
#