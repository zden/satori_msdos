light_len	EQU 1000 ;6*1000
light_frames	EQU 100 ;600

; **************************************
light_start	proc
	xor	light_stop, 1
	jnz	@@e
	xor	edx, edx
	mov	ax, light_len
	shl	eax, 16
	xor	ebx, ebx
	mov	bx, light_frames
	div	ebx
	mov	light_speed, eax
	reset_counter
	mov	light_frame, 0
	call	convert_poison2
	store_palette	grey_pal, 0, 128
	store_command	101
@@e:
	ret

light_speed	dd 0
light_frame	dw 0
light_lframe	dw -1
light_pause	db 0
light_ligdir	db 1
light_ligspd	db 4
light_rotpos	db 0
light_rotdir	db 1
light_switch1	db 0
light_switch2	db 0
light_head_down	db 0
light_stop	db 1
;light_next_turn	db 0
light_picoff_	dw 0
light_picoff2_	dw 0
light_picoff_pos	db 0

light_start	endp

; **************************************
light_progress	proc
	cmp	light_stop, 1
	jz	@@e
	get_counter
;	cmp	ax, light_len
;	jnb	@@e

	cmp	light_pause, 1
	jz	@@ee

	xor	edx, edx
	shl	eax, 16
	mov	ebx, light_speed
	div	ebx
	cmp	ax, light_lframe
	jz	@@ee

	mov	light_frame, ax

	mov	ax, light_frame
	sub	ax, light_lframe
	mov	ah, light_ligspd
	mul	ah
	cmp	light_ligdir, 1
	jz	@@j1
	neg	al
@@j1:
	add	light_angle, al

	cmp	light_switch1, 0
	jz	@@light1

	cmp	light_picoff_pos, 0
	jz	@@poj1
	mov	ax, light_picoff_
	jmp	@@poj2
@@poj1:
	mov	ax, light_picoff2_
@@poj2:
	mov	light_picoff, ax
	xor	light_picoff_pos, 1

	inc	light_switch2
	test	light_switch2, 10
	jnz	@@light3
	mov	light_x_pos, 80
	mov	light_y_pos, 50
	mov	light_light, 16384
	jmp	@@light2
@@light1:

	mov	ax, light_frame
	sub	ax, light_lframe
	cmp	light_rotdir, 1
	jz	@@j2
	neg	al
@@j2:
	add	light_rotpos, al
	mov	bl, light_rotpos
	xor	bh, bh
	mov	al, [offset light_move_sin + bx]
	add	al, 80
	mov	light_x_pos, al
	mov	al, [offset light_move_sin + bx + 64]
	add	al, 50
	mov	light_y_pos, al
	mov	al, light_y_add
	add	light_y_pos, al
@@light2:
	call	light_pic
@@sc:
	new_screen

	mov	ax, light_frame
	mov	light_lframe, ax
@@ee:
	store_command	101
@@e:
	ret
@@light3:
	mov	ax, light_frame
	sub	ax, light_lframe
	mov	bl, 6
	mul	bl
	add	light_angle2, al
	add	light2_angle2, al

;	xor	bh, bh
;	mov	bl, light_next_turn
;	shl	bx, 1
	mov	bx, offset light_next_turn_data

	mov	al, [bx]
	mov	light_x_pos, al;40
	mov	al, [bx+1]
	mov	light_y_pos, al;50
	mov	light_light, 32768 ;-16384
	mov	al, light_angle2
	mov	light_angle, al

	mov	light2_x_pos, 159
	mov	al, [bx]
	sub	light2_x_pos, al
	mov	light2_y_pos, 99
	mov	al, [bx+1]
	sub	light2_y_pos, al
	mov	light2_light, 32768 ;-16384
	mov	al, light2_angle2
	mov	light2_angle, al
	call	light_pic
	call	light_pic2
	jmp	@@sc
light_progress	endp

light_angle2	db 0
light2_angle2	db 128
light_next_turn_data	db 80, 50 ; 40, 50, 120, 30, 70, 60, 30, 80, 88, 33

; **************************************

light_shake	proc
	mov	si, offset light_shake_data
	xor	ah, ah
	mov	al, light_shake_pos
	add	si, ax
	mov	al, [si]
	mov	light_y_add, al
	neg	al
	cbw
	mov	bx, 320
	mul	bx
	mov	light_picoff, ax
	inc	light_shake_pos
	cmp	light_shake_pos, 7
	jz	@@j1
	store_command	114
	jmp	@@e
@@j1:
	mov	light_shake_pos, 0
@@e:
	ret
