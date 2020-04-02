plasma_len	EQU 1000
plasma_frames	EQU 30

plasma_start	proc
	xor	plasma_stop, 1
	jnz	@@e
	xor	edx, edx
	mov	ax, plasma_len
	shl	eax, 16
	xor	ebx, ebx
	mov	bx, plasma_frames
	div	ebx
	mov	plasma_speed, eax
	reset_counter
	call	clear_temp_seg2
	mov	rot_x_mid, 160
	mov	rot_y_mid, 100
	mov	rot_funct, offset rotator_normal
	store_palette	plasma_pal, 0, 256
	store_command	121
@@e:
	ret
plasma_start	endp

plasma_speed	dd 0
plasma_frame	dw 0
plasma_lframe	dw -1
plasma_angle		db 48
plasma_angle_end	db 48
plasma_dist	dw 256
plasma_dist_end	dw 256
plasma_stop		db 1
plasma_mid	dw 8
plasma_no	db 0
plasma_pic_mirror	db 0

plasma_progress	proc
	cmp	plasma_no, 0
	jz	@@no1
	call	clear_temp_seg1
@@no1:

	cmp	plasma_stop, 1
	jz	@@ee
	get_counter
	xor	edx, edx
	shl	eax, 16
	mov	ebx, plasma_speed
	div	ebx
	cmp	ax, plasma_lframe
	jz	@@e

	mov	plasma_frame, ax
	mov	ax, plasma_lframe
@@j1:
	push	ax
	call	plasma_move

	mov	cx, 4
@@l1:
	push	cx
	call	do_plasma_angle
	pop	cx
	loop	@@l1

	mov	cx, 8
@@l2:
	push	cx
	call	do_plasma_dist
	pop	cx
	loop	@@l2

;	mov	cx, 4
;@@l3:
;	push	cx
;	call	plasma_move_pump
;	pop	cx
;	loop	@@l3

	pop	ax
	inc	ax
	cmp	ax, plasma_frame
	jnz	@@j1
	call	plasma_middle

	mov	ax, plasma_frame
	mov	plasma_lframe, ax

	mov	fs, _temp_seg2
	call	do_plasma_frame
	call	plasma_move
	mov	gs, _temp_seg1
	cmp	plasma_no, 1
	jz	@@no2
	call	rotator
;	jmp	@@no3
@@no2:
;	call	gener_xor_crash
;	mov	si, offset xor_crash_data
;	call	do_xor_crash
;@@no3:
	call	plasma_pic
;	call	plasma_show_pump
	new_screen
@@e:
	store_command	121
@@ee:
	ret
plasma_progress	endp

plasma_nou	proc
	xor	plasma_no, 1
	ret
plasma_nou	endp

plasma_pump_here	proc
	mov	byte ptr [offset plasma_pump_pos], 15
	ret
plasma_pump_here	endp

plasma_move_pump	proc
	mov	si, offset plasma_pump_pos+15
	mov	cx, 15
@@l1:
	mov	al, [si-1]
	mov	[si], al
	dec	si
	loop	@@l1
	mov	al, [si]
	or	al, al
	jz	@@j1
	dec	byte ptr [si]
@@j1:
	ret
plasma_move_pump	endp

plasma_show_pump	proc
	mov	fs, _temp_seg1
	mov	si, offset plasma_pump_pos
;	mov	di, 0
;	mov	di, 170*320
	mov	ax, 5
	call	random
	add	ax, 160
	mov	bx, 320
	mul	bx
	mov	di, ax
	mov	cx, 16
@@l1:
	mov	bl, [si]
	shl	bl, 2
	push	cx si di
	call	plasma_show_pump_pic
	pop	di si cx
	inc	si
	add	di, 19
	loop	@@l1
	ret
plasma_pump_pos	db 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
plasma_show_pump	endp

plasma_show_pump_pic	proc
	mov	si, offset plasma_pump_pic
	mov	cx, 6*4-1
@@l1:
	push	cx
	mov	cx, 12
@@l2:
	mov	al, [si]
	mul	bl
	or	al, al
	jz	@@j1
	add	byte ptr fs:[di], al
	cmp	byte ptr fs:[di], 64
	jb	@@j1
	mov	byte ptr fs:[di], 63
@@j1:
	inc	si
	inc	di
	loop	@@l2
	add	di, 320-12
	pop	cx
	loop	@@l1
	ret
plasma_show_pump_pic	endp

