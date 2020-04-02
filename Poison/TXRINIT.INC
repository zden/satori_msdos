;predefined constants
persp	EQU 7
pers	EQU 128
txsize	EQU 6
txsz	EQU 64

;textured plane vectors
b0	EQU word ptr cs:[si+0]
b1	EQU word ptr cs:[si+2]
b2	EQU word ptr cs:[si+4]

u0	EQU word ptr cs:[si+6]
u1	EQU word ptr cs:[si+8]
u2	EQU word ptr cs:[si+10]

v0	EQU word ptr cs:[si+12]
v1	EQU word ptr cs:[si+14]
v2	EQU word ptr cs:[si+16]

;precalculated constants
ta	EQU dword ptr cs:[nums+0]
tb	EQU dword ptr cs:[nums+4]
tc	EQU dword ptr cs:[nums+8]
td	EQU dword ptr cs:[nums+12]
te	EQU dword ptr cs:[nums+16]
tf	EQU dword ptr cs:[nums+20]
tg	EQU dword ptr cs:[nums+24]
th	EQU dword ptr cs:[nums+28]
ti	EQU dword ptr cs:[nums+32]
ct1	EQU dword ptr cs:[nums+36]
ct2	EQU dword ptr cs:[nums+40]
men	EQU dword ptr cs:[nums+44]
ustart	EQU dword ptr cs:[nums+48]
vstart	EQU dword ptr cs:[nums+52]
du	EQU dword ptr cs:[nums+56]
dv	EQU dword ptr cs:[nums+60]
tu	EQU dword ptr cs:[nums+64]
tv	EQU dword ptr cs:[nums+68]

;clip constants
k0	EQU word ptr cs:[n3ds+0]
k1	EQU word ptr cs:[n3ds+2]
k2	EQU word ptr cs:[n3ds+4]
l0	EQU word ptr cs:[n3ds+6]
l1	EQU word ptr cs:[n3ds+8]
l2	EQU word ptr cs:[n3ds+10]
m0	EQU word ptr cs:[n3ds+12]
m1	EQU word ptr cs:[n3ds+14]
m2	EQU word ptr cs:[n3ds+16]
n0	EQU word ptr cs:[n3ds+18]
n1	EQU word ptr cs:[n3ds+20]
n2	EQU word ptr cs:[n3ds+22]

xstart	EQU word ptr cs:[n2ds+0]
xend	EQU word ptr cs:[n2ds+12]
xlen	EQU word ptr cs:[n2ds+16]
ystart	EQU word ptr cs:[n2ds+2]
yend	EQU word ptr cs:[n2ds+14]
ylen	EQU word ptr cs:[n2ds+18]


points	macro
	mov	ax, b0
	mov	k0, ax
	mov	ax, b1
	mov	k1, ax
	mov	ax, b2
	mov	k2, ax

	mov	ax, b0
	add	ax, u0
	mov	l0, ax
	mov	ax, b1
	add	ax, u1
	mov	l1, ax
	mov	ax, b2
	add	ax, u2
	mov	l2, ax

	mov	ax, b0
	add	ax, v0
	mov	n0, ax
	mov	ax, b1
	add	ax, v1
	mov	n1, ax
	mov	ax, b2
	add	ax, v2
	mov	n2, ax

	mov	ax, l0
	add	ax, v0
	mov	m0, ax
	mov	ax, l1
	add	ax, v1
	mov	m1, ax
	mov	ax, l2
	add	ax, v2
	mov	m2, ax

	cmp	k2, 0
	jge	@@jdw
	cmp	l2, 0
	jge	@@jdw
	cmp	m2, 0
	jge	@@jdw
	cmp	n2, 0
	jge	@@jdw
	pop	si
	jmp	@@end
@@jdw:
endm

proj2d	macro
	mov	si, offset n3ds
	mov	di, offset n2ds
	mov	cx, 4
@@jp2d1:
	movsx	ebx, word ptr cs:[si+4]
	cmp	ebx, 1
	jg	@@jp2d2

	mov	ax, 160
	cmp	word ptr cs:[si+0], 0
	jg	@@jpc1
	neg	ax
@@jpc1:
	mov	cs:[di+0], ax

	mov	ax, 100
	cmp	word ptr cs:[si+2], 0
	jg	@@jpc2
	neg	ax
@@jpc2:
	mov	cs:[di+2], ax
	jmp	@@jp2d3

@@jp2d2:
	movsx	eax, word ptr cs:[si+0]
	mov	edx, eax
	sal	eax, persp
	sar	edx, 16
	sar	edx, 16-persp
	idiv	ebx
	mov	cs:[di+0], ax

	movsx	eax, word ptr cs:[si+2]
	cdq
	sal	eax, persp
	idiv	ebx
	mov	cs:[di+2], ax
