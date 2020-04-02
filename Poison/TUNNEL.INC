;tunnel datas
include t_data.inc

;predfined constants
wsize	EQU 128
frame	EQU 4
mframes EQU 3*4*frame
mspeed	EQU 3*wsize/(mframes)
tframes EQU 1*4*frame
tspeed	EQU 64/(tframes)
nbox	EQU 5
runs	EQU 10
walls	EQU runs*12
fixp	EQU 16
fixpt	EQU 65536

;motion predefinitions
actpos	EQU word ptr cs:[mnums+0]
actbox	EQU word ptr cs:[mnums+2]
alf	EQU word ptr cs:[tnums+0]
bet	EQU word ptr cs:[tnums+2]
dalf	EQU word ptr cs:[tnums+4]
dbet	EQU word ptr cs:[tnums+6]
tsin	EQU word ptr cs:[SIN+bx]
tcos	EQU word ptr cs:[COS+bx]
tx	EQU word ptr cs:[di+tunn1+0]
ty	EQU word ptr cs:[di+tunn1+2]
tz	EQU word ptr cs:[di+tunn1+4]
xt	EQU word ptr cs:[di+tunnel+0]
yt	EQU word ptr cs:[di+tunnel+2]
zt	EQU word ptr cs:[di+tunnel+4]
tctr	EQU word ptr cs:[mtnums+0]
degcnt	EQU word ptr cs:[mtnums+2]
rda	EQU word ptr cs:[turns+si+0]
rdb	EQU word ptr cs:[turns+si+2]


include txrinit.inc
include t_ceil.inc
include t_wall.inc

drawpl	proc
	pushad

	basics
	cmp	tb, 0
	jne	@@jdc
	call	wall
	jmp	@@end
@@jdc:
	cmp	ta, 0
	jne	@@end
	call	ceil
@@end:
	popad
	ret

nums	dd 18 dup (0)
n3ds	dw 12 dup (0)
n2ds	dw 10 dup (0)
drawpl endp

drawt	proc
	pushad

	mov	fs, _temp_seg2

	mov	si, actbox
	shl	si, 3
	add	si, actbox
	shl	si, 3
	add	si, offset tunnel+(4*nbox-1)*18
	mov	cx, 4*nbox
@@j1:
	call	drawpl
	sub	si, 18
	dec	cx
	jnz	@@j1

;	 call	 w4r
;	mov	cx, 16000
;	mov	di, 0
;@@j2:
;	mov	eax, fs:[di]
;	mov	es:[di], eax
;	xor	eax, eax
;	mov	fs:[di], eax
;	add	di, 4
;	dec	cx
;	jnz	@@j2

	popad
	ret
drawt	endp

movet	proc
	pushad

	mov	si, offset tunnel
	mov	cx, walls
@@j1:
	sub	word ptr cs:[si+4], mspeed
	add	si, 18
	dec	cx
	jnz	@@j1

	add	actpos, mspeed
	mov	ax, actpos
	cmp	ax, wsize
	jb	@@j2
	inc	actbox
	mov	actpos, 0
@@j2:
	popad
	ret
mnums	dw 2 dup (0)
movet	endp

rotatex proc
	mov	bx, alf
	and	bx, 255
	shl	bx, 1

	xor	di, di
	mov	cx, walls*3
@@j1:
	mov	ax, tx
	mov	xt, ax
	mov	ax, ty
	mov	yt, ax
	mov	ax, tz
	mov	zt, ax

	movsx	eax, ty
	movsx	edx, tcos
	imul	edx
	mov	ebp, eax

	movsx	eax, tz
	movsx	edx, tsin
	imul	edx
	sub	ebp, eax
	sar	ebp, 14
	mov	yt, bp

	movsx	eax, ty
	movsx	edx, tsin
	imul	edx
	mov	ebp, eax

	movsx	eax, tz
	movsx	edx, tcos
	imul	edx
	add	ebp, eax
	sar	ebp, 14
	mov	zt, bp

	add	di, 6
	dec	cx
	jnz	@@j1

	ret
rotatex endp

rotatey proc
	mov	bx, bet
	and	bx, 255
	shl	bx, 1

	xor	di, di
	mov	cx, walls*3
@@j1:
	mov	ax, tx
	mov	xt, ax
	mov	ax, ty
	mov	yt, ax
	mov	ax, tz
	mov	zt, ax

	movsx	eax, tx
	movsx	edx, tcos
	imul	edx
	mov	ebp, eax

	movsx	eax, tz
	movsx	edx, tsin
	imul	edx
	sub	ebp, eax
	sar	ebp, 14
	mov	xt, bp

	movsx	eax, tx
	movsx	edx, tsin
	imul	edx
	mov	ebp, eax

	movsx	eax, tz
	movsx	edx, tcos
	imul	edx
	add	ebp, eax
	sar	ebp, 14
	mov	zt, bp

	add	di, 6
	dec	cx
	jnz	@@j1

	ret
rotatey endp

turnt	proc
	pushad

	cmp	alf, 0
	je	@@jry
	cmp	bet, 0
	je	@@jrx
	jmp	@@end
@@jrx:
	call rotatex
	jmp	@@end
@@jry:
	call rotatey
@@end:
	popad
	ret