plasma_pic_next_line	proc
	call	do_plasma_pic_next_line
	call	do_plasma_pic_next_line
	ret
plasma_pic_next_line	endp

do_plasma_pic_next_line	proc
	mov	cx, 25
	xor	al, al
	mov	si, offset plasma_pic_visible_lines
@@l1:
	add	al, [si]
	inc	si
	loop	@@l1
	cmp	al, 25
	jz	@@e
	mov	ax, 3
	call	random
	inc	al
@@j4:
	mov	si, offset plasma_pic_visible_lines
@@j3:
	cmp	byte ptr [si], 1
	jz	@@j1
	dec	al
@@j1:
	or	al, al
	jz	@@j2
	inc	si
	cmp	si, offset plasma_pic_visible_lines+25
	jz	@@j4
	jmp	@@j3
@@j2:
	mov	byte ptr [si], 1
@@e:
	ret
do_plasma_pic_next_line	endp

plasma_pic	proc
	mov	es, _temp_seg1
	call	plasma_pic_prepare
@@l1:
	push	cx
	cmp	byte ptr ds:[bp], 0
	jz	@@j1
	call	plasma_pic1
	call	plasma_pic1
	call	plasma_pic1
	call	plasma_pic1
@@j1:
	pop	cx
	inc	bp
	add	di, 4*2
	loop	@@l1

	call	plasma_pic_prepare
@@l2:
	push	cx
	cmp	byte ptr ds:[bp], 0
	jz	@@j2
	call	plasma_pic2
	call	plasma_pic2
	call	plasma_pic2
@@j2:
	pop	cx
	inc	bp
	add	di, 4*2
	loop	@@l2

	call	plasma_pic_prepare
@@l3:
	push	cx
	cmp	byte ptr ds:[bp], 0
	jz	@@j3
	call	plasma_pic3
	call	plasma_pic3
@@j3:
	pop	cx
	inc	bp
	add	di, 4*2
	loop	@@l3

	ret

plasma_pic_temp	dw 4 DUP (0)
plasma_pic_visible_lines	db 25 DUP (0)
plasma_pic	endp

plasma_pic_prepare	proc
	mov	ax, 10
	call	random
	sub	ax, 10
	mov	bx, ax
	mov	ax, 10
	call	random
	sub	ax, 15
	mov	dx, ax

	mov	ax, 24 ; 32
	call	random
	or	al, al
	jnz	@@j1
	mov	ax, 20
	call	random
	add	ax, 50
	mov	bp, ax
	mov	al, 2
	call	random
	or	al, al
	jz	@@j2
	neg	bp
@@j2:
	add	dx, bp
@@j1:
	mov	cx, plasma_pic_lines
	mov	si, offset plasma_pic_temp
	mov	di, offset plasma_pic_data
	mov	bp, offset plasma_pic_visible_lines
	ret
plasma_pic_prepare	endp

plasma_pic1	proc
	mov	ax, [di]
	add	ax, bx
	call	plasma_pic_x_convert
	mov	[si], ax
	mov	ax, [di+2]
	add	ax, dx
	call	plasma_pic_y_convert
	mov	[si+2], ax
	mov	ax, [di+4]
	add	ax, bx
	call	plasma_pic_x_convert
	mov	[si+4], ax
	mov	ax, [di+6]
	add	ax, dx
	call	plasma_pic_y_convert
	mov	[si+6], ax
	mov	cx, 1
	call	aaline
	ret
plasma_pic1	endp

plasma_pic2	proc
	mov	ax, [di]
	add	ax, bx
	add	ax, 2
	call	plasma_pic_x_convert
	mov	[si], ax
	mov	ax, [di+2]
	add	ax, dx
	add	ax, 2
	call	plasma_pic_y_convert
	mov	[si+2], ax
	mov	ax, [di+4]
	add	ax, bx
	add	ax, 2
	call	plasma_pic_x_convert
	mov	[si+4], ax
	mov	ax, [di+6]
	add	ax, dx
	add	ax, 2
	call	plasma_pic_y_convert
	mov	[si+6], ax
	mov	cx, 1
	call	aaline
	ret
plasma_pic2	endp

