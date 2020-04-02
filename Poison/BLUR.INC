blur_len	EQU 1000
blur_frames	EQU 300

blur_toggle1	proc
	call	blur_init
	store_command	141
	ret
blur_toggle1	endp

blur_toggle2	proc
	call	blur_init
	mov	rot_distance, 176
	store_command	151
	ret
blur_toggle2	endp

blur_init	proc
	xor	edx, edx
	mov	ax, blur_len
	shl	eax, 16
	xor	ebx, ebx
	mov	bx, blur_frames
	div	ebx
	mov	blur_speed, eax
	reset_counter

	call	clear_temp_seg1
	mov	es, _temp_seg1
	call	drawobject c, offset obj_sign7, 0, 0, 63
	call	clear_temp_seg2
	mov	fs, _temp_seg1
	mov	gs, _temp_seg2
	mov	si, offset soften_intro_signs1
	call	soften
	call	clear_temp_seg6
	mov	fs, _temp_seg2
	mov	gs, _temp_seg6
	mov	si, 0
	mov	di, 27*256+27
	mov	cx, 200
	mov	dx, 200
	call	conv320to256

	mov	ax, _temp_seg2
	mov	[blur_adr], ax
	mov	ax, _temp_seg3
	mov	[blur_adr+2], ax
	mov	ax, _temp_seg4
	mov	[blur_adr+4], ax

	call	clear_temp_seg2
	call	clear_temp_seg3
	call	clear_temp_seg4

	mov	rot_angle, 0
	mov	rot_funct, offset rotator_normal
	mov	rot_x_mid, 160
	mov	rot_y_mid, 100

	mov	blur_stop, 0
	mov	blur_lframe, -1

	store_palette	grey_pal, 0, 256
	ret
blur_init	endp

blur_progress1	proc
	cmp	blur_stop, 1
	jz	@@ee
	get_counter
	xor	edx, edx
	shl	eax, 16
	mov	ebx, blur_speed
	div	ebx
	cmp	ax, blur_lframe
	jz	@@e

	mov	blur_frame, ax
	mov	ax, blur_lframe
@@l1:
	inc	ax
	push	ax

	inc	byte ptr rot_angle

	call	blur_distance
	mov	cx, 64
@@cl1:
	call	blur_clocks_down
	loop	@@cl1
	pop	ax
	push	ax
	and	ax, 101b
	jnz	@@t1
	call	plasma_move_pump
@@t1:
	pop	ax
	cmp	ax, blur_frame
	jnz	@@l1
	mov	ax, blur_frame
	mov	blur_lframe, ax

	mov	fs, _temp_seg6
	mov	gs, [blur_adr]
	call	rotator
	call	blur

	cmp	blur_triton, 0
	jz	@@j1
	call	do_blur_shake
@@j1:
	call	plasma_show_pump

	new_screen

	mov	bx, [blur_adr+4]
	mov	eax, dword ptr [blur_adr]
	mov	dword ptr [blur_adr+2], eax
	mov	[blur_adr], bx

	cmp	blur_shift_pos, 0
	jz	@@noshift
	mov	bl, blur_shift_pos
	mov	fs, _temp_seg1
	call	blur_shift
	dec	blur_shift_pos
@@noshift:

@@e:
	store_command	141
@@ee:
	ret
blur_progress1	endp

blur_progress2	proc
	cmp	blur_stop, 1
	jz	@@ee

	get_counter
	xor	edx, edx
	shl	eax, 16
	mov	ebx, blur_speed
	div	ebx
	cmp	ax, blur_lframe
	jz	@@e

	mov	blur_frame, ax
	mov	ax, blur_lframe
@@l1:
	inc	ax
	push	ax

	inc	byte ptr rot_angle
	call	blur_distance
	cmp	blur_claps, 0
	jz	@@noc
	call	blur_lines_move
@@noc:

	pop	ax
	cmp	ax, blur_frame
	jnz	@@l1
	mov	ax, blur_frame
	mov	blur_lframe, ax

	cmp	blur_drum, 0
	jz	@@nodrum
	call	clear_temp_seg1
	mov	es, _temp_seg1
	call	blur_rotate_vertex
	call	blur_vertex
	call	blur_vertex
	call	blur_rotate_vertex
	call	blur_vertex
	mov	ax, _temp_seg1
	mov	prepared_screen_segment, ax
	new_screen
	jmp	@@e
@@nodrum:

	cmp	rot_ver_start_mul, 1
	jz	@@one
	dec	rot_ver_start_mul
@@one:
	mov	bx, [blur_adr+4]
	mov	eax, dword ptr [blur_adr]
	mov	dword ptr [blur_adr+2], eax
	mov	[blur_adr], bx

	mov	al, rot_angle
	mov	blur_angle, al
	mov	fs, _temp_seg6
	mov	gs, [blur_adr]
	call	rotator
	mov	prepared_screen_segment, gs

	new_screen

@@e:
	cmp	blur_claps, 0
	jz	@@noc2
	call	blur_lines_show
