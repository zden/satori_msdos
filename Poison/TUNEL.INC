tunel_len	EQU 1000
tunel_frames	EQU 40

tunel_toggle	proc
	mov	blur_stop, 1
	xor	tunel_stop, 1
	jnz	@@e

	xor	edx, edx
	mov	ax, tunel_len
	shl	eax, 16
	xor	ebx, ebx
	mov	bx, tunel_frames
	div	ebx
	mov	tunel_speed, eax
	reset_counter

	call	clear_temp_seg2
	mov	es, _temp_seg2
	call	drawobject c, offset obj_sign7, 0, 0, 63
	call	clear_temp_seg6
	mov	fs, _temp_seg2
	mov	gs, _temp_seg6
	mov	si, 0
	mov	di, 27*256+27
	mov	cx, 200
	mov	dx, 200
	call	conv320to256

	call	clear_temp_seg1
	call	clear_temp_seg2

;	mov	rot_funct, offset rotator_tunel

	store_command	171
	store_palette	grey_pal, 0, 256
@@e:
	ret
tunel_toggle	endp

tunel_speed	dd 0
tunel_frame	dw 0
tunel_lframe	dw -1
tunel_stop	db 1
tunel_windows	db 0
tunel_sick	db 0
tunel_spiral	db 0
tunel_spiral_pos	db 0
tunel_spiral_r	dw 0
tunel_leaving	db 0

tunel_progress	proc
	cmp	tunel_stop, 1
	jz	@@ee

	get_counter
	xor	edx, edx
	shl	eax, 16
	mov	ebx, tunel_speed
	div	ebx
	cmp	ax, tunel_lframe
	jz	@@e

	mov	tunel_frame, ax
	mov	ax, tunel_lframe
@@l1:
	inc	ax
	push	ax

	cmp	tunel_spiral, 0
	jz	@@nospiral2
	add	tunel_spiral_pos, 3
	test	al, 10
	jnz	@@no_r
	inc	tunel_spiral_r
@@no_r:
@@nospiral2:
	call	tunel_move_angle
	call	tunel_move_angle
	mov	cx, 16+8
@@cl1:
	push	cx
	call	tunel_move_forward
	pop	cx
	loop	@@cl1

	pop	ax
	cmp	ax, tunel_frame
	jnz	@@l1
	mov	ax, tunel_frame
	mov	tunel_lframe, ax

	cmp	tunel_spiral, 0
	jz	@@nospiral
	call	tunel_move_mid
@@nospiral:

	call	tunel_image
	new_screen
@@e:
	store_command	171
@@ee:
	ret
tunel_progress	endp

tunel_image	proc
	call	clear_temp_seg1
	cmp	tunel_leaving, 0
	jz	@@nol
	call	clear_temp_seg2
@@nol:
	mov	si, offset tunel_main_data
	mov	fs, _temp_seg6
	mov	gs, _temp_seg2
	mov	cx, 4
@@l1:
	push	cx si
	mov	ax, [si]
	cmp	ax, 1ffh
	ja	@@ni
	mov	rot_distance, ax
	mov	bx, 200h ; 400h
	sub	bx, ax
	dec	bx
	shr	bx, 3 ; 4
	mov	tunel_rotator_color, bl
	mov	al, [si+2]
	mov	rot_angle, al
	mov	ax, [si+4]
	mov	rot_x_mid, ax
	mov	ax, [si+6]
	mov	rot_y_mid, ax
	call	tunel_rotator
@@ni:
	pop	si cx
	add	si, 8
	loop	@@l1

;	mov	fs, _temp_seg2
;	mov	gs, _temp_seg1
;	mov	si, 35*320
;	mov	cx, (130*320)/4
;@@cl1:
;	mov	eax, fs:[si]
;	mov	gs:[si], eax
;	add	si, 4
;	dec	cx
;	jnz	@@cl1

	cmp	tunel_windows, 0
	jz	@@nowins
	call	tunel_do_window
@@nowins:
	mov	fs, _temp_seg2
	mov	gs, _temp_seg1
	mov	si, offset tunel_soften_data
	call	soften

	mov	tunel_kaos_add, -16
	mov	cx, 12
@@kl1:
	push	cx
	call	tunel_do_kaos_lines
	pop	cx
	loop	@@kl1
	mov	tunel_kaos_add, 200-16-32
	mov	cx, 12