plasma_pic3	proc
	mov	ax, [di]
	add	ax, bx
	sub	ax, 2
	call	plasma_pic_x_convert
	mov	[si], ax
	mov	ax, [di+2]
	add	ax, dx
	call	plasma_pic_y_convert
	mov	[si+2], ax
	mov	ax, [di+4]
	add	ax, bx
	sub	ax, 2
	call	plasma_pic_x_convert
	mov	[si+4], ax
	mov	ax, [di+6]
	add	ax, dx
	call	plasma_pic_y_convert
	mov	[si+6], ax
	mov	cx, 1
	call	aaline
	ret
plasma_pic3	endp

plasma_pic_x_convert	proc
	cmp	plasma_pic_mirror, 0
	jz	@@j1
	mov	cx, ax
	mov	ax, 40
	call	random
	sub	ax, 20
	add	ax, cx
;	sub	ax, 319
;	neg	ax
@@j1:
	ret
plasma_pic_x_convert	endp

plasma_pic_y_convert	proc
	cmp	plasma_pic_mirror, 0
	jz	@@j1
	mov	cx, ax
	mov	ax, 40
	call	random
	sub	ax, 20
	add	ax, cx
;	sub	ax, 199
;	neg	ax
@@j1:
	ret
plasma_pic_y_convert	endp

plasma_pic_mirror_toggle	proc
	xor	plasma_pic_mirror, 1
	ret
plasma_pic_mirror_toggle	endp

plasma_grey	proc
	store_palette	plasma_pal, 0, 256
	ret
plasma_grey	endp

plasma_flash	proc
	store_palette	white_pal, 0, 256
	store_command 126
	ret
plasma_flash	endp

plasma_normal	proc
	mov	word ptr [plasma_type], 9090h
	store_palette	plasma_pal, 0, 256
	store_command	126
	ret
plasma_normal	endp

plasma_xor	proc
	mov	word ptr [plasma_type], 0c733h
	store_palette	plasma_fl_pal, 0, 256
	store_command	126
	ret
plasma_xor	endp

plasma_middle	proc
	mov	ax, plasma_mid
	call	random
	mov	bx, plasma_mid
	shr	bx, 1
	add	ax, 128
	sub	ax, bx
	neg	ax
	sal	ax, 8
	mov	rot_x_pos, ax

	mov	ax, plasma_mid
	call	random
	mov	bx, plasma_mid
	shr	bx, 1
	add	ax, 128
	sub	ax, bx
	neg	ax
	sal	ax, 8
	mov	rot_y_pos, ax
	ret
plasma_middle	endp

do_plasma_angle	proc
@@j2:
	mov	al, plasma_angle
	mov	ah, plasma_angle_end
	cmp	al, ah
	jnz	@@j1
	mov	al, plasma_angle_end
	mov	plasma_angle, al
	mov	ax, 16 ; 32 ; 64
	call	random
	add	al, 48+8 ; 32
	mov	plasma_angle_end, al
	jmp	@@j2
@@j1:
	cmp	al, ah
	ja	@@j3
	inc	al
	jmp	@@j4
@@j3:
	dec	al
@@j4:
	mov	plasma_angle, al
	mov	rot_angle, al
	ret
do_plasma_angle	endp

do_plasma_dist	proc
@@j2:
	mov	ax, plasma_dist
	mov	bx, plasma_dist_end
	cmp	ax, bx
	jnz	@@j1
	mov	ax, plasma_dist_end
	mov	plasma_dist, ax
	mov	ax, 128+64 ; 64+64
	call	random
	add	ax, 128 ; 192+32 ; 192
	mov	plasma_dist_end, ax
	jmp	@@j2
@@j1:
	cmp	ax, bx
	ja	@@j3
	inc	ax
	jmp	@@j4
@@j3:
	dec	ax
@@j4:
	mov	plasma_dist, ax
	mov	rot_distance, ax
	ret
do_plasma_dist	endp

plasma_sin_destruct	proc
	mov	si, offset plasmasin+256
	mov	cx, 256
@@2:
	mov	ax, 3
	call	random
	add	[si], al
	inc	si
	loop	@@2
	ret
plasma_sin_destruct	endp

plasma_mid_destruct	proc
	add	plasma_mid, 8
	ret
plasma_mid_destruct	endp

do_plasma_frame	proc
	mov	bx, offset plasmasin
;	push cs
;	pop ds
	xor	di, di

	mov	cl, VSP1
	mov	ch, VSP2
	mov	ypos, 0
cfjmp2:
	mov	dl, HSP1
	mov	dh, HSP2
	mov	xpos, 0
