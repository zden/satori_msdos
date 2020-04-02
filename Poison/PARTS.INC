; ******************************************************************************
ende	proc
	mov	quit, 1
	ret
ende	endp

;set_toggle	proc
;	cmp	control_commands_set1, 1
;	jz	@@j1
;	mov	control_commands_set1, 1
;	mov	control_commands_set2, 0
;	ret
;@@j1:
;	mov	control_commands_set1, 0
;	mov	control_commands_set2, 1
;	ret
;set_toggle	endp
;
;set1	proc
;	mov	control_commands_set1, 1
;	mov	control_commands_set2, 0
;	ret
;set1	endp
;
;set2	proc
;	mov	control_commands_set1, 0
;	mov	control_commands_set2, 1
;	ret
;set2	endp

black_palette	proc
	store_palette	black_pal, 0, 64
	ret
black_palette	endp

white_palette	proc
	store_palette	white_pal, 0, 64
	ret
white_palette	endp

grey_palette	proc
	store_palette	grey_pal, 0, 64
	ret
grey_palette	endp

grey2_palette	proc
	store_palette	grey2_pal, 0, 64
	ret
grey2_palette	endp

grey3_palette	proc
	store_palette	grey3_pal, 0, 64
	ret
grey3_palette	endp

grey4_palette	proc
	store_palette	grey4_pal, 0, 64
	ret
grey4_palette	endp

grey_inv_palette	proc
	store_palette	grey_inv_pal, 0, 64
	ret
grey_inv_palette	endp

flash1_palette	proc
	store_palette	flash1_pal, 0, 64
	store_command	3
	ret
flash1_palette	endp

flash2_palette	proc
	store_palette	flash2_pal, 0, 64
	store_command	3
	ret
flash2_palette	endp

flash3_palette	proc
	store_palette	flash3_pal, 0, 64
	store_command	3
	ret
flash3_palette	endp

flash4_palette	proc
	store_palette	white_pal, 0, 64
	store_command	3
	ret
flash4_palette	endp

hardflash	proc
	cmp	hardflash_stop_, 1
	jz	@@e
	xor	hardflash_p, 1
	jz	@@ee

	call	hardflash_colorset
@@ee:
	store_command	11
@@e:
	mov	hardflash_stop_, 0
	ret
hardflash_pos	db 0
hardflash_p	db 0
hardflash_stop_	db 0
hardflash_int	db 63
hardflash	endp

hardflash2	proc
	cmp	hardflash_stop_, 1
	jz	@@e

	call	hardflash_colorset
	store_command	19
@@e:
	mov	hardflash_stop_, 0
	ret
hardflash2	endp

hardflash_colorset	proc
	cmp	hardflash_pos, 1
	jz	@@j1
	mov	al, 0
	mov	ah, hardflash_int
	mov	cx, 64*3
	mov	si, offset black_pal
	mov	di, offset grey_pal
	call	makepal1to2
	store_palette	_temppal, 0, 64
	jmp	@@j2
@@j1:
	mov	al, 0
	mov	ah, hardflash_int
	mov	cx, 64*3
	mov	si, offset black_pal
	mov	di, offset grey_inv_pal
	call	makepal1to2
	store_palette	_temppal, 0, 64
@@j2:
	xor	hardflash_pos, 1
	ret
hardflash_colorset	endp

hardflash_stop	proc
	mov	hardflash_stop_, 1
;	mov	al, 0
;	mov	ah, hardflash_int
;	mov	cx, 64*3
;	mov	si, offset black_pal
;	mov	di, offset grey_inv_pal
;	call	makepal1to2
;	store_palette	_temppal, 0, 64
	ret
hardflash_stop	endp

hardflash_zeroint	proc
	mov	hardflash_int, 0
	ret
hardflash_zeroint	endp

hardflash_intup	proc
	add	hardflash_int, 4
	cmp	hardflash_int, 64
	jb	@@j1
	mov	hardflash_int, 63
@@j1:
	ret
hardflash_intup	endp

hardflash_fullint	proc
	mov	hardflash_int, 63
	ret
hardflash_fullint	endp

hardflash_intdown	proc
	sub	hardflash_int, 4
	cmp	hardflash_int, 64
	jb	@@j1
	mov	hardflash_int, 0
@@j1:
	ret
hardflash_intdown	endp

; ******************************************************************************

shake1	proc
	mov	si, offset shake1_data
	call	do_shake
	ret
shake1_data:
	db 0, 4, 40
	dw 0, 160-80, 160+80, 160
shake1	endp