light_shake_pos	db 0
light_shake_data	db -4, 4, -3, 3, -2, 2, -1, 1, 0
light_shake	endp

; **************************************
;light_rotdir_toggle	proc
;	xor	light_rotdir, 1
;	ret
;light_rotdir_toggle	endp
;
;light_ligdir_toggle	proc
;	xor	light_ligdir, 1
;	ret
;light_ligdir_toggle	endp
;
;light_forward	proc
;	mov	al, 20
;	cmp	light_rotdir, 1
;	jz	@@j1
;	neg	al
;@@j1:
;	add	light_rotpos, al
;	ret
;light_forward	endp

light_switch1_on	proc
	mov	light_ligspd, 12
	mov	light_picoff_, 0
	mov	light_picoff2_, 0
	mov	light_switch1, 1
	ret
light_switch1_on	endp

light_next_turn_funct	proc
;	mov	ax, 80
;	call	random
;	add	al, 40
;	mov	byte ptr [light_next_turn_data], al
;	mov	ax, 60
;	add	al, 20
;	mov	byte ptr [light_next_turn_data+1], al
	mov	ax, 256
	call	random
	mov	bx, ax
	shl	bx, 1
	add	bx, offset rotate_sin
	mov	ax, 25
	call	random
	add	ax, 20
	mov	cx, ax

	mov	ax, word ptr [bx]
	imul	cx
	sar	ax, 8
	add	ax, 80
	mov	byte ptr [light_next_turn_data], al

	mov	ax, word ptr [bx+128]
	imul	cx
	sar	ax, 8
	add	ax, 50
	mov	byte ptr [light_next_turn_data+1], al

	add	light_picoff_, 320*2
	sub	light_picoff2_, 320*2
	ret
light_next_turn_funct	endp

; **************************************

light_head_down_funct	proc
	cmp	light_head_down, 0
	jz	@@j1
	cmp	light_head_down, 3
	jnz	@@j2
	mov	light_head_down, -1
@@j1:
	mov	fs, _temp_seg3
	call	do_head_down
	call	light_letter
@@j2:
	inc	light_head_down
	cmp	light_head_down, 0
	jz	@@e
	store_command	115
@@e:
	ret
light_head_down_funct	endp

; **************************************
light_p		proc
	mov	light2_x_pos, 34
	mov	light2_y_pos, 0
	mov	light2_angle, 192+16+8-4-4
	call	light_letter
	mov	light_pause, 1
	ret
light_p		endp

; **************************************
light_o		proc
	mov	light2_x_pos, 50
	mov	light2_y_pos, 99
	mov	light2_angle, 128-16-8
	call	light_letter
	mov	light_pause, 1
	ret
light_o		endp

; **************************************
light_i		proc
	mov	light2_x_pos, 60
	mov	light2_y_pos, 0
	mov	light2_angle, 192+16+16+8
	call	light_letter
	mov	light_pause, 1
	ret
light_i		endp

; **************************************
light_s		proc
	mov	light2_x_pos, 90
	mov	light2_y_pos, 0
	mov	light2_angle, 192+16+16
	call	light_letter
	mov	light_pause, 1
	ret
light_s		endp

; **************************************
light_o2	proc
	mov	light2_x_pos, 120
	mov	light2_y_pos, 99
	mov	light2_angle, 128-16-16
	call	light_letter
	mov	light_pause, 1
	ret
light_o2	endp

; **************************************
light_n		proc
	mov	light2_x_pos, 125
	mov	light2_y_pos, 0
	mov	light2_angle, 192+32+16
	call	light_letter
	mov	light_pause, 1
	ret
light_n		endp

; **************************************

light_letter	proc
	call	clear_temp_seg1
	mov	light2_light, 32768
	call	light_pic2
	new_screen
	ret
light_letter	endp

light_flash	proc
	call	clear_temp_seg1
;	mov	ax, 20
;	call	random
;	add	al, 70
;	mov	light2_x_pos, al
	mov	light2_x_pos, 80
;	mov	ax, 10
;	call	random
;	add	al, 45
;	mov	light2_y_pos, al
	mov	light2_y_pos, 50
	mov	light2_light, 0
	call	light_pic2
	mov	light_pause, 1
	store_palette	grey_inv_pal, 0, 64
	new_screen
	ret
light_flash	endp

; **************************************
light_unpause	proc
;	mov	light_light, 16384
	mov	light_pause, 0
	store_palette	grey_pal, 0, 64
	ret
light_unpause	endp

; **************************************

count_distances	proc
	mov	fs, _temp_seg5
	xor	di, di
	mov	bh, -128
@@l2:
	mov	bl, -128
