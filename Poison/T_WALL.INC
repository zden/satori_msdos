w1_uv	macro
	mov	ebx, men

	mov	eax, te
	cdq
	shld	edx, eax, fixp+txsize
	sal	eax, txsize
	sal	eax, fixp
	idiv	ebx
	mov	du, eax

	mov	eax, th
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

wall	proc
	pushad

	mov	cx, xlen
	sal	ecx, 16
@@j1:
	w1_uv
	push	di
	mov	cx, ylen
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

	add	di, 320
	dec	cx
	jnz	@@j2

	mov	eax, td
	add	ct1, eax
	mov	eax, tg
	add	ct2, eax
	mov	eax, ta
	add	men, eax

	pop	di
	inc	di

	sub	ecx, 10000h
	jnz	@@j1

@@end:
	popad
	ret
wall	endp