shake2	proc
	mov	si, offset shake2_data
	call	do_shake
	ret
shake2_data:
	db 0, 4, 41
	dw 0, 50*320-44, 150*320-44, 100*320-44
shake2	endp

shake_boom	proc
	mov	si, offset shake_boom_data
	call	do_shake
	ret
shake_boom_data:
	db 0, 7, 42
	dw 0, 1*320, -1*320, 2*320, -2*320, 3*320, -3*320, 4*320, -4*320
shake_boom	endp

shake3	proc
	mov	si, offset shake3_data
	call	do_shake
	ret
shake3_data:
	db 0, 2, 43
	dw 0, 100*320 ; 100*320+160, 50*320 ; , 150*320+80, 20*320+160+80
shake3	endp

do_shake	proc
	cmp	byte ptr [si], 0
	jnz	@@j1
	mov	al, [si+1]
	mov	byte ptr [si], al
@@j1:
	dec	byte ptr [si]
	mov	bl, [si]
	xor	bh, bh
	shl	bl, 1
	mov	ax, [si + bx + 3]
	mov	[prepared_screen_offset_write], ax
	new_screen
	cmp	byte ptr [si], 0
	jz	@@e
	mov	al, byte ptr [si + 2]
	store_command	al
@@e:
	ret
do_shake	endp

; ******************************************************************************
head_down	proc
	mov	fs, _temp_seg1
	call	do_head_down
	new_screen
	ret
head_down	endp

;do_mirror	proc
;	xor	si, si
;	mov	di, 63999
;	mov	cx, 32000
;@@l1:
;	mov	al, fs:[si]
;	xchg	fs:[di], al
;	mov	fs:[si], al
;	inc	si
;	dec	di
;	dec	cx
;	jnz	@@l1
;	ret
;do_mirror	endp

do_head_down	proc
	xor	si, si
	mov	di, 199*320
	mov	bx, 100
@@l2:
	mov	cx, 320/4
@@l1:
	mov	eax, fs:[si]
	xchg	fs:[di], eax
	mov	fs:[si], eax
	add	si, 4
	add	di, 4
	dec	cx
	jnz	@@l1
	sub	di, 640
	dec	bx
	jnz	@@l2
	ret
do_head_down	endp

; ******************************************************************************

xor_crash	proc
	cmp	xor_crash_status, 0
	jnz	@@do
;	mov	ax, 3
	call	gener_xor_crash
	store_command	45
@@do:
	mov	si, offset xor_crash_data
	call	do_xor_crash
	new_screen
	xor	xor_crash_status, 1
	ret
xor_crash_data	dw 13 DUP (0)
xor_crash_status db 0
xor_crash	endp

gener_xor_crash	proc
	mov	si, offset xor_crash_data
	mov	ax, 3
	call	random
	add	ax, 3
	mov	[si], ax
	mov	cx, ax
	add	si, 2
@@l1:
	mov	ax, 63000
	call	random
	mov	[si], ax
	add	si, 2
	mov	ax, 256
	call	random
	add	ax, 128
	mov	[si], ax
	add	si, 2
	loop @@l1
	ret
gener_xor_crash	endp

xor_crash_cont	proc
	cmp	xor_crash_status, 1
	jz	@@j1
	mov	ax, 6
	call	random
	or	ax, ax
	jnz	@@e
;	mov	ax, 3
	call	gener_xor_crash
	jmp	@@j2
@@j1:
	cmp	prepared_screen, 1
	jz	@@j3
@@j2:
	mov	si, offset xor_crash_data
	call	do_xor_crash
@@j3:
	xor	xor_crash_status, 1
@@e:
	new_screen
	cmp	xor_crash_status, 1
	jz	@@j4
	cmp	xor_crash_cont_stop_, 1
	jz	@@e2
@@j4:
	store_command	46
	ret
@@e2:
	mov	xor_crash_cont_stop_, 0
	ret
xor_crash_cont_stop_	db 0
xor_crash_cont	endp

xor_crash_cont_stop	proc
	mov	xor_crash_cont_stop_, 1
	ret
xor_crash_cont_stop	endp

; ******************************************************************************
synchro_crash	proc
	cmp	prepared_screen, 0
	jz	@@e
	mov	fs, _temp_seg1
	mov	si, offset synchro_crash_data
	cmp	synchro_crash_status, 0
	jnz	@@sf
	mov	ax, 180
;	mov	ax, 100
	call	random
;	add	ax, 50
	mov	bx, 320
	mul	bx
	mov	di, ax
	add	di, 319
	mov	synchro_crash_pos, di
	store_command	51
	jmp	@@sf2