tnums	dw 4 dup (0)
turnt	endp

prept	proc
	pushad

	xor	si, si
	mov	cx, walls*9
@@j1:
	mov	ax, cs:[tunnel+si]
	mov	cs:[tunn1+si], ax
	add	si, 2
	dec	cx
	jnz	@@j1

	popad
	ret
prept	endp

motion	proc
	pushad

	cmp	degcnt, (runs-3)*4
	je	@@jmm
	cmp	tctr, mframes
	ja	@@jmt
	je	@@jmti
@@jmm:
	call	movet
	inc	tctr
	jmp	@@end
@@jmti:
	call	prept
	mov	si, degcnt
	mov	ax, rda
	mov	bx, rdb
	mov	dalf, ax
	mov	dbet, bx
	mov	alf, 0
	mov	bet, 0
@@jmt:
	mov	ax, dalf
	mov	bx, dbet
	add	alf, ax
	add	bet, bx
	call	turnt
	inc	tctr
	cmp	tctr, mframes+tframes
	jl	@@end
	mov	tctr, 0
	add	degcnt, 4
@@end:
	popad
	ret
mtnums	dw 2 dup (0)
motion	endp

;initt	proc
;	mov	ax, code
;	mov	ds, ax
;	mov	ax, _temp_seg2
;	mov	fs, _temp_seg2
;	mov	ax, 0a000h
;	mov	es, ax
;	call	drawt
;	call	motion
;	ret
;initt	endp

;dotunn	proc
;	call	drawt
;	call	motion
;	ret
;dotunn	endp

tunnel_start	proc
	call	clear_temp_seg1
	xor	tunnel_stop, 1
	call	tunnel_norm_pal
	mov	word ptr _temppal[64*3], 0
	mov	byte ptr _temppal[64*3+2], 0
	store_palette	_temppal, 0, 65
	store_command	181
	mov	ax, _temp_seg1
	mov	prepared_screen_segment, ax
	ret
tunnel_start	endp

tunnel_progress	proc
	cmp	tunnel_stop, 1
	jz	@@ee
	mov	ax, tunnel_last_pos
	cmp	ax, tunnel_pos
	jz	@@e

@@j1:
	call	motion
	inc	ax
	cmp	ax, tunnel_pos
	jnz	@@j1

	mov	ax, tunnel_pos
	mov	tunnel_last_pos, ax

	call	tunnel_clear_seg
	call	drawt
;	mov	fs, _temp_seg2
;	mov	gs, _temp_seg1
;	mov	si, offset tunnel_soften
;	call	soften
	mov	ax, _temp_seg2
	mov	prepared_screen_segment, ax
	new_screen
@@e:
	store_command	181
@@ee:
	ret
tunnel_pos	dw 0
tunnel_last_pos	dw -1
tunnel_stop	db 1
tunnel_progress	endp

tunnel_norm_pal	proc
	mov	al, 0
	mov	ah, 40
	mov	cx, 64*3
	mov	si, offset grey_pal
	mov	di, offset black_pal
	call	makepal1to2
	store_palette	_temppal, 0, 64
	ret
tunnel_norm_pal	endp

tunnel_flash1_pal	proc
	mov	al, 0
	mov	ah, 30
	mov	cx, 64*3
	mov	si, offset grey_pal
	mov	di, offset black_pal
	call	makepal1to2
	store_palette	_temppal, 0, 64
	store_command	182
	ret
tunnel_flash1_pal	endp

tunnel_flash2_pal	proc
	mov	al, 0
	mov	ah, 20
	mov	cx, 64*3
	mov	si, offset grey_pal
	mov	di, offset black_pal
	call	makepal1to2
	store_palette	_temppal, 0, 64
	store_command	182
	ret
tunnel_flash2_pal	endp

tunnel_flash3_pal	proc
	mov	al, 0
	mov	ah, 10
	mov	cx, 64*3
	mov	si, offset grey_pal
	mov	di, offset black_pal
	call	makepal1to2
	store_palette	_temppal, 0, 64
	store_command	182
	ret
tunnel_flash3_pal	endp

tunnel_flash4_pal	proc
	store_palette	grey_pal, 0, 64
	store_command	182
	ret
tunnel_flash4_pal	endp

tunnel_norm_inv_pal	proc
	mov	al, 0
	mov	ah, 40
	mov	cx, 64*3
	mov	si, offset grey_inv_pal
	mov	di, offset black_pal
	call	makepal1to2
	store_palette	_temppal, 0, 64
	store_command	182
	ret
tunnel_norm_inv_pal	endp

tunnel_clear_seg	proc
	mov	gs, _temp_seg2
	mov	cx, 16384
	mov	eax, 40404040h
	xor	di, di
@@j1:
	mov	gs:[di], eax
	add	di, 4
	dec	cx
	jnz	@@j1
	ret
tunnel_clear_seg	endp

tunnel_soften	proc
	mov	fs, _temp_seg2
	mov	gs, _temp_seg1
	mov	si, offset tunnel_soften_data
	call	soften
	mov	ax, _temp_seg1
	mov	prepared_screen_segment, ax
	ret
tunnel_soften	endp
tunnel_soften_data	dw 2, 2+20, 320-4, 200-4-40