rotate_len	EQU	3800
rotate_frames	EQU	(130+130+320)/2
; **************************************
rotate_sign1_init	proc
	xor	rotate_sign1_status, 1
	jnz	@@e
	call	clear_temp_seg2
	mov	es, _temp_seg2
;	cmp	rotate_sign1_pos, 0
;	jz	@@first
	call	drawobject c, offset obj_sign6, 60, 0, 63
;	jmp	@@second
;@@first:
;	call	drawobject c, offset obj_sign5, 60, 0, 63
;@@second:

	xor	si, si
	mov	cx, 64000
	xor	ah, ah
@@l1:
	mov	al, es:[si]
	or	al, al
	jz	@@j1
	mov	al, 16
	call	random
	add	al, 16
	mov	es:[si], al
@@j1:
	inc	si
	dec	cx
	jnz	@@l1

;	inc	rotate_sign1_pos
	store_command	75
@@e:
	ret
rotate_sign1_status	db 1
;rotate_sign1_pos	db 0
rotate_sign1_init	endp

rotate_sign1_progress	proc
	cmp	rotate_sign1_status, 1
	jz	@@e
	mov	es, _temp_seg2
	xor	si, si
	mov	cx, 64000
@@l1:
	mov	al, es:[si]
	or	al, al
	jz	@@j1
	add	al, 2 ; 3
	cmp	al, 64
	jb	@@j2
	xor	al, al
@@j2:
	mov	byte ptr es:[si], al
@@j1:
	inc	si
	loop	@@l1

	call	clear_temp_seg1
	mov	fs, _temp_seg2
	mov	gs, _temp_seg1
	mov	si, offset soften_intro_signs1
	call	soften

	call	rotate_2lines

	new_screen
	store_command	75
@@e:
	ret
rotate_sign1_progress	endp

; **************************************
rotate_start	proc
	xor	edx, edx
	mov	ax, rotate_len
	shl	eax, 16
	xor	ebx, ebx
	mov	bx, rotate_frames
	div	ebx
	mov	rotate_speed, eax
	reset_counter
	mov	rotate_frame, 0
;	call	clear_temp_seg1
;	new_screen
	store_command	61
	ret

rotate_speed	dd 0
rotate_frame	dw 0
rotate_lframe	dw -1
rotate_delay	db 0
rotate_pos_count db 0
rotate_2lines1	dw 20, 20, 300, 21, 20, 181, 300, 180
rotate_2lines2	dw 8 DUP (0)

rotate_start	endp

; **************************************
rotate_progress	proc
	get_counter
	cmp	ax, rotate_len
	jnb	@@e

	xor	edx, edx
	shl	eax, 16
	mov	ebx, rotate_speed
	div	ebx
	cmp	ax, rotate_lframe
	jz	@@ee

	mov	rotate_frame, ax
	sub	ax, rotate_lframe
	mov	bl, al
	mov	al, rotate_x_angel_dir
	mul	bl
	add	rotate_x_angel, al
	mov	al, rotate_y_angel_dir
	mul	bl
	add	rotate_y_angel, al
	mov	al, rotate_z_angel_dir
	mul	bl
	add	rotate_z_angel, al

	cmp	rotate_delay, 1
	jz	@@j1
	call	clear_temp_seg1
@@j1:
	mov	ax, rotate_frame
	shl	ax, 1
	sub	ax, 130
	mov	rotate_x_pos, ax
	call	rotate
	call	rotate_2lines
	new_screen
	mov	ax, rotate_frame
	mov	rotate_lframe, ax
	cmp	rotate_pos_count, 0
	ja	@@j2
	mov	rotate_y_pos, 100
	jmp	@@ee
@@j2:
	dec	rotate_pos_count
@@ee:
	store_command	61
@@e:
	ret
rotate_progress	endp

; **************************************
rotate_2lines	proc
	mov	si, offset rotate_2lines1
	mov	di, offset rotate_2lines2
	mov	cx, (8*2)/4
@@l1:
	mov	eax, [si]
	mov	[di], eax
	add	si, 4
	add	di, 4
	loop	@@l1
	mov	es, _temp_seg1
	mov	si, offset rotate_2lines2
	mov	cx, 2
	call	aaline
	ret
rotate_2lines	endp

; **************************************
rotate_x_change	proc
	xor	rotate_x_change_s, 1
	jz	@@jj1
	mov	rotate_y_pos, 70
	jmp	@@jj2
@@jj1:
	mov	rotate_y_pos, 130
@@jj2:
	mov	rotate_pos_count, 3
	cmp	rotate_x_angel_dir, 1
	jz	@@j1
	mov	rotate_x_angel_dir, 1
	jmp	@@e