@@l1:
	mov	al, bl
	or	al, al
	jns	@@j1
	neg	al
@@j1:
	mul	al
	mov	cx, ax
	mov	al, bh
	or	al, al
	jns	@@j2
	neg	al
@@j2:
	mul	al
	add	ax, cx
	call	sqrt
	mov	fs:[di], al
	inc	di
	inc	bl
	cmp	bl, 128
	jnz	@@l1
	inc	bh
	cmp	bh, 128
	jnz	@@l2
	ret
count_distances	endp

count_angles	proc
	mov	fs, _temp_seg5
	mov	gs, _temp_seg6
	xor	di, di
	mov	bh, -128
@@l2:
	mov	bl, -128
@@l1:
	xor	eax, eax ; al, al
	or	bx, bx
	jz	@@j1
	push	bx
	or	bl, bl
	jns	@@jj1
	neg	bl
	or	ah, 1
@@jj1:
	or	bh, bh
	jns	@@jj2
	neg	bh
	or	ah, 2
@@jj2:
	push	ax
	call	angle
	pop	cx
	pop	bx

	cmp	ch, 0
	jz	@@j1
	cmp	ch, 1
	jz	@@a1
	cmp	ch, 2
	jz	@@a2
	add	al, 64*2
	jmp	@@j1
@@a1:
	mov	ah, 63+64
	sub	ah, al
	mov	al, ah
	jmp	@@j1
@@a2:
	mov	ah, 255 ; 127
	sub	ah, al
	mov	al, ah
@@j1:
	mov	gs:[di], al
	inc	di
	inc	bl
	cmp	bl, 128
	jnz	@@l1
	inc	bh

	test	bh, 00011111b
	jnz	@@pw1
	call	poison_wait
@@pw1:

	cmp	bh, 128
	jnz	@@l2
	ret
count_angles	endp

angle	proc
	mov	si, offset light_sin
	xor	ecx, ecx
	mov	al, bl
	mul	al
	mov	cx, ax
	mov	al, bh
	mul	al
	add	ecx, eax
	shl	eax, 16
	xor	edx, edx
	div	ecx
	cmp	eax, 010000h
	jz	@@_90
	xor	cl, cl
	mov	bp, ax
@@j3:
	mov	ax, [si]
	mul	ax
	mov	ax, dx
	cmp	ax, bp
	jae	@@j2
	add	si, 2 ; 4
	inc	cl
	cmp	cl, 64 ; 32
	jnz	@@j3
	dec	cl
@@j2:
	mov	al, cl
	ret
@@_90:
	mov	al, 63
	ret
angle	endp

light_pic	proc
	mov	fs, _temp_seg5
	mov	gs, _temp_seg6
	mov	es, _temp_seg1

	xor	di, di

	mov	si, light_picoff

	mov	ax, light_light
	mov	[light_decode1], ax
	xor	ah, ah
	mov	al, light_angle
	mov	bp, ax

	mov	ds, _temp_seg4

	mov	ch, 0
	mov	dh, cs:light_y_pos
	neg	dh
	add	dh, 128
@@l2:
	mov	cl, 0
	mov	dl, cs:light_x_pos
	neg	dl
	add	dl, 128
@@l1:
	xor	ah, ah

;	mov	bl, cl
;	sub	bl, dl
;	add	bl, 128
;	mov	bh, ch
;	sub	bh, dh
;	add	bh, 128
	mov	bx, dx

	mov	al, gs:[bx]
;	add	al, ah
	add	ax, bp
	mov	bh, fs:[bx]
	shr	bh, 2
	mov	bl, al

;	mov	bl, ds:[bx+???]
	db 3eh, 8ah, 9fh
light_decode1	dw 0
	xor	bh, bh
	shl	bx, 4

	mov	ax, gs
	sub	ax, 3000h
	mov	gs, ax
	or	bl, gs:[si]
	mov	al, ds:[bx+32768+16384]
	mov	es:[di], al

	and	bl, 11110000b
	or	bl, gs:[si+1]
	mov	al, ds:[bx+32768+16384]
	mov	es:[di+1], al

	and	bl, 11110000b
	or	bl, gs:[si+320]
	mov	al, ds:[bx+32768+16384]
	mov	es:[di+320], al

	and	bl, 11110000b
	or	bl, gs:[si+320+1]
	mov	al, ds:[bx+32768+16384]
	mov	es:[di+320+1], al

	mov	ax, gs
	add	ax, 3000h
	mov	gs, ax

	add	si, 2
	add	di, 2

	inc	dl
	inc	cl
	cmp	cl, 160
	jnz	@@l1
	add	di, 320
	add	si, 320
	inc	dh
	inc	ch
	cmp	ch, 100
	jnz	@@l2

	mov	ax, cs
	mov	ds, ax

	ret