@@kl2:
	push	cx
	call	tunel_do_kaos_lines
	pop	cx
	loop	@@kl2

	ret
tunel_image	endp

tunel_soften_data	dw 2, 35, 320-4, 130

tunel_rotator	proc
	cmp	tunel_sick, 0
	jz	@@j1
	cmp	cl, tunel_sick
	jnz	@@j2
	mov	rot_funct, offset rotator_tunel2
	call	rotator
@@j2:
	ret
@@j1:
	cmp	cx, 4
	jnz	@@redef1
	mov	rot_funct, offset rotator_tunel2
	jmp	@@redef2
@@redef1:
	mov	rot_funct, offset rotator_tunel
@@redef2:
	call	rotator
	ret
tunel_rotator	endp

tunel_leaving_toggle	proc
	xor	tunel_leaving, 1
	ret
tunel_leaving_toggle	endp

tunel_spiral_toggle	proc
	xor	tunel_spiral, 1
	mov	word ptr [tunel_main_data], 80h+140h
	mov	word ptr [tunel_main_data+8], 100h+140h
	mov	word ptr [tunel_main_data+8*2], 180h+140h
	mov	word ptr [tunel_main_data+8*3], 1ffh+140h
	ret
tunel_spiral_toggle	endp

;tunel_do_drum	proc
;	mov	fs, _temp_seg1
;	mov	eax, 03f3f3f3fh
;
;	mov	si, 30*320
;	mov	cx, 320
;@@l1:
;	xor	fs:[si], eax
;	add	si, 4
;	loop	@@l1
;
;	mov	si, (200-34)*320
;	mov	cx, 320
;@@l2:
;	xor	fs:[si], eax
;	add	si, 4
;	loop	@@l2
;	ret
;tunel_do_drum	endp

tunel_windows_toggle	proc
	xor	tunel_windows, 1
	ret
tunel_windows_toggle	endp

tunel_sick_toggle	proc
	inc	tunel_sick
	cmp	tunel_sick, 5
	jnz	@@j1
	mov	tunel_sick, 0
@@j1:
	ret
tunel_sick_toggle	endp

tunel_move_forward	proc
	mov	si, offset tunel_main_data
	mov	cx, 4
@@l1:
	dec	word ptr [si]
	add	si, 8
	loop	@@l1
	cmp	tunel_leaving, 0
	jz	@@nol
	cmp	word ptr [tunel_main_data], 0
	jnz	@@j1
	mov	si, offset tunel_main_data
	mov	cx, 3*8
@@l3:
	mov	al, [si+8]
	mov	[si], al
	inc	si
	loop	@@l3
	mov	word ptr [tunel_main_data+3*8], 0ffffh ; 3ffh
	ret
@@nol:
	cmp	word ptr [tunel_main_data], 0
	jnz	@@j1

	mov	bx, word ptr [tunel_main_data+2]
	mov	edx, dword ptr [tunel_main_data+4]
	mov	si, offset tunel_main_data
	mov	cx, 3*8
@@l2:
	mov	al, [si+8]
	mov	[si], al
	inc	si
	loop	@@l2
	mov	word ptr [tunel_main_data+3*8], 1ffh ; 3ffh
	mov	word ptr [tunel_main_data+3*8+2], bx
	mov	dword ptr [tunel_main_data+3*8+4], edx
@@j1:
	ret
tunel_move_forward	endp

tunel_move_angle	proc
	mov	si, offset tunel_main_data
	mov	cx, 4
@@l1:
	cmp	byte ptr [si+3], 0
	jz	@@j1
	dec	byte ptr [si+2]
	jmp	@@j2
@@j1:
	inc	byte ptr [si+2]
@@j2:
	add	si, 8
	loop	@@l1
	ret
tunel_move_angle	endp

tunel_move_mid	proc
	mov	si, offset tunel_main_data
	mov	di, offset rotate_sin
	mov	cx, 4
@@l1:
	xor	bh, bh
	mov	bl, tunel_spiral_pos ; [si+2]
	shl	bx, 1

	mov	ax, [di+bx]
	mov	dx, tunel_spiral_r
	mul	dx
	sar	ax, 8
	add	ax, 160
	mov	[si+4], ax

	mov	ax, [di+bx+128]
	mov	dx, tunel_spiral_r
	mul	dx
	sar	ax, 8
	add	ax, 65
	mov	[si+6], ax

	add	tunel_spiral_pos, 16
	add	tunel_spiral_r, 8
	add	si, 8
	loop	@@l1
	sub	tunel_spiral_pos, 16*4
	sub	tunel_spiral_r, 8*4
	ret