@@noc2:
	store_command	151
	xor	blur_drum, 1
@@ee:
	ret
blur_progress2	endp

blur_speed	dd 0
blur_frame	dw 0
blur_lframe	dw -1
blur_adr	dw 3 DUP (0)
blur_stop	db 0
blur_triton	db 0
blur_shake_count	db 0
blur_deuxton_flash	db 0
blur_shift_pos	db 32
blur_drum	db 0
blur_claps	db 0
blur_angle	db 0

blur_part2_claps	proc
	mov	rot_ver_start_mul, 16
	mov	blur_claps, 1
	ret
blur_part2_claps	endp

blur_do_stop	proc
	mov	ax, _temp_seg1
	mov	prepared_screen_segment, ax
	mov	blur_stop, 1
	call	blur_mosaic
	ret
blur_do_stop	endp

blur_normal_pal	proc
	store_palette	grey_pal, 0, 256
	ret
blur_normal_pal	endp

blur_plasma_pal	proc
	store_palette	plasma_pal, 0, 256
	ret
blur_plasma_pal	endp

blur_plasma_pal_flash	proc
	store_palette	plasma_pal, 0, 256
	store_command	145
	ret
blur_plasma_pal_flash	endp

blur_deuxton_flash_toggle	proc
	xor	blur_deuxton_flash, 1
	ret
blur_deuxton_flash_toggle	endp

do_blur_shake	proc
	cmp	blur_shake_count, 11
	ja	@@j1
	xor	bh, bh
	mov	bl, blur_shake_count
	shl	bx, 2
	add	bx, offset blur_shake_data
	mov	cx, ds:[bx]
	call	blur_shake
	mov	cx, ds:[bx+2]
	call	cx
	test	blur_shake_count, 10b
	jnz	@@j2
	call	gener_xor_crash
	mov	si, offset xor_crash_data
	call	do_xor_crash
@@j2:
	call	blur_lines_kill
	inc	blur_shake_count
	ret
@@j1:
	mov	[prepared_screen_offset_write], 0
	cmp	blur_deuxton_flash, 0
	jz	@@f1
	call	do_blur_deuxton
;	mov	blur_deuxton_flash, 0
	ret
@@f1:
	call	do_blur_triton
	call	blur_show_clocks
	ret
do_blur_shake	endp

blur_shake_data:
	dw 16, offset do_blur_deuxton
	dw 32, offset do_blur_noton
	dw 48, offset do_blur_deuxton
	dw 64, offset do_blur_triton

	dw 80, offset do_blur_deuxton
	dw 96, offset do_blur_noton
	dw 80, offset do_blur_deuxton
	dw 64, offset do_blur_triton

	dw 48, offset do_blur_deuxton
	dw 32, offset do_blur_noton
	dw 16, offset do_blur_deuxton
	dw 08, offset do_blur_triton

blur_shake	proc
;	mov	cx, 32 ;99

	mov	ax, cx
	shl	ax, 1
	call	random
	sub	ax, cx
	mov	dx, 320
	imul	dx
	mov	dx, ax

	mov	ax, cx
	shr	ax, 1
	call	random
	shl	ax, 2
	sub	ax, cx
	add	dx, ax

	mov	[prepared_screen_offset_write], dx

	ret
blur_shake	endp

blur_lines_kill	proc
	mov	fs, _temp_seg1
	mov	ax, 20
	call	random
	add	ax, 10
	mov	cx, ax
@@l1:
	mov	ax, 200
	call	random
	mov	dx, 320
	mul	dx
	mov	di, ax
;	xor	eax, eax
	mov	eax, 010101010h
	mov	bx, 320/4
@@j1:
	xor	fs:[di], eax
	and	fs:[di], 0c0c0c0c0h
	add	di, 4
	dec	bx
	jnz	@@j1
	loop	@@l1
	ret
blur_lines_kill	endp

do_blur_triton	proc
	mov	fs, _temp_seg1
	xor	si, si
	xor	di, di

	mov	bx, 200
@@l2:
	mov	cx, 106
@@l1:
	mov	al, fs:[si]
	mov	fs:[di], al
	add	si, 3
	inc	di
	dec	cx
	jnz	@@l1
	add	si, 320-106*3
	add	di, 320-106
	dec	bx
	jnz	@@l2

	xor	si, si
	mov	bx, 200
@@l4:
	mov	cx, 104/4
@@l3:
	mov	eax, fs:[si]
	mov	fs:[si+104], eax
	mov	fs:[si+104+104], eax
	add	si, 4
	dec	cx
	jnz	@@l3
	add	si, 320-104
	dec	bx
	jnz	@@l4
	ret
do_blur_triton	endp

do_blur_deuxton	proc
	mov	fs, _temp_seg1
	xor	si, si
	xor	di, di

	mov	bx, 200
@@l2:
	mov	cx, 160
@@l1:
	mov	al, fs:[si]
	mov	fs:[di], al
	add	si, 2
	inc	di
	dec	cx
	jnz	@@l1