@@sf:
	mov	di, synchro_crash_pos
;	store_command	51
;	jmp	@@sf2
@@sf2:
	mov	cx, 16
@@l1:
	push	cx di
	mov	bl, [si]
	xor	bh, bh
	mov	dx, 320
	sub	dx, bx
	neg	bx
@@j1:
	mov	al, fs:[di+bx]
	mov	fs:[di], al
	dec	di
	dec	dx
	jnz	@@j1
	neg	bx
	jz	@@j2
@@j3:
	mov	byte ptr fs:[di], 0
	dec	di
	dec	bx
	jnz	@@j3
@@j2:
	pop	di cx
	inc	si
	add	di, 320
	loop	@@l1
@@e:
	xor	synchro_crash_status, 1
	ret
synchro_crash_data:
	db 2, 4, 7, 11, 16, 18, 19, 22
	db 24, 21, 17, 14, 7, 4, 3, 2
synchro_crash_pos	dw 0
synchro_crash_status	db 0

synchro_crash	endp

; ******************************************************************************
bassdrum	proc
	mov	fs, _module_seg
	mov	gs, _temp_seg1
	cmp	bassdrum_inprog, 0
	jz	@@j1
	cmp	prepared_screen, 1
	jz	@@j1
	call	do_bassdrum
@@j1:
	mov	bassdrum_pos, 0
	call	do_bassdrum
	cmp	bassdrum_inprog, 0
	jnz	@@j2
	store_command	50
@@j2:
	mov	bassdrum_inprog, 1
	ret
bassdrum	endp

bassdrum_progress	proc
	mov	fs, _module_seg
	mov	gs, _temp_seg1
	cmp	prepared_screen, 1
	jz	@@j1
	call	do_bassdrum
@@j1:
	add	bassdrum_pos, 256+128
	cmp	bassdrum_pos, 27034-21704
	jb	@@okb2
;	sub	bassdrum_pos, 27034-21704
	mov	bassdrum_inprog, 0
	jmp	@@e
@@okb2:
	call	do_bassdrum
	new_screen
	store_command	50
@@e:
	new_screen
	ret
bassdrum_pos	dw 0
bassdrum_inprog	db 0
bassdrum_progress	endp

do_bassdrum	proc
	mov	si, 21704
	add	si, bassdrum_pos
	xor	cx, cx
	xor	bp, bp
@@l1:
	mov	al, fs:[si]
	xor	al, 128
	xor	ah, ah
	shr	ax, 1
	add	ax, (200-128)/2
	mov	bx, 320
	mul	bx
	mov	di, ax
	add	di, cx
	mov	al, fs:[si]
	xor	al, 128
	shr	al, 1
	mov	ah, fs:[si+1]
	xor	ah, 128
	shr	ah, 1
	add	ax, 0101h
	cmp	al, ah
	jb	@@j1
@@j2:
	xor	byte ptr gs:[di], 63
	sub	di, 320
	dec	al
	cmp	al, ah
	jnb	@@j2
	jmp	@@jj
@@j1:
	xor	byte ptr gs:[di], 63
	add	di, 320
	inc	al
	cmp	al, ah
	jna	@@j1
@@jj:
	inc	si
	or	bp, bp
	jnz	@@jjj
	cmp	si, 27034
	jb	@@okb1
	mov	bp, 1
@@jjj:
	mov	si, 21704
@@okb1:
	inc	cx
	cmp	cx, 320
	jnz	@@l1
	ret
do_bassdrum	endp

; ******************************************************************************
comment	%

kaos_sign	proc
	cmp	kaos_sign_stop_, 1
	jnz	@@j1
	mov	kaos_sign_stop_, 0
	mov	kaos_sign_status, 0
	jmp	@@e
@@j1:
	mov	kaos_sign_status, 1
	call	clear_temp_seg1
;	call	clear_temp_seg2
	mov	es, _temp_seg1
	call	drawobject c, offset obj_sign1, 60, 0, 63
	mov	bl, kaos_sign_pos
	shl	bl, 1
	xor	bh, bh
	mov	si, [offset kaos_sign_data + bx]
	call	random_fill
;	mov	fs, _temp_seg2
;	mov	gs, _temp_seg1
;	mov	si, offset soften_intro_signs1
;	call	soften
	new_screen
	inc	kaos_sign_pos
	and	kaos_sign_pos, 15
	store_command	27
@@e:
	ret