light_pic	endp

light_pic2	proc
	mov	fs, _temp_seg5
	mov	gs, _temp_seg6
	mov	es, _temp_seg1

	xor	di, di
;	mov	dl, light2_x_pos
;	mov	dh, light2_y_pos

	mov	ax, light2_light
	mov	[light_decode11], ax
	xor	ah, ah
	mov	al, light2_angle
	mov	bp, ax

	mov	ds, _temp_seg4

	mov	ch, 0
	mov	dh, cs:light2_y_pos
	neg	dh
	add	dh, 128
@@l2:
	mov	cl, 0
	mov	dl, cs:light2_x_pos
	neg	dl
	add	dl, 128
@@l1:
	xor	ah, ah

;	mov	bl, cl
;	sub	bl, dl
;	add	bl, 128
;	mov	bh, ch
;	sub	bh, dh
;	add	bh, 128
	mov	bx, dx

	mov	al, gs:[bx]
	add	ax, bp
;	add	al, ah
	mov	bh, fs:[bx]
	shr	bh, 2
	mov	bl, al

;	mov	bl, ds:[bx+???]
	db 3eh, 8ah, 9fh
light_decode11	dw 0
	xor	bh, bh
	shl	bx, 4

	mov	ax, gs
	sub	ax, 3000h
	mov	gs, ax

	or	bl, gs:[di]
	mov	al, ds:[bx+32768+16384]
	add	es:[di], al

	and	bl, 11110000b
	or	bl, gs:[di+1]
	mov	al, ds:[bx+32768+16384]
	add	es:[di+1], al

	and	bl, 11110000b
	or	bl, gs:[di+320]
	mov	al, ds:[bx+32768+16384]
	add	es:[di+320], al

	and	bl, 11110000b
	or	bl, gs:[di+320+1]
	mov	al, ds:[bx+32768+16384]
	add	es:[di+320+1], al

	mov	ax, gs
	add	ax, 3000h
	mov	gs, ax

	add	di, 2

	inc	dl
	inc	cl
	cmp	cl, 160
	jnz	@@l1
	add	di, 320
	inc	dh
	inc	ch
	cmp	ch, 100
	jnz	@@l2

	mov	ax, cs
	mov	ds, ax

	ret
light_pic2	endp

;light_ss	dw 0
;light_sp	dw 0

light_dist_count	proc
	mov	di, offset light_dist
	mov	ax, (3eh shl 8)
	xor	dx, dx
	mov	bx, 40
	div	bx
	mov	dx, ax
	mov	bx, 3e00h
	mov	cx, 40
@@l1:
	mov	[di], bh
	inc	di
	sub	bx, dx
	loop	@@l1
	ret
light_dist_count	endp

light_up	proc
	mov	si, offset light_fade
	mov	cx, 32
@@l1:
	mov	al, [si]
	mov	fs:[di], al
	inc	si
	inc	di
	loop	@@l1
	ret
light_up	endp

light_down	proc
	mov	si, offset light_fade+31
	mov	cx, 32
@@l1:
	mov	al, [si]
	mov	fs:[di], al
	dec	si
	inc	di
	loop	@@l1
	ret
light_down	endp

light_zero	proc
	mov	cx, 64/4
	xor	eax, eax
@@l1:
	mov	fs:[di], eax
	add	di, 4
	loop	@@l1
	ret
light_zero	endp

light_count	proc
	mov	bx, offset light_dist
	mov	si, di
	sub	si, 256
	mov	cx, 39
@@l1:
	push	cx
	mov	cx, 256
@@l2:
	mov	al, fs:[si]
	mov	ah, ds:[bx]
	mul	ah
	shr	ax, 6
	mov	fs:[di], al
	inc	si
	inc	di
	loop	@@l2
	pop	cx
	sub	si, 256
	inc	bx
	loop	@@l1
	ret
light_count	endp

light_shade	proc
	mov	si, offset convert_poison2_data2
	mov	cx, 12
@@ll1:
	mov	al, 63
	sub	al, [si]
	mov	ah, 3
	mul	ah
	mov	bl, 10
	div	bl
	add	[si], al
	inc	si
	loop	@@ll1

	xor	dl, dl
	mov	di, 32768+16384
@@l1:
	mov	si, offset convert_poison2_data2
	mov	cx, 12
@@l2:
	mov	al, [si]
	mul	dl
	shr	ax, 6
	mov	fs:[di], al
	inc	di
	inc	si
	loop	@@l2
	add	di, 16-12
	inc	dl
	cmp	dl, 64
	jnz	@@l1
	ret