tunel_move_mid	endp

tunel_main_data:
	dw 0080h+140h
	db 0, 1
	dw 160, 65

	dw 0100h+140h
	db 64, 1
	dw 160, 65

	dw 0180h+140h
	db 128, 1
	dw 160, 65

	dw 01ffh+140h
	db 192, 1
	dw 160, 65

tunel_rotator_color	db 0

comment	%

tunel_do_windows	proc
	mov	fs, _temp_seg1
	mov	gs, _temp_seg2
	mov	si, offset tunel_windows_data
	xor	ch, ch
	mov	cl, [tunel_windows_count]
@@l1:
	push	cx
	call	tunel_do_window
	pop	cx
	add	si, 6*2
	loop	@@l1
	ret
tunel_do_windows	endp

tunel_do_window	proc
;	int 3
	mov	ax, 10
	call	random
	sub	ax, 5
	add	ax, [si+2]
	mov	bx, 320
	mul	bx
	mov	bp, ax

	mov	ax, 10
	call	random
	sub	ax, 5
	add	bp, ax
	add	bp, [si]

	mov	ax, [si+10]
	mov	bx, 320
	mul	bx
	add	ax, [si+8]
	mov	di, ax

	xor	al, al
	mov	cx, [si+6]
@@l1:
	xor	ah, ah
	push	cx bp

	xor	dx, dx
	mov	cx, [si+4]
@@l2:
	add	dx, [si+12]
	mov	bx, dx
	shr	bx, 8
	mov	bl, fs:[di+bx]
	mov	byte ptr gs:[bp], bl
	inc	bp
	dec	cx
	jnz	@@l2

	add	ax, [si+14]
	mov	bl, ah
@@j2:
	or	bl, bl
	jz	@@j1
	add	di, 320
	dec	bl
	jmp	@@j2
@@j1:
	pop	bp cx
	add	bp, 320
	dec	cx
	jnz	@@l1
	ret
tunel_do_window	endp

tunel_windows_count	db 1
tunel_windows_data:
;	dw 160, 60, 150, 70, 80, 120h ; .
;	dw 50, 120-10, 250, 40, 080h, 0c0h
;	dw 120, 50, 50, 100, 80h, 80h ; w
;	dw 200, 45, 50, 100-20, 0c0h, 080h
;	dw 50, 50, 130, 50, 1e0h, 140h
;	dw 20, 90-5, 70, 60, 180h, 120h
;	dw 50, 50, 320-100, 50, 080h, 200h
;	dw 30, 150, 320-60, 30, 080h, 080h
	dw 160-80, 100-40, 160, 80, 0, 40, 200h, 190h

;	dw 10, 50, 50, 40, 200h, 180h
;	dw 10+60, 50, 50, 40, 200h, 180h
;	dw 10+60+60, 50, 50, 40, 200h, 180h
;	dw 10+60+60+60, 50, 50, 40, 200h, 180h
;	dw 10+60+60+60+60, 50, 50, 40, 200h, 180h

;	dw 10, 60+50, 50, 40, 200h, 180h
;	dw 10+60, 60+50, 50, 40, 200h, 180h
;	dw 10+60+60, 60+50, 50, 40, 200h, 180h
;	dw 10+60+60+60, 60+50, 50, 40, 200h, 180h
;	dw 10+60+60+60+60, 60+50, 50, 40, 200h, 180h

%

tunel_do_kaos_lines	proc
	mov	es, _temp_seg1

	mov	dx, 32-8-8
	mov	cx, 320/32
@@l1:
	cmp	cx, 320/32
	jnz	@@j1
	mov	word ptr [tunel_kaos_lines], 0
	mov	ax, 48
	call	random
	xor	ax, tunel_kaos_xor
	add	ax, tunel_kaos_add
	mov	word ptr [tunel_kaos_lines+2], ax
	mov	ax, 16+16
	call	random
	add	ax, dx
	mov	word ptr [tunel_kaos_lines+4], ax
	mov	ax, 48
	call	random
	xor	ax, tunel_kaos_xor
	add	ax, tunel_kaos_add
	mov	word ptr [tunel_kaos_lines+6], ax
	jmp	@@sh