kaos_sign_pos		db 0
kaos_sign_data:
	dw 15000, 12222, 17000, 19000, 22000, 17777, 19999, 13333
	dw 14555, 25000, 15353, 22222, 19191, 16666, 14444, 16384
kaos_sign_stop_		db 0
kaos_sign_status	db 0
kaos_sign	endp

kaos_sign_stop	proc
	mov	kaos_sign_stop_, 1
	ret
kaos_sign_stop	endp

%

; ******************************************************************************
intro_sign1	proc
	mov	si, offset obj_sign1
	call	intro_do_sign
	ret
intro_sign1	endp

intro_sign2	proc
	mov	si, offset obj_sign2
	call	intro_do_sign
	ret
intro_sign2	endp

intro_sign3	proc
	mov	si, offset obj_sign3
	call	intro_do_sign
	ret
intro_sign3	endp
comment	&
intro_sign4	proc
	mov	si, offset obj_sign4
	call	intro_do_sign
	ret
intro_sign4	endp
&
intro_do_sign	proc
	call	clear_temp_seg1
	call	clear_temp_seg2
	mov	es, _temp_seg2
	call	drawobject c, si, 60, 0, 63
	mov	fs, _temp_seg2
	mov	gs, _temp_seg1
	mov	si, offset soften_intro_signs1
	call	soften

	new_screen
	store_palette	white_pal, 0, 64

	mov	intro_fade_pos, 127
	store_command	26

	ret
intro_do_sign	endp

soften_intro_signs1	dw 30, 2, 320-30-30, 196

intro_fade	proc
	cmp	intro_fade_pos, -1
	jz	@@e
	cmp	intro_fade_pos, 63
	jbe	@@j1
	mov	si, offset white_pal
	mov	di, offset grey_pal
	mov	ah, 127
	sub	ah, intro_fade_pos
	jmp	@@j2
@@j1:
	mov	si, offset grey_pal
	mov	di, offset black_pal
	mov	ah, 63
	sub	ah, intro_fade_pos
@@j2:
	mov	al, 0
	mov	cx, 64*3
	call	makepal1to2
	store_palette	_temppal, 0, 64
	store_command	26
	dec	intro_fade_pos
@@e:
	ret

intro_fade_pos	db 63
intro_fade	endp

; ******************************************************************************

poison	proc
	call	clear_temp_seg1
	call	clear_temp_seg2
	mov	es, _temp_seg2
	call	drawobject c, offset obj_poi, 0, 0, 63
	call	drawobject c, offset obj_son, 160, 0, 63
	mov	fs, _temp_seg2
	mov	gs, _temp_seg1
	mov	si, offset soften_poison2
	call	soften
	new_screen
	ret
soften_poison2	dw 2, 2, 316, 196
poison	endp

poison_1	proc
	call	clear_temp_seg1
	mov	si, offset obj_p1p
	call	do_poison
	ret
poison_1	endp

do_poison	proc
	call	clear_temp_seg2
	mov	es, _temp_seg2
	call	drawobject c, si, 60, 0, 63
	mov	fs, _temp_seg2
	mov	gs, _temp_seg1
	mov	si, offset offset soften_poison
	call	soften

	new_screen
	store_palette	grey_pal, 0, 64
	ret
do_poison	endp
soften_poison	dw 62, 2, 196, 196

; ******************************************************************************
poison_2	proc
	mov	si, offset obj_p2o
	call	do_poison
	ret
poison_2	endp

; ******************************************************************************
poison_3	proc
	mov	si, offset obj_p3i
	call	do_poison
	ret
poison_3	endp

; ******************************************************************************
poison_4	proc
	mov	si, offset obj_p4s
	call	do_poison
	ret
poison_4	endp

; ******************************************************************************
poison_5	proc
	mov	si, offset obj_p5o
	call	do_poison
	ret
poison_5	endp

; ******************************************************************************
poison_6	proc
	mov	si, offset obj_p6n
	call	do_poison
	ret
poison_6	endp

; ******************************************************************************
; si - offset soften datas
; [si] x - [si+2] y - [si+4] len_x - [si+6] len_y
; fs = source seg : gs = dest seg
soften	proc
	xor	di, di
	add	di, [si]
	mov	ax, [si+2]
	mov	bx, 320
	mul	bx
	add	di, ax

	mov	cx, [si+6]
@@l2:
	push	cx
	mov	cx, [si+4]
	push	di