;	add	si, 320-160*2
	add	di, 320-160
	dec	bx
	jnz	@@l2

	xor	si, si
	mov	bx, 200
@@l4:
	mov	cx, 160/4
@@l3:
	mov	eax, fs:[si]
	mov	fs:[si+160], eax
	add	si, 4
	dec	cx
	jnz	@@l3
	add	si, 320-160
	dec	bx
	jnz	@@l4
	ret
do_blur_deuxton	endp

do_blur_noton	proc
	ret
do_blur_noton	endp

blur_triton_toggle	proc
	xor	blur_triton, 1
	ret
blur_triton_toggle	endp

blur_distance	proc
	shr	ax, 1
	cmp	ax, 256
	jb	@@j1
	mov	rot_distance, 176
	ret
@@j1:
	mov	si, offset blur_distance_sin
	mov	bx, ax
	mov	bl, [si+bx]
	xor	bh, bh
	mov	rot_distance, bx
	ret
blur_distance	endp

conv320to256	proc
	mov	cx, 200
@@l1:
	push	si di cx
	shr	cx, 2
@@l2:
	mov	eax, fs:[si]
	mov	gs:[di], eax
	add	si, 4
	add	di, 4
	dec	cx
	jnz	@@l2
	pop	cx di si
	add	si, 320
	add	di, 256
	dec	dx
	jnz	@@l1
	ret
conv320to256	endp

blur	proc
	mov	gs, _temp_seg1
	mov	fs, [blur_adr]
	call	blur_copy_it
	mov	fs, [blur_adr+2]
	mov	cl, 1
	call	blur_it
	mov	fs, [blur_adr+4]
	mov	cl, 2
	call	blur_it
;	mov	fs, [blur_adr+6]
;	mov	cl, 3
;	call	blur_it

	ret
blur	endp

blur_it	proc
	xor si, si
	mov bx, 64000
@@l1:
	mov	al, fs:[si]
	shr	al, cl
	add	gs:[si], al
@@j1:
	inc	si
	dec	bx
	jnz	@@l1
	ret

blur_it	endp

blur_copy_it	proc
	xor	si, si
	mov	cx, 64000/4
@@l1:
	mov	eax, fs:[si]
	mov	gs:[si], eax
	add	si, 4
	dec	cx
	jnz	@@l1
	ret
blur_copy_it	endp

rotator		proc
	mov	cx, 256+64
	mov	si, offset rotate_sin
	mov	di, offset rotator_sin
	xor	ebx, ebx
	mov	bx, [rot_distance]
@@l3:
	mov	ax, [si]
	movsx	eax, ax
	mul	ebx
	shr	eax, 8
	mov	[di], ax
	add	si, 2
	add	di, 2
	loop	@@l3

	mov	bl, rot_angle
	xor	bh, bh
	shl	bx, 1

	mov	ax, word ptr [rotator_sin+bx]
	mov	[rot_hor_pos], ax
	mov	ax, word ptr [rotator_sin+bx+128]
	mov	[rot_ver_pos], ax

	add	bx, 64*2
	and	bx, 511
	mov	ax, word ptr [rotator_sin+bx]
	mov	[rot_hor_pos2], ax
	mov	ax, word ptr [rotator_sin+bx+128]
	mov	[rot_ver_pos2], ax

	add	bx, 64*2
	and	bx, 511
; - 1
	mov	ax, word ptr [rotator_sin+bx]
	mov	cx, rot_x_mid
	imul	cx
	mov	[rot_hor_start], ax
; -
	mov	ax, word ptr [rotator_sin+bx+128]
	mov	cx, rot_x_mid
	imul	cx
	mov	[rot_ver_start], ax

	add	bx, 64*2
	and	bx, 511
; - 2
	mov	ax, word ptr [rotator_sin+bx]
	mov	cx, rot_y_mid
	imul	cx
	add	[rot_hor_start], ax
; -
	mov	ax, word ptr [rotator_sin+bx+128]
	mov	cx, rot_y_mid
	imul	cx
	add	[rot_ver_start], ax

	mov	ax, rot_x_pos
	add	[rot_hor_start], ax
	mov	dx, rot_y_pos
	add	[rot_ver_start], dx

;	mov	cx, 6
;	mov	si, offset rot_hor_pos
;	xor	ebx, ebx
;	mov	bx, [rot_distance]
;@@l3:
;	mov	ax, [si]
;	movsx	eax, ax
;	imul	ebx
;	shr	eax, 8
;	mov	[si], ax
;	add	si, 2
;	loop	@@l3

	mov	ax, rot_funct
	call	ax

	ret
rotator		endp

rotator_normal	proc
	xor	di, di

	mov	dx, [rot_ver_start]
	mov	ax, rot_ver_start_mul
	imul	dx
	mov	dx, ax

	mov	ax, [rot_hor_start]

	mov	bp, rot_hor_pos
	mov	si, rot_ver_pos

	mov	cx, 200