@@j1:
	mov	rotate_x_angel_dir, -1
@@e:
	ret
rotate_x_change_s	db 0
rotate_x_change	endp

rotate_y_change	proc
	xor	rotate_y_change_s, 1
	jz	@@jj1
	mov	rotate_y_pos, 70
	jmp	@@jj2
@@jj1:
	mov	rotate_y_pos, 130
@@jj2:
	mov	rotate_pos_count, 3
	cmp	rotate_y_angel_dir, 1
	jz	@@j1
	mov	rotate_y_angel_dir, 1
	jmp	@@e
@@j1:
	mov	rotate_y_angel_dir, -1
@@e:
	ret
rotate_y_change_s	db 0
rotate_y_change	endp

rotate_z_change	proc
	xor	rotate_z_change_s, 1
	jz	@@jj1
	mov	rotate_y_pos, 70
	jmp	@@jj2
@@jj1:
	mov	rotate_y_pos, 130
@@jj2:
	mov	rotate_pos_count, 3
	cmp	rotate_z_angel_dir, 1
	jz	@@j1
	mov	rotate_z_angel_dir, 1
	jmp	@@e
@@j1:
	mov	rotate_z_angel_dir, -1
@@e:
	ret
rotate_z_change_s	db 0
rotate_z_change	endp

rotate_pauseoff	proc
	mov	rotate_delay, 0
	ret
rotate_pauseoff	endp

; **************************************
rotate_p1p	proc
	call	clear_temp_seg1
	mov	si, offset obj_p1p
	call	do_poison
	store_palette	grey_inv_pal, 0, 256
	mov	rotate_delay, 1
	ret
rotate_p1p	endp

rotate_p2o	proc
	call	clear_temp_seg1
	mov	si, offset obj_p2o
	call	do_poison
	store_palette	grey_pal, 0, 256
	mov	rotate_delay, 1
	ret
rotate_p2o	endp

rotate_p3i	proc
	call	clear_temp_seg1
	mov	si, offset obj_p3i
	call	do_poison
	store_palette	grey_inv_pal, 0, 256
	mov	rotate_delay, 1
	ret
rotate_p3i	endp

rotate_p4s	proc
	call	clear_temp_seg1
	mov	si, offset obj_p4s
	call	do_poison
	store_palette	grey_pal, 0, 256
	mov	rotate_delay, 1
	ret
rotate_p4s	endp

rotate_p5o	proc
	call	clear_temp_seg1
	mov	si, offset obj_p5o
	call	do_poison
	store_palette	grey_inv_pal, 0, 256
	mov	rotate_delay, 1
	ret
rotate_p5o	endp

rotate_p6n	proc
	call	clear_temp_seg1
	mov	si, offset obj_p6n
	call	do_poison
	store_palette	grey_pal, 0, 256
	mov	rotate_delay, 1
	ret
rotate_p6n	endp

; **************************************
rotate	proc
	call	rotate_pixels
	mov	cx, rotate_num_lines
	mov	si, offset rotate_lines
@@l1:
	push	cx si
	mov	di, offset rotate_pixels_dest
	lodsb
	xor	ah, ah
	mov	cx, 6
	mul	cx
	mov	bx, ax
	mov	ax, [di+bx]
	add	ax, rotate_x_pos
	mov	word ptr rotate_line_coor, ax
	mov	ax, [di+bx+2]
	add	ax, rotate_y_pos
	mov	word ptr rotate_line_coor+2, ax
	lodsb
	xor	ah, ah
	mul	cx
	mov	bx, ax
	mov	ax, [di+bx]
	add	ax, rotate_x_pos
	mov	word ptr rotate_line_coor+4, ax
	mov	ax, [di+bx+2]
	add	ax, rotate_y_pos
	mov	word ptr rotate_line_coor+6, ax

	mov	si, offset rotate_line_coor
	mov	di, offset rotate_line_coords
	call	rotate_convert	; point 1 / line 1
	add	si, 4
	add	di, 4
	call	rotate_convert	; point 2 / line 1
	mov	eax, [di]
	add	di, 4
	mov	[di], eax	; point 1 / line 2
	sub	si, 4
	add	di, 4
	call	rotate_convert	; point 2 / line 2
	mov	eax, [di]
	add	di, 4
	mov	[di], eax	; point 1 / line 3
	add	si, 4
	add	di, 4
	call	rotate_convert	; point 2 / line 3

	mov	es, _temp_seg1
	mov	si, offset rotate_line_coords
	mov	cx, 3
	call	aaline

	pop	si cx
	add	si, 2
	loop	@@l1

	ret