@@l1:
	mov	al, fs:[di-1]
	add	al, fs:[di+1]
	add	al, fs:[di-320]
	add	al, fs:[di+320]
	shr	al, 3
	mov	ah, fs:[di-(320*2)-2]
	add	ah, fs:[di-(320*2)+2]
	add	ah, fs:[di+(320*2)-2]
	add	ah, fs:[di+(320*2)+2]
	shr	ah, 3
	add	al, ah
	mov	gs:[di], al
	inc	di
	loop	@@l1
	pop	di
	add	di, 320
	pop	cx
	loop	@@l2
	ret

soften	endp

soften2	proc
	xor	di, di
	add	di, [si]
	mov	ax, [si+2]
	mov	bx, 320
	mul	bx
	add	di, ax

	mov	cx, [si+6]
@@l2:
	push	cx
	mov	cx, [si+4]
	push	di
@@l1:
	mov	al, fs:[di-1]
	add	al, fs:[di+1]
	add	al, fs:[di-320]
	add	al, fs:[di+320]
	shr	al, 3
	mov	ah, fs:[di-320-1]
	add	ah, fs:[di-320+1]
	add	ah, fs:[di+320-1]
	add	ah, fs:[di+320+1]
	shr	ah, 3
	add	al, ah
	mov	gs:[di], al
	inc	di
	loop	@@l1
	pop	di
	add	di, 320
	pop	cx
	loop	@@l2
	ret

soften2	endp

; ******************************************************************************
do_xor_crash	proc
	mov	fs, _module_seg
	mov	gs, _temp_seg1
	mov	cx, [si]
	add	si, 2
@@l2:
	push	cx si
	mov	cx, [si+2]
	mov	di, [si]
	add	si, cx
	add	si, 15000
@@l1:
	mov	al, fs:[si]
	and	al, 63
	xor	gs:[di], al
	inc	si
	inc	di
	loop	@@l1
	pop	si cx
	add	si, 4
	loop	@@l2
	ret
do_xor_crash	endp

; ******************************************************************************
random_fill	proc
	mov	ax, _module_seg
	mov	fs, ax
	xor	di, di
	mov	cx, 64000
@@l1:
	cmp	byte ptr es:[di], 0
	jz	@@j1
	mov	al, fs:[si]
;	and	al, 63
	and	al, 11111100b
;	neg	al
	mov	es:[di], al
	inc	si
@@j1:
	inc	di
	dec	cx
	jnz	@@l1
	ret

random_fill	endp

; ******************************************************************************
clear_temp_seg1	proc
	mov	gs, _temp_seg1
	call	clear_seg
	ret
clear_temp_seg1	endp

clear_temp_seg2	proc
	mov	gs, _temp_seg2
	call	clear_seg
	ret
clear_temp_seg2	endp

clear_temp_seg3	proc
	mov	gs, _temp_seg3
	call	clear_seg
	ret
clear_temp_seg3	endp

clear_temp_seg4	proc
	mov	gs, _temp_seg4
	call	clear_seg
	ret
clear_temp_seg4	endp

clear_temp_seg5	proc
	mov	gs, _temp_seg5
	call	clear_seg
	ret
clear_temp_seg5	endp

clear_temp_seg6	proc
	mov	gs, _temp_seg6
	call	clear_seg
	ret
clear_temp_seg6	endp

clear_seg	proc
	mov	cx, 16384
	xor	eax, eax
	xor	di, di
@@j1:
	mov	gs:[di], eax
	add	di, 4
	dec	cx
	jnz	@@j1
	ret
clear_seg	endp

; ******************************************************************************