@@l2:
	push	ax cx dx
	mov	cx, 320
@@l1:
	mov	bl, ah
	mov	bh, dh
	mov	bl, fs:[bx]
	mov	gs:[di], bl
	add	ax, bp
	add	dx, si
	inc	di
	dec	cx
	jnz	@@l1
	pop	dx cx ax
	add	ax, [rot_hor_pos2]
	add	dx, [rot_ver_pos2]
	dec	cx
	jnz	@@l2
	ret
rotator_normal	endp

rotator_tunel	proc
	mov	di, 35*320
	mov	es, _temp_seg5

	mov	al, tunel_rotator_color
	mov	byte ptr [@@tunel_write_decode], al

	xor	dx, dx
	mov	ax, 140 shl 8
	mov	bx, rot_distance
	div	bx
	shr	ax, 1
	cmp	ax, 256
	jb	@@b1
	mov	al, 255
@@b1:
	mov	byte ptr [@@tunel_cmp_decode], al

	mov	ax, [rot_hor_start]
	mov	dx, [rot_ver_start]

	mov	bp, rot_hor_pos
	mov	si, rot_ver_pos

	mov	bx, rot_y_mid
	shr	bx, 1
	neg	bl
	add	bl, 128
	mov	byte ptr [@@tunel_decode+1], bl
	mov	cx, 130
@@l2:
	push	ax cx dx
	mov	bx, rot_x_mid
	shr	bx, 1
	neg	bl
	add	bl, 128
	mov	byte ptr [@@tunel_decode], bl
	jmp	$+2
	mov	cx, 320
@@l1:
	cmp	byte ptr gs:[di], 0
	jnz	@@nowrite

	db	0bbh	; mov bx, ...
@@tunel_decode:
	dw	0
	mov	bl, es:[bx]
	db	080h, 0fbh	; cmp bl, ...
@@tunel_cmp_decode:
	db	0
	ja	@@write

	mov	bl, ah
	mov	bh, dh
	mov	bl, fs:[bx]
	or	bl, bl
	jnz	@@nowrite

@@write:
	db 65h, 0c6h, 05h	; move byte ptr gs:[di], ...
@@tunel_write_decode:
	db 0
@@nowrite:

	add	ax, bp
	add	dx, si
	inc	di
	test	cl, 1
	jz	@@t1
	inc	byte ptr [@@tunel_decode]
@@t1:
	dec	cx
	jnz	@@l1
	pop	dx cx ax
	add	ax, [rot_hor_pos2]
	add	dx, [rot_ver_pos2]
	test	cl, 1
	jz	@@t2
	inc	byte ptr [@@tunel_decode+1]
@@t2:
	dec	cx
	jnz	@@l2
	ret
rotator_tunel	endp

rotator_tunel2	proc
	mov	di, 35*320
	mov	es, _temp_seg5

	mov	al, tunel_rotator_color
	mov	byte ptr [@@tunel_write_decode], al

	xor	dx, dx
	mov	ax, 140 shl 8
	mov	bx, rot_distance
	div	bx
	shr	ax, 1
	cmp	ax, 256
	jb	@@b1
	mov	al, 255
@@b1:
	mov	byte ptr [@@tunel_cmp_decode], al

	mov	ax, [rot_hor_start]
	mov	dx, [rot_ver_start]

	mov	bp, rot_hor_pos
	mov	si, rot_ver_pos

	mov	bx, rot_y_mid
	shr	bx, 1
	neg	bl
	add	bl, 128
	mov	byte ptr [@@tunel_decode+1], bl
	mov	cx, 130
@@l2:
	push	ax cx dx
	mov	bx, rot_x_mid
	shr	bx, 1
	neg	bl
	add	bl, 128
	mov	byte ptr [@@tunel_decode], bl
	jmp	$+2
	mov	cx, 320
@@l1:
	db	0bbh	; mov bx, ...
@@tunel_decode:
	dw	0
	mov	bl, es:[bx]
	db	080h, 0fbh	; cmp bl, ...
@@tunel_cmp_decode:
	db	0
	ja	@@write

	mov	bl, ah
	mov	bh, dh
	mov	bl, fs:[bx]
	or	bl, bl
	jnz	@@write0

@@write:
	db 65h, 0c6h, 05h	; move byte ptr gs:[di], ...
@@tunel_write_decode:
	db 0
	jmp	@@lab
@@write0:
	mov	byte ptr gs:[di], 0
@@lab:

	add	ax, bp
	add	dx, si
	inc	di
	test	cl, 1
	jz	@@t1
	inc	byte ptr [@@tunel_decode]
@@t1:
	dec	cx
	jnz	@@l1
	pop	dx cx ax
	add	ax, [rot_hor_pos2]
	add	dx, [rot_ver_pos2]
	test	cl, 1
	jz	@@t2
	inc	byte ptr [@@tunel_decode+1]
@@t2:
	dec	cx
	jnz	@@l2
	ret
rotator_tunel2	endp