light_shade	endp

light_tabs	proc
	call	light_dist_count
	mov	fs, _temp_seg4

	xor	di, di
	call	light_up
	call	light_down
	call	light_up
	call	light_down
	call	light_up
	call	light_down
	call	light_up
	call	light_down
	call	light_count

	mov	di, 16384
	call	light_up
	call	light_down
	call	light_zero
	call	light_up
	call	light_down
	call	light_zero
	call	light_count

	mov	di, 32768
	call	light_up
	call	light_down
	call	light_zero
	call	light_zero
	call	light_zero
	call	light_count

	call	light_shade

	ret

light_tabs	endp

light_sin:
dw 0,1608,3215,4821,6423,8022,9615,11203,12785,14358,15923,17479,19023,20557,22078,23585,25079,26557,28019,29465,30892,32302,33691,35061,36409,37735,39039,40319,41574,42805,44010,45189,46340,47463,48558,49623,50659,51664,52638,53580,54490,55367,56211,57021,57796,58537,59242,59912,60546,61143
dw 61704,62227,62713,63161,63570,63942,64275,64570,64825,65042,65219,65357,65456,65515

light_fade:	;	db 32 DUP (0)
	db 00,01,02,03,04,05,06,07
	db 08,09,10,11,13,16,18,20
	db 21,27,33,38,43,47,51,54
	db 57,59,61,62,62,63,63,63
light_dist	db 40 DUP (0)

convert_poison2		proc
	call	clear_temp_seg1
	call	clear_temp_seg3
	mov	es, _temp_seg1
	call	drawobject c, offset obj_poi, 0, 0, 63
	call	drawobject c, offset obj_son, 160, 0, 63
	mov	fs, _temp_seg1
	mov	gs, _temp_seg3
	mov	si, offset soften_poison2
	call	soften

	mov	fs, _temp_seg3
	mov	si, offset convert_poison2_data
	xor	di, di
	mov	cx, 64000
	xor	bh, bh
@@l1:
	mov	al, fs:[di]
	xor	bl, bl
@@j2:
	cmp	al, ds:[si+bx]
	jz	@@j1
	inc	bx
	cmp	bx, 11
	jnz	@@j2
@@j1:
	mov	fs:[di], bl
	inc	di
	dec	cx
	jnz	@@l1
	ret

ll	EQU 63
convert_poison2_data	db 0, 7, 0eh, 0fh, 16h, 17h, 1eh, 26h, 2eh, 36h, 3eh, 0
;convert_poison2_data2	db 63, ll-7, ll-0eh, ll-0fh, ll-16h, ll-17h+1, ll-1eh+3, ll-26h+5, ll-2eh+10, ll-36h+15, ll-3eh+20, ll-0
convert_poison2_data2	db 63, ll-7, ll-0eh, ll-0fh, ll-16h, ll-17h, ll-1eh, ll-26h, ll-2eh, ll-36h, ll-3eh, ll-0
convert_poison2		endp

light_x_pos	db 120
light_y_pos	db 50
light_angle	db 0
light_light	dw 16384
light_y_add	db 0
light_picoff	dw 0
light2_x_pos	db 120
light2_y_pos	db 50
light2_angle	db 0
light2_light	dw 16384

light_move_sin:
	db 0,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,17,18,19,20,21,22,23,23,24,25,26,26,27,28,28,29,30,30,31,32,32,33,33,34,34,35,35,36,36,36,37
	db 37,37,38,38,38,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,38,38,38,37,37,37,36,36,36,35,35,34,34,33,33,32,32,31,30,30,29,28,28,27,26,26
	db 25,24,23,23,22,21,20,19,18,17,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0,0,0,255,254,253,252,251,250,249,248,247,246,245,244,243,242,241,240,239,239,238,237
	db 236,235,234,233,233,232,231,230,230,229,228,228,227,226,226,225,224,224,223,223,222,222,221,221,220,220,220,219,219,219,218,218,218,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217
	db 217,217,218,218,218,219,219,219,220,220,220,221,221,222,222,223,223,224,224,225,226,226,227,228,228,229,230,230,231,232,233,233,234,235,236,237,238,239,239,240,241,242,243,244,245,246,247,248,249,250
	db 251,252,253,254,255,0,0,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,17,18,19,20,21,22,23,23,24,25,26,26,27,28,28,29,30,30,31,32,32,33,33,34,34
	db 35,35,36,36,36,37,37,37,38,38,38,39,39,39,39,39,39,39,39,39