;stretch	proc
;	mov	si, [bx]
;	mov	di, [bx+2]
;
;	mov	ax, [bx+4]
;	shl	eax, 16
;	xor	ecx, ecx
;	mov	cx, [bx+8]
;	xor	edx, edx
;	div	ecx
;	mov	stretch_ver_rem_pos, ax
;	shr	eax, 16
;	mov	stretch_ver_pos, ax
;
;	mov	ax, [bx+6]
;	shl	eax, 16
;	xor	ecx, ecx
;	mov	cx, [bx+10]
;	xor	edx, edx
;	div	ecx
;	mov	stretch_hor_rem_pos, ax
;	shr	eax, 16
;	mov	stretch_hor_pos, ax
;
;	mov	cx, [bx+10]
;@@l2:
;	push	cx si di
;	mov	cx, [bx+8]
;@@l1:
;	mov	al, fs:[si]
;	or	al, al
;	jz	@@j1
;	mov	gs:[di], al
;@@j1:
;	mov	ax, stretch_ver_rem_pos
;	add	stretch_ver_tmp, ax
;	adc	si, stretch_ver_pos
;	inc	di
;	dec	cx
;	jnz	@@l1
;	pop	di si cx
;
;	mov	ax, stretch_hor_rem_pos
;	add	stretch_hor_tmp, ax
;	mov	ax, stretch_hor_pos
;	adc	ax, 0
;	mov	dx, 320
;	mul	dx
;	add	si, ax
;	add	di, 320
;	dec	cx
;	jnz	@@l2
;
;	ret
;
;stretch_ver_pos		dw 0
;stretch_ver_rem_pos	dw 0
;stretch_ver_tmp		dw 0
;stretch_hor_pos		dw 0
;stretch_hor_rem_pos	dw 0
;stretch_hor_tmp		dw 0
;
;stretch	endp
;
;; ******************************************************************************
;
;convert_poison	proc
;	call	clear_temp_seg3
;	mov	bx, offset convert_poison_data
;	mov	cx, 6
;@@l1:
;	push	cx
;	call	clear_temp_seg1
;	call	clear_temp_seg2
;	mov	es, _temp_seg2
;	mov	si, [bx]
;	push	bx
;	call	drawobject c, si, 60, 0, 63
;	mov	fs, _temp_seg2
;	mov	gs, _temp_seg1
;	mov	si, offset offset soften_poison
;	call	soften
;	pop	bx
;	add	bx, 2
;	mov	fs, _temp_seg1
;	mov	gs, _temp_seg3
;	call	stretch
;	add	bx, 6*2
;	pop	cx
;	loop	@@l1
;
;	ret
;
;convert_poison_data:
;	dw offset obj_p1p, 60, 8+0+25*320,     200, 200, 50, 150
;	dw offset obj_p2o, 60, 8+50+3+25*320,  200, 200, 50, 150
;	dw offset obj_p3i, 60, 8+100+25*320,   200, 200, 50, 150
;	dw offset obj_p4s, 60, 8+150-3+25*320, 200, 200, 50, 150
;	dw offset obj_p5o, 60, 8+200+25*320,   200, 200, 50, 150
;	dw offset obj_p6n, 60, 8+250+25*320,   200, 200, 50, 150
;convert_poison	endp

poison_wait_1st	proc
	mov	ax, 0a000h
	mov	gs, ax
	mov	bl, 12
	mov	dl, 10
	call	show_poison_dots
	ret
poison_wait_1st	endp

clear_poison_wait	proc
	call	wait4retrace
	mov	ax, 0a000h
	mov	gs, ax
	mov	di, 320*180
	mov	cx, (320/4)*8
	xor	eax, eax
@@j1:
	mov	gs:[di], eax
	add	di, 4
	dec	cx
	jnz	@@j1
	ret
clear_poison_wait	endp

poison_wait	proc
	pusha
	push	gs
	call	clear_poison_wait

	inc	poison_wait_time
	mov	bl, 12
	mov	dl, 10
	call	show_poison_dots
	mov	bl, poison_wait_time
	mov	dl, 30
	call	show_poison_dots
	pop	gs
	popa
	ret
poison_wait	endp

poison_wait_clear	proc
	mov	cx, 32
@@wa:
	call	wait4retrace
	loop	@@wa

	mov	ax, 0a000h
	mov	gs, ax
	mov	di, 320*180
	mov	dx, 320
@@l2:
	xor	bx, bx
	mov	cx, 8
@@l1:
	mov	ax, 63
	call	random
	mov	gs:[di+bx], al
	add	bx, 320
	loop	@@l1
	test	dx, 11
	jnz	@@te
	call	wait4retrace
@@te:
	xor	bx, bx
	xor	al, al
	mov	cx, 8
@@l3:
	mov	gs:[di+bx], al
	add	bx, 320
	loop	@@l3
	inc	di
	dec	dx
	jnz	@@l2
	ret
poison_wait_clear	endp

show_poison_dots	proc
	xor	bh, bh
	inc	bl
	push	dx
	mov	ax, 320
	xor	dh, dh
	mov	dl, ah
	xchg	ah, al
	xor	al, al
	div	bx
	pop	dx

	xor	bp, bp
	mov	di, 320*180 - 4

	xor	ch, ch
	mov	cl, bl
	xor	bl, bl
	dec	cl
@@l1:
	push	dx
	xor	dh, dh
	mov	dl, ah
	add	bl, al
	adc	di, dx
	pop	dx

	push	ax bx cx di
	call	poison_wait_dot
	pop	di cx bx ax
	loop	@@l1
	ret
show_poison_dots	endp

poison_wait_dot	proc
	mov	si, offset poison_wait_dot_data
	mov	cx, 8