rot_hor_pos	dw 0
rot_ver_pos	dw 0
rot_hor_pos2	dw 0
rot_ver_pos2	dw 0
rot_hor_start	dw 0
rot_ver_start	dw 0
rot_x_pos	dw -128*256
rot_y_pos	dw -128*256
rot_x_mid	dw 160
rot_y_mid	dw 100-50
rot_angle	db 64+48
rot_distance	dw 300h ; 192-16
rot_funct	dw offset rotator_tunel
rot_ver_start_mul	dw 1

rotator_sin	dw 256+64 DUP (0)

blur_distance_sin:
	db 1,2,3,4,5,6,7,8,9,10,11,12,13,15,16,17,18,19,20,21,22,23,24,25,26,27,28,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,50,51,52,53
	db 54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,83,84,85,86,87,88,89,90,91,92,93,94,95,96,96,97,98,99,100,101
	db 102,103,104,104,105,106,107,108,109,110,110,111,112,113,114,115,115,116,117,118,119,119,120,121,122,123,123,124,125,126,126,127,128,129,129,130,131,132,132,133,134,134,135,136,137,137,138,139,139,140
	db 141,141,142,143,143,144,144,145,146,146,147,147,148,149,149,150,150,151,151,152,153,153,154,154,155,155,156,156,157,157,158,158,159,159,160,160,161,161,161,162,162,163,163,164,164,164,165,165,165,166
	db 166,167,167,167,168,168,168,169,169,169,170,170,170,170,171,171,171,171,172,172,172,172,173,173,173,173,174,174,174,174,174,174,175,175,175,175,175,175,175,176,176,176,176,176,176,176,176,176,176,176
	db 176,176,176,176,176,176

blur_show_clock	proc
	push	cx si di
	mov	si, offset blur_clocks_pic
	mov	al, 80
	mul	ah
	add	si, ax
	mov	cx, 10
@@l1:
	mov	eax, [si]
	add	fs:[di], eax
	mov	eax, [si+4]
	add	fs:[di+4], eax
;	mov	byte ptr fs:[di+8], 0
	add	si, 8
	add	di, 320
	loop	@@l1
	pop	di si cx
	ret
blur_show_clock	endp

blur_show_clocks	proc
	mov	si, offset blur_clocks
	mov	di, 4*320 + 4

	mov	cx, 4
@@l1:
	mov	ah, [si]
	call	blur_show_clock
	add	di, 9
	mov	ah, [si+1]
	call	blur_show_clock
	add	di, 9
	cmp	cl, 1
	jz	@@j1
	mov	ah, 10
	call	blur_show_clock
	add	di, 9
	add	si, 2
@@j1:
	loop	@@l1

	ret
blur_show_clocks	endp

blur_clocks_down	proc
	mov	si, offset blur_clocks+7
@@j2:
	dec	byte ptr [si]
	jns	@@j1
	mov	byte ptr [si], 9
	dec	si
	jmp	@@j2
@@j1:
	ret
blur_clocks_down	endp