cfjmp1:
	mov	ah, 0

	mov	al, cl
	xlat
	add	ah, al

	mov	al, ch
	xlat
	add	ah, al

	add	bx, 256

	mov	al, dl
	xlat
	add	ah, al
	add	dl, HS1

	mov	al, dh
	xlat
	add	ah, al
	sub	dh, HS2

	sub	bx, 256

	mov	al, ah
plasma_type:
	db 33h, 0c7h
;	xor	ax, di
	mov	fs:[di+256+2], al	; 0
	sub	al, 2+2			; -2
	mov	fs:[di+256+1], al
	mov	fs:[di+256+3], al
	dec	al			; -3
	dec	al
	mov	fs:[di+2], al
	mov	fs:[di+256*2+2], al
	dec	al			; -4
	dec	al
	mov	fs:[di+256], al
	mov	fs:[di+256+4], al
	dec	al			; -5
	dec	al
	mov	fs:[di+1], al
	mov	fs:[di+3], al
	mov	fs:[di+256*2+1], al
	mov	fs:[di+256*2+3], al
	sub	al, 3*2			; -8
	mov	fs:[di], al
	mov	fs:[di+4], al
	mov	fs:[di+256*2], al
	mov	fs:[di+256*2+4], al

	mov	byte ptr fs:[di+256*3], 16
	mov	byte ptr fs:[di+256*3+1], 16
	mov	byte ptr fs:[di+256*3+2], 16
	mov	byte ptr fs:[di+256*3+3], 16
	mov	byte ptr fs:[di+256*3+4], 16

	add	di, 5
	inc	xpos
	cmp	xpos, 50
	jnz	cfjmp1

	add	cl, VS1
	add	ch, VS2

	inc	ypos

	add	di, 256-50*5 + 256*3

	cmp	ypos, 63
	jnz	cfjmp2
	ret
do_plasma_frame	endp

plasma_move	proc
	add	VSP1, VSPS1
	sub	VSP2, VSPS2
	add	HSP1, HSPS1
	sub	HSP2, HSPS2
	ret
plasma_move	endp

; ****************************
plasmasin:
;	db 54,53,52,51,49,48,47,46,45,44,43,41,40,39,38,36,35,33,32,30,29,28,26,25,23,22,20,19,18,17,16,16,15,14,13,12,11,10,10,9,8,7,7,6,6,6,5,5,5,5,4,4,4,3,3,3,2,2,2,2,1,1,1,0,0,1,3,5,7,9,10,12,14,16,18,19,20,21,22,24,25,26,27,28,29,30,31,33,34,35,36,37,38,39,41,42,43,44,48,55,61,68,75,79,81,83,85,87,89,91,93,95,97,99,101,103,105,107,108,108,109,110,110,111,112,112,113,113,114,115,115,116,117,117,118,118,118,118,119,119,119,119,119,119,119,119,120,120,120,120,119,118,117,116,116,115,114,113,112,111,110,109,108,108,107,106,105,102,96,91,85,81,78,75,73,70,67,64,61,58,56,53,50,48,46,44,42,40,38,36,34,32,30,28,27,25,23,21,19,17,15,13,11,9,7,6,7,8,9,11,12,13,14,15,16,18,19,20,21,22,23,25,26,27,31,37,43,49,56,62,68,74,79,83,86,90,93,97,100,103,99,94,90,85,80,76,72,69,65,62,60,59,58,57,56,54
	db 47,45,44,43,41,40,38,37,36,34,33,31,30,29,27,26,25,24,23,22,21,20,19,18,18,17,16,15,14,13,12,11,11,10,10,10,9,9,8,8,8,7,7,7,6,6,5,5,5,4,4,4,4,4,4,4,3,3,3,3,3,3,3,3,3,3,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,2,2,2,3,3,3,4,4,5,5,5,6,6,6,7,7,8,8,8,9,9,9,10,10,11,11,12,13,14,16,17,18,19,20,21,23,24,25,27,29,31,33,35,37,40,42,44,46,48,50,53,56,60,63,66,70,73,75,77,79,81,83,85,86,88,90,92,94,96,97,99,100,102,103,104,106,107,109,109,110,110,111,111,112,112,113,113,113,114,114,115,115,116,116,117,117,117,117,117,117,117,117,117,117,117,117,117,117,117,117,117,117,116,116,115,114,113,113,112,111,111,110,109,108,108,106,103,100,97,93,90,87,83,78,73,69,64,59,56,55,54,52,51,50,47
	db 128,131,134,137,140,143,146,149,152,155,158,161,164,167,170,173,176,179,182,185,187,190,193,195,198,201,203,206,208,210,213,215,217,219,222,224,226,228,230,231,233,235,236,238,240,241,242,244,245,246
	db 247,248,249,250,251,251,252,253,253,254,254,254,254,254,255,254,254,254,254,254,253,253,252,251,251,250,249,248,247,246,245,244,242,241,240,238,236,235,233,231,230,228,226,224,222,219,217,215,213,210
	db 208,206,203,201,198,195,193,190,187,185,182,179,176,173,170,167,164,161,158,155,152,149,146,143,140,137,134,131,128,124,121,118,115,112,109,106,103,100,97,94,91,88,85,82,79,76,73,70,68,65
	db 62,60,57,54,52,49,47,45,42,40,38,36,33,31,29,27,25,24,22,20,19,17,15,14,13,11,10,9,8,7,6,5,4,4,3,2,2,1,1,1,1,1,1,1,1,1,1,1,2,2
	db 3,4,4,5,6,7,8,9,10,11,13,14,15,17,19,20,22,24,25,27,29,31,33,36,38,40,42,45,47,49,52,54,57,60,62,65,68,70,73,76,79,82,85,88,91,94,97,100,103,106
	db 109,112,115,118,121,124