@@l2:
	push	cx
	mov	cx, 8
@@l1:
	mov	al, [si]
	or	al, al
	jz	@@j1
	add	al, dl
@@j1:
	add	gs:[di], al
	inc	si
	inc	di
	loop	@@l1
	add	di, 320-8
	pop	cx
	loop	@@l2
	ret
poison_wait_dot	endp

poison_wait_time	db 0

poison_wait_dot_data:
	db 0,0,0,3,3,0,0,0
	db 0,0,3,6,6,3,0,0
	db 0,3,6,6,6,6,3,0
	db 3,6,6,6,6,6,6,3
	db 3,6,6,6,6,6,6,3
	db 0,3,6,6,6,6,3,0
	db 0,0,3,6,6,3,0,0
	db 0,0,0,3,3,0,0,0

; ******************************************************************************

show_dots	proc
	xor	ch, ch
	mov	cl, [si]
	xor	bh, bh
	mov	bl, [si+1]
	shl	bx, 1
	mov	di, offset rotate_sin
@@l1:
	mov	ax, [di+bx]
	xor	dh, dh
	mov	dl, [si+2]
	imul	dx
	sar	ax, 8
	add	ax, 160-8;-8
	mov	bp, ax
;	mov	ax, 16	; !
;	call	random
;	add	bp, ax
	mov	ax, [di+bx+128]
	xor	dh, dh
	mov	dl, [si+2]
	imul	dx
	sar	ax, 8
	add	ax, 100-8;-8
	mov	dx, ax
;	mov	ax, 16
;	call	random
;	add	ax, dx
	mov	dx, 320
	mul	dx
	add	bp, ax
	call	dots_show_pic
	xor	ah, ah
	mov	al, [si+3]
	shl	ax, 1
	add	bx, ax
	and	bx, 111111111b
	loop	@@l1

	ret
show_dots	endp

dots_show_pic	proc
	push	cx si
	mov	si, offset dots_pic
	mov	cx, 16
@@l2:
	push	cx
	mov	cx, 16
@@l1:
	mov	al, [si]
	add	gs:[bp], al
	cmp	byte ptr gs:[bp], 64
	jb	@@j1
	mov	byte ptr gs:[bp], 63
@@j1:
	inc	si
	inc	bp
	loop	@@l1
	add	bp, 320-16
	pop	cx
	loop	@@l2
	pop	si cx
	ret
dots_show_pic	endp

dots_data1	db 8, 0, 80, 32
dots_data2	db 8, 0, 80, 32

dots_pic:
db	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,32,52,63,63,55,40,8,0,0,0,0,0,0,0,20,59,63,63,63,63,63,63,63,32,0,0,0,0,0
db	20,63,63,63,63,63,63,63,63,63,63,36,0,0,0,4,59,63,63,63,63,63,63,63,63,63,63,63,16,0,0,32,63,63,63,63,63,63,63,63,63,63,63,63,48,0,0,52,63,63
db	63,63,63,63,63,63,63,63,63,63,63,4,0,63,63,63,63,63,63,63,63,63,63,63,63,63,63,16,0,63,63,63,63,63,63,63,63,63,63,63,63,63,63,16,0,55,63,63,63,63
db	63,63,63,63,63,63,63,63,63,8,0,40,63,63,63,63,63,63,63,63,63,63,63,63,55,0,0,8,63,63,63,63,63,63,63,63,63,63,63,63,24,0,0,0,32,63,63,63,63,63
db	63,63,63,63,63,48,0,0,0,0,0,36,63,63,63,63,63,63,63,63,48,4,0,0,0,0,0,0,16,48,63,63,63,63,55,24,0,0,0,0,0,0,0,0,0,0,4,16,16,8
db	0,0,0,0,0,0

dots_len	EQU 1000
dots_frames	EQU 150/2

dots_init_timer	proc
	xor	edx, edx
	mov	ax, dots_len
	shl	eax, 16
	xor	ebx, ebx
	mov	bx, dots_frames
	div	ebx
	mov	dots_speed, eax
	reset_counter
	ret
dots_init_timer	endp

dots_get_timer	proc
	get_counter
	xor	edx, edx
	shl	eax, 16
	mov	ebx, dots_speed
	div	ebx
	ret
dots_get_timer	endp

dots_part1	proc
	xor	dots_stop, 1
	jnz	@@j1
	call	dots_init_timer

	mov	byte ptr [dots_data1+1], 0
	mov	byte ptr [dots_data1+2], 80
	mov	byte ptr [dots_data2+1], 16
	mov	byte ptr [dots_data2+2], 40
	store_command	161
	ret