@@jp2d3:
	add	si, 6
	add	di, 4
	dec	cx
	jnz	@@jp2d1
endm

sortt	macro
	mov	si, offset n2ds
	mov	ch, 3
@@js1:
	mov	di, si
	add	di, 4
	mov	cl, ch
@@js2:
	mov	ax, cs:[si]
	mov	dx, cs:[di]
	cmp	ax, dx
	jle	@@js3
	mov	cs:[si], dx
	mov	cs:[di], ax
@@js3:
	mov	ax, cs:[si+2]
	mov	dx, cs:[di+2]
	cmp	ax, dx
	jle	@@js4
	mov	cs:[si+2], dx
	mov	cs:[di+2], ax
@@js4:
	add	di, 4
	dec	cl
	jnz	@@js2
	add	si, 4
	dec	ch
	jnz	@@js1
endm

clip	macro
	cmp	xstart, 160
	jge	@@end

	cmp	xstart, -160
	jge	@@jc1
	mov	xstart, -160
@@jc1:
	cmp	xend, -160
	jl	@@end

	cmp	xend, 160
	jl	@@jc2
	mov	xend, 159

@@jc2:
	cmp	ystart, 100
	jge	@@end

	cmp	ystart, -100
	jge	@@jc3
	mov	ystart, -100
@@jc3:
	cmp	yend, -100
	jl	@@end

	cmp	yend, 100
	jl	@@jc4
	mov	yend, 99
@@jc4:

	mov	ax, xend
	sub	ax, xstart
	jz	@@end
	inc	ax
	mov	xlen, ax

	mov	ax, yend
	sub	ax, ystart
	jz	@@end
	inc	ax
	mov	ylen, ax
endm

calcn	macro
	movsx	eax, u1
	movsx	ebx, v2
	imul	ebx
	mov	ta, eax
	movsx	eax, u2
	movsx	ebx, v1
	imul	ebx
	sub	ta, eax

	movsx	eax, u2
	movsx	ebx, v0
	imul	ebx
	mov	tb, eax
	movsx	eax, u0
	movsx	ebx, v2
	imul	ebx
	sub	tb, eax

	movsx	eax, u0
	movsx	ebx, v1
	imul	ebx
	mov	tc, eax
	movsx	eax, u1
	movsx	ebx, v0
	imul	ebx
	sub	tc, eax

	movsx	eax, v1
	movsx	ebx, b2
	imul	ebx
	mov	td, eax
	movsx	eax, v2
	movsx	ebx, b1
	imul	ebx
	sub	td, eax

	movsx	eax, v2
	movsx	ebx, b0
	imul	ebx
	mov	te, eax
	movsx	eax, v0
	movsx	ebx, b2
	imul	ebx
	sub	te, eax

	movsx	eax, v0
	movsx	ebx, b1
	imul	ebx
	mov	tf, eax
	movsx	eax, v1
	movsx	ebx, b0
	imul	ebx
	sub	tf, eax

	movsx	eax, b1
	movsx	ebx, u2
	imul	ebx
	mov	tg, eax
	movsx	eax, b2
	movsx	ebx, u1
	imul	ebx
	sub	tg, eax

	movsx	eax, b2
	movsx	ebx, u0
	imul	ebx
	mov	th, eax
	movsx	eax, b0
	movsx	ebx, u2
	imul	ebx
	sub	th, eax

	movsx	eax, b0
	movsx	ebx, u1
	imul	ebx
	mov	ti, eax
	movsx	eax, b1
	movsx	ebx, u0
	imul	ebx
	sub	ti, eax

	mov	eax, tf
	sal	eax, persp
	mov	ct1, eax
	mov	eax, td
	movsx	ebx, xstart
	imul	ebx
	add	ct1, eax
	mov	eax, te
	movsx	ebx, ystart
	imul	ebx
	add	ct1, eax

	mov	eax, ti
	sal	eax, persp
	mov	ct2, eax
	mov	eax, tg
	movsx	ebx, xstart
	imul	ebx
	add	ct2, eax
	mov	eax, th
	movsx	ebx, ystart
	imul	ebx
	add	ct2, eax

	mov	eax, tc
	sal	eax, persp
	mov	men, eax
	mov	eax, ta
	movsx	ebx, xstart
	imul	ebx
	add	men, eax
	mov	eax, tb
	movsx	ebx, ystart
	imul	ebx
	inc	eax
	add	men, eax
endm

basics	macro
	push	si
	points
	proj2d
	sortt
	pop	si

	clip
	calcn

	mov	ax, ystart
	add	ax, 100
	mov	di, ax
	shl	di, 2
	add	di, ax
	shl	di, 6
	add	di, xstart
	add	di, 160
endm