xpos	db 0
ypos	db 0

VSP1	db 1
VSP2	db 22
HSP1	db 33
HSP2	db 77

VSPS1	EQU 1
VSPS2	EQU -3
HSPS1	EQU 1
HSPS2	EQU 1

VS1	EQU 2
VS2	EQU 2
HS1	EQU 3
HS2	EQU 4

plasma_pic_lines	EQU 25

plasma_pic_data:
	dw 28, 21, 39, 182		; p
	dw 16, 23, 86, 95
	dw 19, 126, 90, 72
	dw 19, 82, 138, 47		; o
	dw 36, 147, 153, 129
	dw 62, 43, 69, 156
	dw 117, 27, 130, 152
	dw 106, 178, 138, 63		; i
	dw 95, 163, 185, 154
	dw 120, 87, 195, 172
	dw 96, 76, 174, 59		; .
	dw 93, 103, 167, 94
	dw 104, 64, 105, 110
	dw 150, 54, 166, 108
	dw 218, 28, 149, 112		; s
	dw 145, 106, 236, 100
	dw 215, 90, 212, 196
	dw 228, 185, 132, 193
	dw 180, 93, 260, 74		; o
	dw 185, 135, 263, 122
	dw 191, 81, 203, 141
	dw 244, 59, 248, 141
	dw 267, 171, 226, 56		; n
	dw 217, 61, 314, 153
	dw 262+20, 36, 298, 161

plasma_pump_pic:
	db 1,0,0,0,0,0,0,0,0,0,0,0
	db 1,1,0,0,0,0,0,0,0,0,0,0
	db 1,1,1,0,0,0,0,0,0,0,0,0
	db 1,1,1,1,0,0,0,0,0,0,0,0

	db 1,1,1,1,1,0,0,0,0,0,0,0
	db 1,1,1,1,1,1,0,0,0,0,0,0
	db 1,1,1,1,1,1,1,0,0,0,0,0
	db 1,1,1,1,1,1,1,1,0,0,0,0

	db 1,1,1,1,1,1,1,1,1,0,0,0
	db 1,1,1,1,1,1,1,1,1,1,0,0
	db 1,1,1,1,1,1,1,1,1,1,1,0
	db 1,1,1,1,1,1,1,1,1,1,1,1

	db 1,1,1,1,1,1,1,1,1,1,1,0
	db 1,1,1,1,1,1,1,1,1,1,0,0
	db 1,1,1,1,1,1,1,1,1,0,0,0
	db 1,1,1,1,1,1,1,1,0,0,0,0

	db 1,1,1,1,1,1,1,0,0,0,0,0
	db 1,1,1,1,1,1,0,0,0,0,0,0
	db 1,1,1,1,1,0,0,0,0,0,0,0
	db 1,1,1,1,0,0,0,0,0,0,0,0

	db 1,1,1,0,0,0,0,0,0,0,0,0
	db 1,1,0,0,0,0,0,0,0,0,0,0
	db 1,0,0,0,0,0,0,0,0,0,0,0