@@j1:
	call clear_temp_seg1
	new_screen
	ret
dots_part1	endp

dots_part2	proc
	xor	dots_stop, 1
	jnz	@@j1
	call	dots_init_timer

	mov	byte ptr [dots_data1+2], 40
	mov	byte ptr [dots_data2+2], 80
	store_command	163
	ret
@@j1:
	call clear_temp_seg1
	new_screen
	ret
dots_part2	endp

dots_part3	proc
	xor	dots_stop, 1
	jnz	@@j1
	call	dots_init_timer

	mov	byte ptr [dots_data1+2], 80
	mov	byte ptr [dots_data2+2], 40
	store_command	165
	ret
@@j1:
	call clear_temp_seg1
	new_screen
	ret
dots_part3	endp

dots_part1_progress	proc
	cmp	dots_stop, 1
	jz	@@ee
	call	dots_get_timer
	cmp	ax, dots_lframe
	jz	@@e

	mov	dots_frame, ax
	mov	ax, dots_lframe
@@l1:
	inc	ax
	inc	byte ptr [dots_data1+1]
	dec	byte ptr [dots_data1+2]
	dec	byte ptr [dots_data2+1]
	inc	byte ptr [dots_data2+2]
	cmp	ax, dots_frame
	jnz	@@l1
	mov	ax, dots_frame
	mov	dots_lframe, ax

	call	clear_temp_seg1
	mov	gs, _temp_seg1
	mov	si, offset dots_data1
	call	show_dots
	mov	si, offset dots_data2
	call	show_dots
;	mov	bp, (100-8)*320+(160-8)
;	call	dots_show_pic
	new_screen
@@e:
	store_command	161
@@ee:
	ret
dots_part1_progress	endp

dots_part2_progress	proc
	cmp	dots_stop, 1
	jz	@@ee
	call	dots_get_timer
	cmp	ax, dots_lframe
	jz	@@e

	mov	dots_frame, ax
	mov	ax, dots_lframe
@@l1:
	inc	ax
	dec	byte ptr [dots_data1+1]
	inc	byte ptr [dots_data2+1]
	cmp	ax, dots_frame
	jnz	@@l1
	mov	ax, dots_frame
	mov	dots_lframe, ax

	call	clear_temp_seg1
	mov	gs, _temp_seg1
	mov	si, offset dots_data1
	call	show_dots
	mov	si, offset dots_data2
	call	show_dots
;	mov	bp, (100-8)*320+(160-8)
;	call	dots_show_pic
	new_screen
@@e:
	store_command	163
@@ee:
	ret
dots_part2_progress	endp

dots_part3_progress	proc
	cmp	dots_stop, 1
	jz	@@ee
	call	dots_get_timer
	cmp	ax, dots_lframe
	jz	@@e

	mov	dots_frame, ax
	mov	ax, dots_lframe
@@l1:
	inc	ax
	dec	byte ptr [dots_data1+1]
	inc	byte ptr [dots_data1+2]
	inc	byte ptr [dots_data2+1]
	dec	byte ptr [dots_data2+2]
	cmp	ax, dots_frame
	jnz	@@l1
	mov	ax, dots_frame
	mov	dots_lframe, ax

	call	clear_temp_seg1
	mov	gs, _temp_seg1
	mov	si, offset dots_data1
	call	show_dots
	mov	si, offset dots_data2
	call	show_dots
;	mov	bp, (100-8)*320+(160-8)
;	call	dots_show_pic
	new_screen
@@e:
	store_command	165
@@ee:
	ret
dots_part3_progress	endp

dots_stop	db 1
dots_speed	dd 0
dots_frame	dw 0
dots_lframe	dw -1

; ******************************************************************************

include	blur.inc
include	rotate.inc
include	triang.inc
include	end.inc
include	lights.inc
include	plasma.inc
include	tunel.inc
include	tunnel.inc

include	objects\p1p.tbl
include	objects\p2o.tbl
include	objects\p3i.tbl
include	objects\p4s.tbl
include	objects\p5o.tbl
include	objects\p6n.tbl
include	objects\poi.tbl
include	objects\son.tbl
include	objects\sign1.tbl
include	objects\sign2.tbl
include	objects\sign3.tbl
;include	objects\sign4.tbl
include objects\sign5.tbl
include objects\sign6.tbl
include	objects\sign7.tbl
include	objects\zd.tbl
include	objects\en.tbl
include	objects\mos.tbl
include	objects\he.tbl
include objects\end.tbl