blur_clocks_pic:
db 0,0,0,0,0,0,0,8,0,0,0,0,0,0,12,61,0,0,0,0,0,12,55,12,0,0,0,0,12,55,12,0,36,0,0,32,61,12,0,0,40,52,55,44,8,0,0,0,44,59
db 55,12,0,0,0,0,32,0,20,61,32,0,0,0,0,0,0,8,44,55,16,0,0,0,0,0,0,16,55,44,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
db 0,0,0,0,0,0,0,32,63,63,63,0,0,16,55,44,8,0,63,0,44,55,16,0,0,8,63,0,32,0,0,0,0,40,32,0,0,0,0,0,8,59,4,0,0,0,0,0,40,32
db 0,0,0,0,0,4,63,4,0,0,0,0,63,4,0,0,0,0,0,0,40,28,0,0,0,0,0,0,16,55,0,0,0,0,0,0,0,55,16,0,0,0,0,0,0,28,40,0,0,0
db 0,0,0,4,63,8,12,48,0,0,0,0,36,55,59,24,0,0,16,40,63,59,20,0,28,55,55,28,4,16,55,0,55,22,0,0,0,0,44,32,0,0,0,8,59,0,0,0,0,0
db 0,16,48,0,0,0,0,0,0,20,48,0,0,0,0,0,0,32,32,0,0,0,0,0,0,32,32,0,0,0,0,0,0,61,20,0,0,0,0,0,0,63,16,55,0,0,63,63,63,63
db 63,63,0,0,0,0,0,63,0,0,0,0,0,0,0,0,0,0,0,0,0,0,63,0,0,0,0,0,0,0,63,4,0,0,0,0,0,0,48,20,0,0,0,0,0,0,32,36,0,0
db 0,0,0,0,16,52,0,0,0,0,0,0,0,63,4,0,0,0,0,0,0,40,28,0,0,0,0,0,0,16,55,0,0,0,63,63,63,63,63,63,63,63,0,0,0,0,28,40,0,0
db 0,0,0,0,0,55,0,0,0,0,0,0,0,63,0,0,0,0,0,0,8,63,0,0,0,0,0,0,48,28,0,0,0,0,0,28,48,0,0,0,44,63,63,63,63,44,24,4,24,4
db 52,24,4,24,44,63,0,36,40,0,0,0,0,4,20,52,4,0,0,0,0,0,61,12,0,0,0,0,0,0,0,0,0,0,12,63,46,8,0,0,0,12,55,48,24,0,8,0,12,55
db 12,32,32,0,52,12,55,12,0,20,48,0,46,61,12,0,0,12,55,0,61,55,20,0,0,0,63,28,32,12,63,12,40,63,63,44,0,8,55,61,32,4,48,16,40,59,40,55,0,0
db 48,20,32,4,0,55,16,0,32,32,0,0,0,0,55,12,0,0,0,0,0,0,63,0,0,0,0,0,0,8,59,0,0,12,0,0,0,16,48,0,12,55,63,63,63,63,63,63,63,63
db 0,0,0,32,41,55,12,0,0,0,0,36,63,12,0,0,0,0,12,61,36,0,0,0,0,12,55,52,16,0,0,0,12,55,12,48,16,0,0,0,0,12,61,12,0,48,28,0,0,0
db 20,52,32,48,0,0,8,0,0,36,63,8,0,0,61,12,0,32,59,24,0,0,12,55,12,55,20,61,12,0,0,12,59,52,0,12,55,12,0,0,52,59,12,0,12,55,0,12,55,12
db 61,12,0,12,0,40,32,0,20,52,4,0,12,63,4,0,0,36,40,0,0,0,0,0,0,0,63,0,4,0,0,0,0,4,63,0,52,12,0,0,0,32,40,0,20,63,44,24,8,59
db 8,55,0,4,24,44,63,63,63,63,0,0,0,12,61,8,0,0,0,0,12,55,12,0,0,0,40,52,63,12,0,0,0,0,28,16,0,0,0,0,0,0,0,0,0,0,0,0,0,0
; :
db 0,0,0,63,0,0,0,0,0,0,0,63,0,0,0,0,0,0,0,63,0,0,0,0,0,0,0,63,8,0,0,0,0,0,0,32,40,0,0,0,0,0,0,4,61,0,0,0,0,0
db 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0

blur_clocks	db 1,2,3,4,5,6,7,8
blur_shift	proc
	xor	bh, bh
;	mov	fs, _temp_seg1

	xor	si, si
	mov	ch, 10
@@l3:
	mov	cl, 10
@@l2:
	mov	dx, 320
	add	dx, bx
@@l1:
	mov	al, fs:[si+bx]
	mov	fs:[si], al
	inc	si
	dec	dx
	jnz	@@l1
;	mov	dx, bx
;@@l4:
;	mov	byte ptr fs:[si], 0
;	inc	si
;	dec	dx
;	jnz	@@l4
	add	si, bx

	dec	cl
	jnz	@@l2
	add	si, 10*320
	dec	ch
	jnz	@@l3

	neg	bx
	mov	si, 63999
	mov	ch, 10
@@l33:
	mov	cl, 10
@@l22:
	mov	dx, 320
	sub	dx, bx
@@l11:
	mov	al, fs:[si+bx]
	mov	fs:[si], al
	dec	si
	dec	dx
	jnz	@@l11
;	mov	dx, bx
;	neg	dx
;@@l44:
;	mov	byte ptr fs:[si], 0
;	dec	si
;	dec	dx
;	jnz	@@l44
	add	si, bx

	dec	cl
	jnz	@@l22
	sub	si, 10*320
	dec	ch
	jnz	@@l33

	ret
blur_shift	endp

comment	%

blur_shift2	proc
	xor	bh, bh
	mov	fs, _temp_seg1

	xor	si, si
	mov	ch, 20
@@l3:
	mov	cl, 5
@@l2:
	mov	dx, 320
	sub	dx, bx
@@l1:
	mov	al, fs:[si+bx]
	mov	fs:[si], al
	inc	si
	dec	dx
	jnz	@@l1
	mov	dx, bx
@@l4:
	mov	byte ptr fs:[si], 0
	inc	si
	dec	dx
	jnz	@@l4

	dec	cl
	jnz	@@l2
	add	si, 5*320
	dec	ch
	jnz	@@l3

	neg	bx
	mov	si, 63999
	mov	ch, 20
@@l33:
	mov	cl, 5
@@l22:
	mov	dx, 320
	add	dx, bx
@@l11:
	mov	al, fs:[si+bx]
	mov	fs:[si], al
	dec	si
	dec	dx
	jnz	@@l11
	mov	dx, bx
	neg	dx
@@l44:
	mov	byte ptr fs:[si], 0
	dec	si
	dec	dx
	jnz	@@l44

	dec	cl
	jnz	@@l22
	sub	si, 5*320
	dec	ch
	jnz	@@l33

	ret
blur_shift2	endp

%

