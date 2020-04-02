; **************************************
; * WAIT4RETRACE                       *
; * inputs:  none                      *
; * returns: none                      *
; **************************************

wait4retrace:
	push ax dx
	mov dx, 03dah
_wrj1:
	in al, dx
	test al, 8
	jnz _wrj1
_wrj2:
	in al, dx
	test al, 8
	jz _wrj2
	pop dx ax
	ret

; **************************************
; * SETPALBLOCK                        *
; * inputs:  DS:SI=pal AL=scol BX=num  *
; * returns: none                      *
; **************************************

setpalblock:
	push cx dx

;	call wait4retrace
	mov cx, bx
	shl cx, 1
	add cx, bx
	mov dx, 3c8h
	out dx, al
	inc dx
	rep outsb

	pop dx cx
	ret

setpalblock_i	proc
	push cx dx

;	call wait4retrace
	mov cx, bx
	shl cx, 1
	add cx, bx
	mov dx, 3c8h
	out dx, al
	inc dx
	push	cx si
@@l1:
	xor	byte ptr [si], 63
	inc	si
	loop	@@l1
	pop	si cx
	push	cx si
	rep outsb
	pop	si cx
@@l2:
	xor	byte ptr [si], 63
	inc	si
	loop	@@l2

	pop dx cx
	ret
setpalblock_i	endp

; **************************************
_fadepal1to2:
	mov cx, 64
_fppp:
	mov ah, 64
	sub ah, cl
	call wait4retrace
	call fadepal1to2
	loop _fppp
	ret

fadepal1to2:
	pusha

	mov cx, bx
	shl cx, 1
	add cx, bx

	push cs
	pop es

	mov _fp12p, bx
	call makepal1to2
	mov si, offset _temppal
	db 0bbh
_fp12p	dw 0
	call setpalblock
;	pop di
;	pop si
	popa

	ret

makepal1to2:
	push si di
	mov dx, 03f00h
	mov dl, ah
_fp12j2:
	mov bx, offset _temppal
;	push si
;	push di
	push ax cx
_fp12l1:
	mov al, [si]
	mov ah, [di]
	xchg al, ah
	sub al, ah
	imul dl
	idiv dh
	xchg al, ah
	lodsb
	add ah, al
	mov [bx], ah
	inc bx
	inc di
	loop _fp12l1
	pop cx ax
	pop di si
	ret

_temppal	db 768 DUP (0)