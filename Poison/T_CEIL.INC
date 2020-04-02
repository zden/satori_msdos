c1_uv	macro
	mov	ebx, men

	mov	eax, td
	cdq
	shld	edx, eax, fixp+txsize
	sal	eax, txsize
	sal	eax, fixp
	idiv	ebx
	mov	du, eax

	mov	eax, tg
	cdq
	shld	edx, eax, fixp+txsize
	sal	eax, txsize
	sal	eax, fixp
	idiv	ebx
	mov	dv, eax

	mov	eax, ct1
	cdq
	shld	edx, eax, fixp+txsize
	sal	eax, txsize
	sal	eax, fixp
	idiv	ebx
	mov	tu, eax

	mov	eax, ct2
	cdq
	shld	edx, eax, fixp+txsize
	sal	eax, txsize
	sal	eax, fixp
	idiv	ebx
	mov	tv, eax

	mov	eax, tu
	mov	ebx, tv
endm

ceil	proc
	pushad

	mov	cx, ylen
	sal	ecx, 16
@@j1:
	c1_uv
	push	di
	mov	cx, xlen
@@j2:
	cmp	eax, fixpt*txsz
	jae	@@j3
	cmp	ebx, fixpt*txsz
	jae	@@j3

	mov	esi, eax
	xor	si, si
	sal	esi, txsize
	add	esi, ebx

	shr	esi, fixp
	mov	dl, cs:[TXR+si]
	mov	fs:[di], dl

@@j3:
	add	eax, du
	add	ebx, dv

	inc	di
	dec	cx
	jnz	@@j2

	mov	eax, te
	add	ct1, eax
	mov	eax, th
	add	ct2, eax
	mov	eax, tb
	add	men, eax

	pop	di
	add	di, 320

	sub	ecx, 10000h
	jnz	@@j1

@@end:
	popad
	ret
ceil	endp