@@j1:
	cmp	cx, 1
	jz	@@j2
	mov	eax, dword ptr [tunel_kaos_lines+4]
	inc	ax
	mov	dword ptr [tunel_kaos_lines], eax
	mov	ax, 16+16
	call	random
	add	ax, dx
	mov	word ptr [tunel_kaos_lines+4], ax
	mov	ax, 48
	call	random
	xor	ax, tunel_kaos_xor
	add	ax, tunel_kaos_add
	mov	word ptr [tunel_kaos_lines+6], ax
	jmp	@@sh
@@j2:
	mov	eax, dword ptr [tunel_kaos_lines+4]
	inc	ax
	mov	dword ptr [tunel_kaos_lines], eax
	mov	word ptr [tunel_kaos_lines+4], 319
	mov	ax, 48
	call	random
	xor	ax, tunel_kaos_xor
	add	ax, tunel_kaos_add
	mov	word ptr [tunel_kaos_lines+6], ax
@@sh:
	push	cx dx
	mov	eax, dword ptr [tunel_kaos_lines]
	mov	dword ptr [tunel_kaos_lines2], eax
	mov	eax, dword ptr [tunel_kaos_lines+4]
	mov	dword ptr [tunel_kaos_lines2+4], eax
	mov	si, offset tunel_kaos_lines2
	mov	cx, 1
	call	aaline
	pop	dx cx
	add	dx, 32
	xor	tunel_kaos_xor, 48
	loop	@@l1
;	xor	tunel_kaos_xor, 48
	ret

tunel_kaos_lines	dw 0, 0, 0, 0
tunel_kaos_lines2	dw 0, 0, 0, 0
tunel_kaos_add		dw -16
tunel_kaos_xor	dw 0

tunel_do_kaos_lines	endp

comment	%

tunel_do_window	proc
	mov	fs, _temp_seg2
	mov	gs, _temp_seg3
	mov	si, 35*320
	xor	di, di
	mov	dx, 65
@@l2:
	mov	cx, 160
@@l1:
	mov	al, fs:[si]
	mov	gs:[di], al
	add	si, 2
	inc	di
	dec	cx
	jnz	@@l1
	add	si, 320
	dec	dx
	jnz	@@l2

	mov	fs, _temp_seg3
	mov	gs, _temp_seg2
	xor	si, si
	mov	ax, 10
	call	random
	add	ax, 100-32-5
	mov	bx, 320
	mul	bx
	mov	di, ax
	mov	ax, 10
	call	random
	add	ax, 75
	add	di, ax
;	mov	di, (100-32)*320 + 80
	mov	dx, 65
@@l22:
	mov	cx, 160
@@l11:
	mov	al, fs:[si]
	mov	gs:[di], al
	inc	si
	inc	di
	dec	cx
	jnz	@@l11
	add	di, 160
	dec	dx
	jnz	@@l22

	ret
tunel_do_window	endp

%

tunel_do_window	proc
	mov	fs, _temp_seg2
	mov	gs, _temp_seg3
	mov	si, 35*320
	xor	di, di
	mov	dx, 65+20
@@l2:
	mov	cx, 100 ; 160
@@l1:
	mov	al, fs:[si]
	mov	gs:[di], al
	add	si, 2
	inc	di
	mov	al, fs:[si]
	mov	gs:[di], al
	inc	si
	inc	di
	dec	cx
	jnz	@@l1
	add	si, 20
	test	dl, 1
	jnz	@@nn
	add	si, 320 ; +20
@@nn:
	dec	dx
	jnz	@@l2

	mov	fs, _temp_seg3
	mov	gs, _temp_seg2
	xor	si, si
	mov	ax, 10
	call	random
	add	ax, 100-(85/2)-5
	mov	bx, 320
	mul	bx
	mov	di, ax
	mov	ax, 10
	call	random
	add	ax, (320-200)/2 - 5
	add	di, ax
;	mov	di, (100-32)*320 + 80
	mov	dx, 65+20
@@l22:
	mov	cx, 200
@@l11:
	mov	al, fs:[si]
	mov	gs:[di], al
	inc	si
	inc	di
	dec	cx
	jnz	@@l11
	add	di, 320-200
	dec	dx
	jnz	@@l22

	ret
tunel_do_window	endp