blur_vertex	proc
	mov	fs, _temp_seg5
	mov	di, offset sign7_faces
	xor	si, si

	mov	cx, 88
@@l1:
	push	cx

	xor	bh, bh
	mov	bl, [di]
	shl	bx, 2
	mov	ax, fs:[si+bx]
	call	blur_vertex_random
	mov	word ptr [blur_vertex_data], cx
	mov	word ptr [blur_vertex_data+20], dx
	mov	ax, fs:[si+bx+2]
	call	blur_vertex_random
	mov	word ptr [blur_vertex_data+2], cx
	mov	word ptr [blur_vertex_data+22], dx

	xor	bh, bh
	mov	bl, [di+1]
	shl	bx, 2
	mov	ax, fs:[si+bx]
	call	blur_vertex_random
	mov	word ptr [blur_vertex_data+4], cx
	mov	word ptr [blur_vertex_data+8], dx
	mov	ax, fs:[si+bx+2]
	call	blur_vertex_random
	mov	word ptr [blur_vertex_data+6], cx
	mov	word ptr [blur_vertex_data+10], dx

	xor	bh, bh
	mov	bl, [di+2]
	shl	bx, 2
	mov	ax, fs:[si+bx]
	call	blur_vertex_random
	mov	word ptr [blur_vertex_data+12], cx
	mov	word ptr [blur_vertex_data+16], dx
	mov	ax, fs:[si+bx+2]
	call	blur_vertex_random
	mov	word ptr [blur_vertex_data+14], cx
	mov	word ptr [blur_vertex_data+18], dx

	push	si cx
	mov	si, offset blur_vertex_data
	mov	cx, 3
	call	aaline
	pop	cx si

	add	di, 3
	pop	cx
	loop	@@l1

	ret
blur_vertex	endp
blur_vertex_data	dw 12 DUP (0)

blur_vertex_random	proc
	mov	cx, ax
	mov	dx, ax
	mov	ax, 7
	call	random
	sub	ax, 3
	add	cx, ax
	mov	ax, 7
	call	random
	sub	ax, 3
	add	dx, ax
	ret
blur_vertex_random	endp

blur_rotate_vertex	proc
	mov	si, offset sign7_vertex
	mov	fs, _temp_seg5
	xor	di, di
	xor	ah, ah
	mov	al, blur_angle
	add	al, 16+8
	shl	ax, 1
	mov	bp, ax
	add	bp, offset rotate_sin
	mov	cx, 90
@@l1:
	push	cx
	push	si
	xor	ah, ah
	mov	al, [si]
	sub	ax, 64+32
	mov	word ptr [blur_rotate_vertex_data], ax
	xor	ah, ah
	mov	al, [si+1]
	sub	ax, 64+32
	mov	word ptr [blur_rotate_vertex_data+2], ax
	mov	si, offset blur_rotate_vertex_data
	call	blur_rotate_z
	pop	si

	add	si, 2
	add	di, 4

	pop	cx
	loop	@@l1
	ret
blur_rotate_vertex	endp

blur_rotate_vertex_data	dw 0, 0

blur_rotate_z	proc
	mov	ax, [si+2]
	movsx	eax, ax
	mov	cx, ds:[bp] ; y*sin(a)
	movsx	ecx, cx
	imul	ecx
        sar	eax, 8
	mov	bx, ax
	mov	ax, [si]
	movsx	eax, ax
	mov	cx, ds:[bp+64*2] ; x*cos(a)
	movsx	ecx, cx
	imul	ecx
        sar	eax, 8
	sub	ax, bx
	mov	bx, 1b0h
	imul	bx
	mov	al, ah
	mov	ah, dl
	add	ax, 160
	mov fs:[di], ax
;
	mov	ax, [si]
	movsx	eax, ax
	mov	cx, ds:[bp] ; x*sin(a)
	movsx	ecx, cx
	imul	ecx
        sar	eax, 8
	mov	bx, ax
	mov	ax, [si+2]
	movsx	eax, ax
	mov	cx, ds:[bp+64*2] ; y*cos(a)
	movsx	ecx, cx
	imul	ecx
        sar	eax, 8
	add	ax, bx
	mov	bx, 1b0h
	imul	bx
	mov	al, ah
	mov	ah, dl
	mov	bx, 100
	sub	bx, ax
	mov	fs:[di+2], bx
;
	ret
blur_rotate_z	endp

blur_lines_move	proc
	xor	ch, ch
	mov	cl, blur_lines_count
	mov	si, offset blur_lines_data
@@l1:
	mov	eax, dword ptr [si+4]
	sub	dword ptr [si], eax
	add	si, 9
	loop	@@l1
	ret
blur_lines_move	endp

blur_lines_show	proc
	mov	fs, _temp_seg1
	xor	ch, ch
	mov	cl, blur_lines_count
	mov	si, offset blur_lines_data
@@l1:
	mov	eax, dword ptr [si]
	shr	eax, 16
	cmp	ax, 199
	ja	@@j1
	call	blur_lines_line