rotate	endp
; **************************************
rotate_convert	proc
	mov	ax, 8
	call	random
	sub	ax, 4
	add	ax, [si]
	mov	[di], ax
	mov	ax, 8
	call	random
	sub	ax, 4
	add	ax, [si+2]
	mov	[di+2], ax
	ret
rotate_convert	endp
; **************************************
rotate_pixels	proc
	mov	si, offset rotate_pixels_sour
	mov	di, offset rotate_pixels_dest
	mov	cx, rotate_pixels_num
@@l1:
	push	cx
	push	si di

	mov	di, offset rotate_pixel
	mov	al, rotate_x_angel
	call	rotate_set_angel
	call	rotate_x

	mov	si, offset rotate_pixel
	mov	di, offset rotate_pixel2
	mov	al, rotate_y_angel
	call	rotate_set_angel
	call	rotate_y

	mov	si, offset rotate_pixel2
	pop	di
	mov	al, rotate_z_angel
	call	rotate_set_angel
	call	rotate_z

	pop	si
	add	si, 6
	add	di, 6
	pop	cx
	loop	@@l1
	ret
rotate_pixels	endp

rotate_set_angel	proc
	mov bp, offset rotate_sin
	xor ah, ah
	shl ax, 1
	add bp, ax
	ret
rotate_set_angel	endp


rotate_x_angel		db 0
rotate_y_angel		db 0
rotate_z_angel		db 0
rotate_x_angel_dir	db 1
rotate_y_angel_dir	db 1
rotate_z_angel_dir	db 1

rotate_pixels_num	EQU 14
rotate_pixels_sour:

	dw -20, 20, -20		; 0  -  obj 1  -  14 points
	dw -20, -20, -20	; 1
	dw 20, -20, -20		; 2
	dw 20, 20, -20		; 3
	dw -20, 20, 20		; 4
	dw -20, -20, 20		; 5
	dw 20, -20, 20		; 6
	dw 20, 20, 20		; 7
	dw 0, 120, 0		; 8
	dw -120, 0, 0		; 9
	dw 0, -120, 0		; 10
	dw 120, 0, 0		; 11
	dw 0, 0, -120		; 12
	dw 0, 0, 120		; 13

rotate_num_lines	EQU (6*4) + 12
rotate_lines:
	db 0, 1	; cube	-  obj1  -  6*4 + 12 lines
	db 1, 2
	db 2, 3
	db 3, 0
	db 4, 5
	db 5, 6
	db 6, 7
	db 7, 4
	db 1, 5
	db 2, 6
	db 3, 7
	db 0, 4
	db 0, 8  ; thorn 1
	db 3, 8
	db 7, 8
	db 4, 8
	db 0, 9  ; thorn 2
	db 4, 9
	db 5, 9
	db 1, 9
	db 1, 10 ; thorn 3
	db 2, 10
	db 6, 10
	db 5, 10
	db 2, 11 ; thorn 4
	db 6, 11
	db 7, 11
	db 3, 11
	db 1, 12 ; thorn 5
	db 2, 12
	db 3, 12
	db 0, 12
	db 5, 13 ; thorn 6
	db 6, 13
	db 7, 13
	db 4, 13

rotate_x_pos	dw 50
rotate_y_pos	dw 100

rotate_pixels_dest	dw rotate_pixels_num*6 DUP (0)
rotate_pixel	dw 6 DUP (0)
rotate_pixel2	dw 6 DUP (0)

rotate_line_coor	dw 4 DUP (0)
rotate_line_coords	dw 4*3 DUP (0)

rotate_z	proc
	mov ax, [si+2]
	mov cx, ds:[bp] ; y*sin(a)
	imul cx
        sar ax, 8
	mov bx, ax
	mov ax, [si]
	mov cx, ds:[bp+64*2] ; x*cos(a)
	imul cx
        sar ax, 8
	sub ax, bx
	mov [di], ax
;
	mov ax, [si]
	mov cx, ds:[bp] ; x*sin(a)
	imul cx
        sar ax, 8
	mov bx, ax
	mov ax, [si+2]
	mov cx, ds:[bp+64*2] ; y*cos(a)
	imul cx
        sar ax, 8
	add ax, bx
	mov [di+2], ax
;
	mov ax, [si+4]
	mov [di+4], ax
	ret
rotate_z	endp

