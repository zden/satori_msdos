line0	macro
	mov	ax, [si+2]
	mov	di, ax
	sal	di, 2
	add	di, ax
	sal	di, 6
	add	di, [si+0]

	xor	bp, bp

	cmp	word ptr [si+6], 200
	jng	@@l0jc1
	mov	word ptr [si+6], 200
@@l0jc1:
	cmp	cx, 0
	jge	@@l0jc3
	cmp	cx, [si+6]
	jge	@@end
	inc	cx
	add	di, 320
	add	di, bx
	add	[si+0], bx
	jmp	@@l0jc1

@@l0jc3:
	cmp	word ptr [si+0], 319
	jge	@@end
	cmp	word ptr [si+0], 1
	jge	@@l0j1
	cmp	cx, [si+6]
	jge	@@end
	inc	cx
	add	di, 320
	add	di, bx
	add	[si+0], bx
	jmp	@@l0jc3
@@l0j1:
	cmp	cx, [si+6]
	jg	@@end

	mov	al, 11
	mov	ah, 4
	add	es:[di], al
	cmp	byte ptr es:[di], 63
	jna	@@l0j2
	mov	byte ptr es:[di], 63
@@l0j2:
	add	es:[di+bx], ah
	cmp	byte ptr es:[di+bx], 63
	jna	@@l0j3
	mov	byte ptr es:[di+bx], 63
@@l0j3:
	add	di, 320
	add	di, bx
	add	[si+0], bx
	cmp	word ptr [si+0], 319
	jge	@@end
	cmp	word ptr [si+0], 1
	jle	@@end
@@l0j4:
	inc	cx
	jmp	@@l0j1

	jmp	@@end
endm

line1	macro
	mov	ax, [si+2]
	mov	di, ax
	sal	di, 2
	add	di, ax
	sal	di, 6
	add	di, [si+0]

	xor	bp, bp

	cmp	word ptr [si+6], 200
	jng	@@l1jc1
	mov	word ptr [si+6], 200
@@l1jc1:
	cmp	cx, 0
	jge	@@l1jc3
	cmp	cx, [si+6]
	jge	@@end
	inc	cx
	add	di, 320
	add	bp, dx
	jnc	@@l1jc1
	add	di, bx
	add	[si+0], bx
	jmp	@@l1jc1

@@l1jc3:
	cmp	word ptr [si+0], 319
	jge	@@end
	cmp	word ptr [si+0], 1
	jge	@@l1j1
	cmp	cx, [si+6]
	jge	@@end
	inc	cx
	add	di, 320
	add	bp, dx
	jnc	@@l1jc3
	add	di, bx
	add	[si+0], bx
	jmp	@@l1jc3
@@l1j1:
	cmp	cx, [si+6]
	jg	@@end

	mov	ax, bp
	shr	ax, 4
	and	ax, 0fff0h
	mov	al, 15
	sub	al, ah
	add	es:[di], al
	cmp	byte ptr es:[di], 63
	jna	@@l1j2
	mov	byte ptr es:[di], 63
@@l1j2:
	add	es:[di+bx], ah
	cmp	byte ptr es:[di+bx], 63
	jna	@@l1j3
	mov	byte ptr es:[di+bx], 63
@@l1j3:
	add	di, 320
	add	bp, dx
	jnc	@@l1j4
	add	di, bx
	add	[si+0], bx
	cmp	word ptr [si+0], 319
	jge	@@end
	cmp	word ptr [si+0], 1
	jle	@@end
@@l1j4:
	inc	cx
	jmp	@@l1j1

	jmp	@@end
endm

line2	macro
	mov	ax, [si+2]
	mov	di, ax
	sal	di, 2
	add	di, ax
	sal	di, 6
	add	di, [si+0]

	xor	bp, bp

	ror	ebx, 16
	mov	ax, bx
	ror	ebx, 16

	cmp	word ptr [si+4], 318
	jle	@@l2jc1
	mov	word ptr [si+4], 318
@@l2jc1:
	cmp	cx, 1
	jge	@@l2jc3
	cmp	cx, [si+4]
	jge	@@end
	inc	cx
	inc	di
	add	bp, dx
	jnc	@@l2jc1
	add	di, bx
	add	[si+2], ax
	jmp	@@l2jc1

@@l2jc3:
	cmp	word ptr [si+2], 200
	jb	@@l2j1
	jge	@@end
	cmp	cx, [si+4]
	jge	@@end
	inc	cx
	inc	di
	add	bp, dx
	jnc	@@l2jc3
	add	di, bx
	add	[si+2], ax
	jmp	@@l2jc3

@@l2j1:
	cmp	cx, [si+4]
	jg	@@end

	mov	ax, bp
	shr	ax, 4
	and	ax, 0fff0h
	mov	al, 15
	sub	al, ah
	add	es:[di], al
	cmp	byte ptr es:[di], 63
	jna	@@l2j2
	mov	byte ptr es:[di], 63
@@l2j2:
	add	es:[di+bx], ah
	cmp	byte ptr es:[di+bx], 63
	jna	@@l2j3
	mov	byte ptr es:[di+bx], 63
@@l2j3:
	inc	di
	add	bp, dx
	jnc	@@l2j4
	add	di, bx
	ror	ebx, 16
	add	[si+2], bx
	ror	ebx, 16
	cmp	word ptr [si+2], 200
	jae	@@end
@@l2j4:
	inc	cx
	jmp	@@l2j1

	jmp	@@end
endm

xchange macro
	mov	ax, [si+6]
	mov	cx, [si+2]
	mov	[si+6], cx
	mov	[si+2], ax

	mov	ax, [si+4]
	mov	cx, [si+0]
	mov	[si+4], cx
	mov	[si+0], ax
endm

aaline	proc
	pusha
@@jcc1:
	push	cx

	mov	bx, [si+6]
	sub	bx, [si+2]
	jns	@@jnx
	neg	bx
	xchange
@@jnx:
	mov	dx, [si+4]
	sub	dx, [si+0]

	mov	ax, dx
	or	ax, ax
	jns	@@jny
	neg	ax
@@jny:
	cmp	bx, ax
	jl	@@l2
	jg	@@l1
@@l0:
	or	bx, bx
	jz	@@end
	mov	cx, [si+2]
	or	dx, dx
	js	@@l02
@@l01:
	mov	bx, 1
	jmp	@@ln0
@@l02:
	mov	bx, -1
@@ln0:
	line0
@@l1:
	or	bx, bx
	jz	@@end
	mov	cx, [si+2]
	xor	ax, ax
	or	dx, dx
	js	@@l12
@@l11:
	div	bx
	mov	dx, ax
	mov	bx, 1
	jmp	@@ln1
@@l12:
	neg	dx
	div	bx
	mov	dx, ax
	mov	bx, -1
@@ln1:
	line1
@@l2:
	xchg	bx, dx
	or	bx, bx
	jns	@@jl2
	xchange
	neg	bx
	neg	dx
@@jl2:
	or	bx, bx
	jz	@@end
	xor	ax, ax
	mov	cx, [si+0]
	or	dx, dx
	js	@@l22
@@l21:
	div	bx
	mov	dx, ax
	mov	ebx, 1
	ror	ebx, 16
	mov	bx, 320
	jmp	@@ln2
@@l22:
	neg	dx
	div	bx
	mov	dx, ax
	mov	ebx, -1
	ror	ebx, 16
	mov	bx, -320
@@ln2:
	line2
@@end:
	pop	cx
	add	si, 8
	dec	cx
	jnz	@@jcc1

	popa
	ret
aaline	endp
