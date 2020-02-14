; **************************************
; * OPENXMODE                          *
; * inputs:  none                      *
; * returns: none                      *
; **************************************
openxmode:
	mov ax, 2101h
	mov dx, 3c4h
	out dx, ax
	mov ax, 13h
	int 10h
	mov dx, 3c4h
	mov ax, 604h
	out dx, ax
	mov ax, 0f02h
	out dx, ax
	mov dx, 0a000h
	mov es, dx
	xor di, di
	xor ax, ax
	mov cx, 8000h
	rep stosw
	mov dx, 3d4h
	mov ax, 14h
	out dx, ax
	mov ax, 0e317h
	out dx, ax
	cld		; !!!!!
	ret

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
; * SETSPLITSCREEN                     *
; * inputs:  AX = line                 *
; * returns: nono                      *
; **************************************

setsplitscreen:
	push ax bx dx
;	call wait4retrace

	mov dx, 03d4h
	mov bh, ah
	mov bl, bh
	and bx, 0201h
	mov cl, 4
	shl bx, cl
	shl bh, 1
	mov ah, al
	mov al, 18h
	out dx, ax
	mov al, 7
	out dx, al
	inc dx
	in al, dx
	dec dx
	mov ah, al
	and ah, 11101111b
	or ah, bl
	mov al, 7
	out dx, ax
	mov al, 9
	out dx, al
	inc dx
	in al, dx
	dec dx
	mov ah, al
	and ah, 10111111b
	or ah, bh
	mov al, 9
	out dx, ax
	pop dx bx ax
	ret

; **************************************
; * SETSTARTADRRESS                    *
; * inputs:  AX = offset               *
; * returns: none                      *
; **************************************

setstartaddress:
	push ax bx dx
;	call wait4retrace
	mov bx, ax
	mov dx, 03d4h
	mov al, 0ch
	mov ah, bh
	out dx, ax
	mov al, 0dh
	mov ah, bl
	out dx, ax
	pop dx bx ax
	ret

; **************************************
; * SETYZOOM                           *
; * inputs:  AL = yzoom                *
; * returns: none                      *
; **************************************

setyzoom:
	push dx
	mov dx, 03d4h
	mov ah, al
	mov al, 9
	out dx, ax
	pop dx
	ret

; **************************************
; * CLEARXBLOCK                        *
; * inputs:  DI = offset CX = len/8    *
; * returns: none                      *
; **************************************

;clearxblock:

;	push ax dx
;	mov ax, 0a000h
;	mov es, ax
;	mov dx, 3c4h
;	mov ax, 0f02h
;	out dx, ax
;	xor ax, ax
;	rep movsw
;	pop ax dx
;	ret

; **************************************
; * SETCOLOR                           *
; * inputs:  AL = 0 BX=G|R CL=B        *
; * returns: none                      *
; **************************************

;setcolor:
;	push dx
;	mov dx, 3c8h
;	out dx, al
;	inc dx
;	mov al, bl
;	out dx, al
;	mov al, bh
;	out dx, al
;	mov al, cl
;	out dx, al
;	pop dx
;	ret

; **************************************
; * SETPALBLOCK                        *
; * inputs:  DS:SI=pal AL=scol BX=num  *
; * returns: none                      *
; **************************************

setpalblock:
	push cx dx si

;	call wait4retrace
	mov cx, bx
	shl cx, 1
	add cx, bx
	mov dx, 3c8h
	out dx, al
	inc dx
	rep outsb

	pop si dx cx
	ret

; **************************************
; * SETBLACKPAL                        *
; * inputs:  none                      *
; * returns: none                      *
; **************************************

;setblackpal:
;	push ax cx dx

;	call wait4retrace
;	mov dx, 3c8h
;	xor al, al
;	out dx, al
;	inc dx
;	mov cx, 768
;	rep outsb

;	pop dx cx ax
;	ret

; **************************************
; * GETPALBLOCK                        *
; * inputs:  DS:SI=pal AL=scol BX=num  *
; * returns: none                      *
; **************************************

getpalblock:
;	push ax cx dx si

;	mov cx, bx
;	shl cx, 1
;	add cx, bx
;	mov dx, 3c7h
;	out dx, al
;	inc dx
;	inc dx
_gpbl1:
;	in al, dx
;	mov [si], al
;	inc si
;	loop _gpbl1

;	pop si dx cx ax
;	ret
; **************************************

_fadepal1to2:
	mov cx, 64
_fppp:
	mov ah, 64
	sub ah, cl
	call wait4retrace
	call fadepal1to2
;	call wait4retrace
;	call setpalblock
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
	mov dx, 03f00h
	mov dl, ah
_fp12j2:
	mov bx, offset _temppal2
	push si
	push di
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

	mov si, offset _temppal2
	db 0bbh
_fp12p	dw 0
;	call wait4retrace
	call setpalblock
	pop di
	pop si
	popa

	ret


_temppal	EQU 63464
_temppal2	EQU 64232