rotate_x	proc
	mov ax, [si+4]
	mov cx, ds:[bp] ; z*sin(a)
	imul cx
        sar ax, 8
	mov bx, ax
	mov ax, [si+2]
	mov cx, ds:[bp+64*2] ; y*cos(a)
	imul cx
        sar ax, 8
	sub ax, bx
	mov [di+2], ax
;
	mov ax, [si+2]
	mov cx, ds:[bp] ; y*sin(a)
	imul cx
        sar ax, 8
	mov bx, ax
	mov ax, [si+4]
	mov cx, ds:[bp+64*2] ; z*cos(a)
	imul cx
        sar ax, 8
	add ax, bx
	mov [di+4], ax
;
	mov ax, [si]
	mov [di], ax
	ret
rotate_x	endp

rotate_y	proc
	mov ax, [si+4]
	mov cx, ds:[bp] ; z*sin(a)
	imul cx
        sar ax, 8
	mov bx, ax
	mov ax, [si]
	mov cx, ds:[bp+64*2] ; x*cos(a)
	imul cx
        sar ax, 8
	add ax, bx
	mov [di], ax
;
	mov ax, [si]
	mov cx, ds:[bp] ; x*sin(a)
	imul cx
        sar ax, 8
	mov bx, ax
	mov ax, [si+4]
	mov cx, ds:[bp+64*2] ; y*cos(a)
	imul cx
        sar ax, 8
	add ax, bx
	mov [di+4], ax
;
	mov ax, [si+2]
	mov [di+2], ax
	ret
rotate_y	endp

; **************************************
; random -  returns random number in ax from range 0 - ax
;  input:   ax - range
;  returns: ax - random number
;  kills:   Nothing
random          proc near
                push bx
                push cx
                push dx
                push si
                mov valid, ax

                mov cx, rd_var1
                mov bx, rd_var2
                mov dx, 15ah
                mov ax, 4e35h

                xchg si, ax
                xchg dx, ax
                test ax, ax
                je rd_j1
                mul bx
rd_j1:          jcxz rd_j2
                xchg cx, ax
                mul si
                add ax, cx
rd_j2:          xchg si, ax
                mul bx
                add dx, si

                add ax, 1
                adc dx, 0
                mov rd_var1, dx
                mov rd_var2, ax
                mov ax, dx

                xor dx, dx
                div valid
                mov ax, dx

                pop si
                pop dx
                pop cx
                pop bx
                ret
rd_var1         dw 1+24
rd_var2         dw 0+234
valid           dw 0
random          endp

INCLUDE		aaline.inc

pal:
	i = 63
	rept 64
		db i, i, i
		i = i - 1
	endm
	i = 0
	rept 64
		db 0, 0, 0
		i = i + 1
	endm

rotate_sin:
dw 0,6,12,18,25,31,37,43,50,56,62,68,74,80,86,92,98,104,109,115,121,126,132,137,142,147,153,158,163,167,172,177,181,186,190,194,198,202,206,210,213,217,220,223,226,229,232,234,237,239
dw 241,244,245,247,249,250,252,253,254,255,255,256,256,256,256,256,256,256,255,255,254,253,252,250,249,247,245,244,241,239,237,234,232,229,226,223,220,217,213,210,206,202,198,194,190,186,181,177,172,167
dw 163,158,153,147,142,137,132,126,121,115,109,104,98,92,86,80,74,68,62,56,50,43,37,31,25,18,12,6,0,65530,65524,65518,65511,65505,65499,65493,65486,65480,65474,65468,65462,65456,65450,65444,65438,65432,65427,65421,65415,65410
dw 65404,65399,65394,65389,65383,65378,65373,65369,65364,65359,65355,65350,65346,65342,65338,65334,65330,65326,65323,65319,65316,65313,65310,65307,65304,65302,65299,65297,65295,65292,65291,65289,65287,65286,65284,65283,65282,65281,65281,65280,65280,65280,65280,65280,65280,65280,65281,65281,65282,65283
dw 65284,65286,65287,65289,65291,65292,65295,65297,65299,65302,65304,65307,65310,65313,65316,65319,65323,65326,65330,65334,65338,65342,65346,65350,65355,65359,65364,65369,65373,65378,65383,65389,65394,65399,65404,65410,65415,65421,65427,65432,65438,65444,65450,65456,65462,65468,65474,65480,65486,65493
dw 65499,65505,65511,65518,65524,65530,0,6,12,18,25,31,37,43,50,56,62,68,74,80,86,92,98,104,109,115,121,126,132,137,142,147,153,158,163,167,172,177,181,186,190,194,198,202,206,210,213,217,220,223
dw 226,229,232,234,237,239,241,244,245,247,249,250,252,253,254,255,255,256,256,256