@@j1:
	add	si, 9
	loop	@@l1
	ret
blur_lines_show	endp

blur_lines_line	proc
	push	cx
	mov	bx, 320
	mul	bx
	mov	di, ax
	mov	cx, 320
	cmp	byte ptr [si+8], 10
	ja	@@j1
	xor	ah, ah
@@j1:
@@l1:
	mov	al, 10
	call	random
	add	al, [si+8]
	add	fs:[di], al
	inc	di
	loop	@@l1
	pop	cx
	ret
blur_lines_line	endp

blur_lines_count	db 4
blur_lines_data:
	dw 0, 200, 0, 2
	db 30
	dw 0, 200+200, 0, 2
	db 30
	dw 0, 200, 8000h, 1
	db 20
	dw 0, 220, 4000h, 1
	db 10
;	dw 0, 300, 0, 1
;	db 10
;	dw 0, 300, 0e000h, 0
;	db 0

blur_clear	proc
	cmp	sound_card, 1
	jz	@@sb
	mov	ax, seg _code
	mov	fs, ax
	mov	ax, fs:[RealRow]
	jmp	@@jj
@@sb:
	mov	ax, seg __code
	mov	fs, ax
	mov	ax, fs:[curnote]
@@jj:
	cmp	ax, blur_clear_row
	jz	@@j1
	mov	blur_clear_row, ax
	mov	bx, blur_clear_pos
	call	blur_do_clear
	add	blur_clear_pos, 13
	new_screen
	cmp	blur_clear_row, 63
	jz	@@e
@@j1:
	store_command	153
@@e:
	ret
blur_clear	endp

blur_clear_pos	dw 0
blur_clear_row	dw 0

blur_do_clear	proc
	mov	fs, _temp_seg1
	mov	ax, 320
	mul	bx
	mov	di, ax
	mov	si, 63999
	sub	si, di
	mov	cx, 320
@@l1:
	mov	dl, 13
	mov	ax, 5
	call	random
	add	dl, al
	push	di si
@@l2:
	mov	byte ptr fs:[di], 0
	mov	byte ptr fs:[si], 0
	add	di, 320
	sub	si, 320
	dec	dl
	jnz	@@l2
	pop	si di
	inc	di
	dec	si
	loop	@@l1
	ret
blur_do_clear	endp

blur_mosaic	proc
	call	blur

	mov	fs, [blur_adr]
	mov	gs, _temp_seg1
	mov	si, 160
	call	blur_mosaic_copy

	mov	fs, [blur_adr]
	mov	bl, 8
	call	blur_shift
	mov	fs, [blur_adr]
	mov	gs, _temp_seg1
	mov	si, 100*320
	call	blur_mosaic_copy

	mov	gs, _temp_seg2
	call	clear_seg
	mov	es, _temp_seg2
	call	blur_rotate_vertex
	call	blur_vertex
	call	blur_vertex
	call	blur_vertex
	call	blur_rotate_vertex
	call	blur_vertex
	call	blur_vertex
	call	blur_rotate_vertex
	call	blur_vertex
	mov	fs, _temp_seg2
	mov	gs, _temp_seg1
	mov	si, 100*320+160
	call	blur_mosaic_copy

	new_screen
	store_palette	grey_inv_pal, 0, 256
	store_command	154
	ret
blur_mosaic	endp

blur_mosaic_copy	proc
	mov	dx, 100
@@l2:
	mov	cx, 160/4
@@l1:
	mov	eax, fs:[si]
	mov	gs:[si], eax
	add	si, 4
	dec	cx
	jnz	@@l1
	add	si, 160
	dec	dx
	jnz	@@l2
	ret
blur_mosaic_copy	endp

blur_palette	proc
	mov	al, 5
	mov	bl, blur_palette_pos
	mul	bl
	mov	bx, ax
	add	bx, offset blur_palette_data

	mov	al, 0
	mov	ah, [bx+4]
	mov	cx, 256*3
	mov	si, [bx]
	mov	di, [bx+2]
	call	makepal1to2
	store_palette	_temppal, 0, 256

	inc	blur_palette_pos
	cmp	blur_palette_pos, 11-3
	jz	@@e
	store_command	154
@@e:
	ret
blur_palette_pos	db 0
blur_palette_data:
	dw offset grey_inv_pal, offset grey_pal
	db 8
	dw offset grey_inv_pal, offset grey_pal
	db 16
	dw offset grey_inv_pal, offset grey_pal
	db 24
	dw offset grey_inv_pal, offset grey_pal
	db 32
;	dw offset grey_pal, offset white_pal
;	db 32
	dw offset grey_inv_pal, offset grey_pal
	db 40
	dw offset grey_inv_pal, offset grey_pal
	db 48
	dw offset grey_inv_pal, offset grey_pal
	db 56
	dw offset grey_inv_pal, offset grey_pal
	db 63
;	dw offset grey_pal, offset white_pal
;	db 16
;	dw offset grey_inv_pal, offset grey_pal
;	db 63
blur_palette	endp