; ******************************************************************************

end_frame2_pos	EQU 10
end_frame3_pos	EQU -2
end_frame4_pos	EQU 23
end_frame5_pos	EQU -11
end_frame6_pos	EQU 18
end_frame7_pos	EQU 10
end_frame8_pos	EQU 0

end_frames_bold		dw 50, 1, 320-50-50, 198
end_frames_soften	dw 50, 1, 320-50-50, 198

end_random_lines	proc
;	mov	ax, fs
	mov	es, _temp_seg1
	mov	cx, 20
@@l1:
	push	cx
	mov	ax, 49
	call	random
	mov	word ptr [end_random_lines_data], ax
	mov	ax, 199
	call	random
	mov	word ptr [end_random_lines_data+2], ax
	mov	ax, 49
	call	random
	add	ax, 320-50
	mov	word ptr [end_random_lines_data+4], ax
	mov	ax, 199
	call	random
	mov	word ptr [end_random_lines_data+6], ax
	mov	si, offset end_random_lines_data
	mov	cx, 1
	call	aaline
	pop	cx
	loop	@@l1
	new_screen
	ret
end_random_lines_data	dw 0, 0, 0, 0
end_random_lines	endp

end_frame1	proc
	call	clear_temp_seg1
	call	clear_temp_seg2
	mov	fs, _temp_seg2
	mov	es, _temp_seg2
	mov	di, 100*320 + 160
	mov	cx, 5
	call	circle
	mov	di, 99*320+129
	mov	cx, 20
	call	circle
	call	end_uparrow_full
	call	end_leftarrow_empty
	call	end_rightarrow_empty
	call	end_downarrow_empty
	call	end_circlemiddle
	call	end_box1
	call	clear_temp_seg3
	mov	gs, _temp_seg3
	mov	si, offset end_frames_bold
	call	bold
;	call	clear_temp_seg4		; end_both
	mov	fs, _temp_seg3
	mov	gs, _temp_seg1
	mov	si, offset end_frames_soften
	call	soften2
	call	end_random_lines
	new_screen
	store_palette	grey_pal, 0, 64
	ret
end_frame1	endp

end_frame2	proc
	call	clear_temp_seg1
	call	clear_temp_seg2
	mov	fs, _temp_seg2
	mov	es, _temp_seg2
	mov	di, 100*320 + 160
	mov	cx, 5
	call	circle
	mov	di, 106*320+130 ; 108*320+130
	mov	cx, 20
	call	circle
	call	end_uparrow_empty
	call	end_leftarrow_full
	call	end_rightarrow_empty
	call	end_downarrow_empty
	call	end_circlemiddle
	call	end_box2
	call	clear_temp_seg4
	mov	gs, _temp_seg4
	mov	si, offset end_frames_bold
	call	bold
	mov	fs, _temp_seg3
	mov	gs, _temp_seg4
	xor	si, si
	mov	di, end_frame2_pos
	call	end_both
	mov	fs, _temp_seg5
	mov	gs, _temp_seg1
	mov	si, offset end_frames_soften
	call	soften2
	call	end_random_lines
	new_screen
	ret
end_frame2	endp

end_frame3	proc
	call	clear_temp_seg1
	call	clear_temp_seg2
	mov	fs, _temp_seg2
	mov	es, _temp_seg2
	mov	di, 100*320 + 160
	mov	cx, 5
	call	circle
	mov	di, 113*320+132 ; 115*320+133
	mov	cx, 20
	call	circle
	call	end_uparrow_empty
	call	end_leftarrow_empty
	call	end_downarrow_full
	call	end_rightarrow_empty
	call	end_circlemiddle
	call	end_box1
	call	clear_temp_seg3
	mov	gs, _temp_seg3
	mov	si, offset end_frames_bold
	call	bold
	mov	fs, _temp_seg4
	mov	gs, _temp_seg3
	mov	si, end_frame2_pos
	mov	di, end_frame3_pos
	call	end_both
	mov	fs, _temp_seg5
	mov	gs, _temp_seg1
	mov	si, offset end_frames_soften
	call	soften2
	call	end_random_lines
	new_screen
	ret
end_frame3	endp

end_frame4	proc
	call	clear_temp_seg1
	call	clear_temp_seg2
	mov	fs, _temp_seg2
	mov	es, _temp_seg2
	mov	di, 100*320 + 160
	mov	cx, 5
	call	circle
	mov	di, 118*320+135 ; 121*320+138
	mov	cx, 20
	call	circle
	call	end_uparrow_empty
	call	end_leftarrow_empty
	call	end_downarrow_empty
	call	end_rightarrow_full
	call	end_circlemiddle
	call	end_box2
	call	clear_temp_seg4
	mov	gs, _temp_seg4
	mov	si, offset end_frames_bold
	call	bold
	mov	fs, _temp_seg3
	mov	gs, _temp_seg4
	mov	si, end_frame3_pos
	mov	di, end_frame4_pos
	call	end_both
	mov	fs, _temp_seg5
	mov	gs, _temp_seg1
	mov	si, offset end_frames_soften
	call	soften2
	call	end_random_lines
	new_screen
	ret
end_frame4	endp

end_frame5	proc
	call	clear_temp_seg1
	call	clear_temp_seg2
	mov	fs, _temp_seg2
	mov	es, _temp_seg2
	mov	di, 100*320 + 160
	mov	cx, 5
	call	circle
	mov	di, 123*320 + 140 ; 126*320+144
	mov	cx, 20
	call	circle
	call	end_uparrow_full
	call	end_leftarrow_empty
	call	end_downarrow_empty
	call	end_rightarrow_empty
	call	end_circlemiddle
	call	end_box1
	call	clear_temp_seg3
	mov	gs, _temp_seg3
	mov	si, offset end_frames_bold
	call	bold
	mov	fs, _temp_seg4
	mov	gs, _temp_seg3
	mov	si, end_frame4_pos
	mov	di, end_frame5_pos
	call	end_both
	mov	fs, _temp_seg5
	mov	gs, _temp_seg1
	mov	si, offset end_frames_soften
	call	soften2
	call	end_random_lines
	new_screen
	ret
end_frame5	endp

end_frame6	proc
	call	clear_temp_seg1
	call	clear_temp_seg2
	mov	fs, _temp_seg2
	mov	es, _temp_seg2
	mov	di, 100*320 + 160
	mov	cx, 5
	call	circle
	mov	di, 127*320+147 ; 129*320+151
	mov	cx, 20
	call	circle
	call	end_uparrow_empty
	call	end_leftarrow_full
	call	end_downarrow_empty
	call	end_rightarrow_empty
	call	end_circlemiddle
	call	end_box2
	call	clear_temp_seg4
	mov	gs, _temp_seg4
	mov	si, offset end_frames_bold
	call	bold
	mov	fs, _temp_seg3
	mov	gs, _temp_seg4
	mov	si, end_frame5_pos
	mov	di, end_frame6_pos
	call	end_both
	mov	fs, _temp_seg5
	mov	gs, _temp_seg1
	mov	si, offset end_frames_soften
	call	soften2
	call	end_random_lines
	new_screen
	ret
end_frame6	endp

end_frame7	proc
	call	clear_temp_seg1
	call	clear_temp_seg2
	mov	fs, _temp_seg2
	mov	es, _temp_seg2
	mov	di, 100*320 + 160
	mov	cx, 5
	call	circle
	mov	di, 130*320+154 ; 131*320+161
	mov	cx, 20
	call	circle
	call	end_uparrow_empty
	call	end_leftarrow_empty
	call	end_downarrow_full
	call	end_rightarrow_empty
	call	end_circlemiddle
	call	end_box1
	call	clear_temp_seg3
	mov	gs, _temp_seg3
	mov	si, offset end_frames_bold
	call	bold
	mov	fs, _temp_seg4
	mov	gs, _temp_seg3
	mov	si, end_frame6_pos
	mov	di, end_frame7_pos
	call	end_both
	mov	fs, _temp_seg5
	mov	gs, _temp_seg1
	mov	si, offset end_frames_soften
	call	soften2
	call	end_random_lines
	new_screen
	ret
end_frame7	endp

end_frame8	proc
	call	clear_temp_seg1
	call	clear_temp_seg2
	mov	fs, _temp_seg2
	mov	es, _temp_seg2
	mov	di, 100*320 + 160
	mov	cx, 5
	call	circle
	mov	di, 131*320+161 ; 129*320+151
	mov	cx, 20
	call	circle
	call	end_uparrow_empty
	call	end_leftarrow_empty
	call	end_downarrow_empty
	call	end_rightarrow_full
	call	end_circlemiddle
	call	end_box2
	call	clear_temp_seg4
	mov	gs, _temp_seg4
	mov	si, offset end_frames_bold
	call	bold
	mov	fs, _temp_seg3
	mov	gs, _temp_seg4
	mov	si, end_frame7_pos
	mov	di, end_frame8_pos
	call	end_both
	mov	fs, _temp_seg5
	mov	gs, _temp_seg1
	mov	si, offset end_frames_soften
	call	soften2
	call	end_random_lines
	new_screen
	ret
end_frame8	endp

; ******************************************************************************
end_both	proc
	mov	es, _temp_seg5
	mov	cx, 64000
	xor	bx, bx
@@l1:
	mov	al, gs:[di]
	or	al, al
	jnz	@@j1
	mov	al, fs:[si]
	shr	al, 1
@@j1:
	mov	es:[bx], al
	inc	bx
	inc	di
	inc	si
	dec	cx
	jnz	@@l1
	ret
end_both	endp

; ******************************************************************************
end_circlemiddle	proc
	call	drawobject c, offset obj_end_circlemiddle, 156, 94, 63
	ret
end_circlemiddle	endp

end_uparrow_full	proc
	call	drawobject c, offset obj_end_uparrow_full, 138, 10
	ret
end_uparrow_full	endp

end_leftarrow_full	proc
	call	drawobject c, offset obj_end_leftarrow_full, 73, 80
	ret
end_leftarrow_full	endp

end_downarrow_full	proc
	call	drawobject c, offset obj_end_downarrow_full, 138, 160
	ret
end_downarrow_full	endp

end_rightarrow_full	proc
	call	drawobject c, offset obj_end_rightarrow_full, 220, 80
	ret
end_rightarrow_full	endp

end_leftarrow_empty	proc
	mov	si, offset end_leftarrow_empty_data
	call	end_line
	ret
end_la_x	EQU 73
end_la_y	EQU 80
end_leftarrow_empty_data:
	dw 27+end_la_x, 0+end_la_y, 27+end_la_x, 40+end_la_y, 27+end_la_x, 0+end_la_y, 0+end_la_x, 20+end_la_y, 0+end_la_x, 20+end_la_y, 27+end_la_x, 40+end_la_y
	dw 26+end_la_x, 0+end_la_y, 26+end_la_x, 40+end_la_y
end_leftarrow_empty	endp

end_rightarrow_empty	proc
	mov	si, offset end_rightarrow_empty_data
	call	end_line
	ret
end_ra_x	EQU 220
end_ra_y	EQU 80
end_rightarrow_empty_data:
	dw 0+end_ra_x, 0+end_ra_y, 30+end_ra_x, 20+end_ra_y, 0+end_ra_x, 40+end_ra_y, 30+end_ra_x, 20+end_ra_y
	dw 0+end_ra_x, 0+end_ra_y, 0+end_ra_x, 40+end_ra_y, 1+end_ra_x, 0+end_ra_y, 1+end_ra_x, 40+end_ra_y
end_rightarrow_empty	endp

end_downarrow_empty	proc
	mov	si, offset end_downarrow_empty_data
	call	end_line
	ret
end_da_x	EQU 138
end_da_y	EQU 159
end_downarrow_empty_data:
	dw 0+end_da_x, 0+end_da_y, 22+end_da_x, 30+end_da_y, 44+end_da_x, 0+end_da_y, 22+end_da_x, 30+end_da_y
	dw 0+end_da_x, 0+end_da_y, 44+end_da_x, 0+end_da_y, 0+end_da_x, 1+end_da_y, 44+end_da_x, 1+end_da_y
end_downarrow_empty	endp

end_uparrow_empty	proc
	mov	si, offset end_uparrow_empty_data
	call	end_line
	ret
end_ua_x	EQU 138
end_ua_y	EQU 11
end_uparrow_empty_data:
	dw 0+end_ua_x, 30+end_ua_y, 22+end_ua_x, 0+end_ua_y, 44+end_ua_x, 30+end_ua_y, 22+end_ua_x, 0+end_ua_y
	dw 0+end_ua_x, 30+end_ua_y, 44+end_ua_x, 30+end_ua_y, 0+end_ua_x, 29+end_ua_y, 44+end_ua_x, 29+end_ua_y
end_uparrow_empty	endp

end_box1		proc
	mov	si, offset end_box1_data
	call	end_line
	ret
end_b1_x	EQU 160-15
end_b1_y	EQU 65
end_box1_data:
	dw 0+end_b1_x, 0+end_b1_y, 30+end_b1_x, 0+end_b1_y, 0+end_b1_x, 15+end_b1_y, 30+end_b1_x, 15+end_b1_y
	dw 0+end_b1_x, 0+end_b1_y, 0+end_b1_x, 15+end_b1_y, 30+end_b1_x, 0+end_b1_y, 30+end_b1_x, 15+end_b1_y
end_box1		endp

end_box2		proc
	mov	si, offset end_box2_data
	call	end_line
	ret
end_b2_x	EQU 160-15/2
end_b2_y	EQU 58
end_box2_data:
	dw 0+end_b2_x, 0+end_b2_y, 15+end_b2_x, 0+end_b2_y, 0+end_b2_x, 30+end_b2_y, 15+end_b2_x, 30+end_b2_y
	dw 0+end_b2_x, 0+end_b2_y, 0+end_b2_x, 30+end_b2_y, 15+end_b2_x, 0+end_b2_y, 15+end_b2_x, 30+end_b2_y
end_box2		endp

end_line	proc
	mov	cx, (4*4*2)/2
	mov	di, offset end_line_tmp
@@l1:
	mov	eax, [si]
	mov	[di], eax
	add	si, 4
	add	di, 4
	dec	cx
	jnz	@@l1
	mov	si, offset end_line_tmp
	mov	cx, 4
	call	aaline
	ret
end_line_tmp	dw 4*4*2 DUP (0)
end_line	endp

; ******************************************************************************
circle	proc
	xor	bx, bx
@@l1:
	push	di
	mov	ax, [offset rotate_sin + bx]
	cwd
	idiv	cx
	add	di, ax
	mov	ax, [offset rotate_sin + bx + 128]
	cwd
	idiv	cx
	mov	dx, 320
	imul	dx
	add	di, ax
	mov	byte ptr fs:[di], 63
	pop	di
	add	bx, 2
	cmp	bx, 256*2
	jnz	@@l1
	ret

circle	endp

; ******************************************************************************
; si - offset soften uatas
; [si] x - [si+2] y - [si+4] len_x - [si+6] len_y
; fs = source seg : gs = dest seg
bold	proc
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
	xor	al, al
	add	al, fs:[di-1]
	jnz	@@st
	add	al, fs:[di+1]
	jnz	@@st
	add	al, fs:[di-320]
	jnz	@@st
	add	al, fs:[di+320]
	jnz	@@st
	mov	al, fs:[di-320-1]
	jnz	@@st
	add	al, fs:[di-320+1]
	jnz	@@st
	add	al, fs:[di+320-1]
	jnz	@@st
	add	al, fs:[di+320+1]
	jnz	@@st
	jmp	@@nost
@@st:
	mov	byte ptr gs:[di], 63
@@nost:
	inc	di
	loop	@@l1
	pop	di
	add	di, 320
	pop	cx
	loop	@@l2
	ret

bold	endp

; **************************************
;	AL = SQRT(AX)

sqrt:
	push bx cx dx
	cmp ax, 0
	jz sq_j4
	xor ch, ch
	mov bx, ax
	mov cl, 18
sq_j1:
	dec cl
	shl bx, 1
	jnc sq_j1

	shr cl, 1
	mov dl, 128
	rol dl, cl

	mov bx, ax
	or ch, dl
sq_j3:
	shr dl, 1
	cmp dl, 0
	jz sq_j2
	mov al, ch
	or al, dl
	mul al
	cmp ax, bx
	ja sq_j3
	or ch, dl
	jmp sq_j3
sq_j2:
	mov al, ch
sq_j4:
	pop dx cx bx
	ret

credits_init	proc
	in	al, 40h
	and	al, 2
	shr	al, 1
	mov	credits_status, al
	ret
credits_init	endp

credits_both_names1	proc
	cmp	credits_status, 1
	jz	@@j1
	call	credits_both_names_zden
	ret
@@j1:
	call	credits_both_names_moshe
	ret
credits_both_names1	endp

credits_both_names2	proc
	cmp	credits_status, 1
	jz	@@j1
	call	credits_both_names_moshe
	ret
@@j1:
	call	credits_both_names_zden
	ret
credits_both_names2	endp

credits_both_names_zden	proc
	call	clear_temp_seg1
	call	clear_temp_seg2
	mov	es, _temp_seg2
	call	drawobject c, offset obj_zd, 30-5, 2, 63
	call	drawobject c, offset obj_en, 160-5, 2, 63
	call	drawobject c, offset obj_mos, 2, 100, 63
	call	drawobject c, offset obj_he, 185, 100, 63
	mov	fs, _temp_seg2
	mov	gs, _temp_seg1
	mov	si, offset credits_soften_both
	call	soften
	new_screen
	ret
credits_both_names_zden	endp

credits_both_names_moshe	proc
	call	clear_temp_seg1
	call	clear_temp_seg2
	mov	es, _temp_seg2
	call	drawobject c, offset obj_mos, 2, 0, 63
	call	drawobject c, offset obj_he, 185, 0, 63
	call	drawobject c, offset obj_zd, 30-5, 100, 63
	call	drawobject c, offset obj_en, 160-5, 100, 63
	mov	fs, _temp_seg2
	mov	gs, _temp_seg1
	mov	si, offset credits_soften_both
	call	soften
	new_screen
	ret
credits_both_names_moshe	endp

credits_name1	proc
	cmp	credits_status, 1
	jz	@@j1
	call	credits_name_zden
	ret
@@j1:
	call	credits_name_moshe
	ret
credits_name1	endp

credits_name2	proc
	cmp	credits_status, 1
	jz	@@j1
	call	credits_name_moshe
	ret
@@j1:
	call	credits_name_zden
	ret
credits_name2	endp

credits_name_zden	proc
	call	clear_temp_seg1
	call	clear_temp_seg2
	mov	es, _temp_seg2
	call	drawobject c, offset obj_zd, 30-5, 50, 63
	call	drawobject c, offset obj_en, 160-5, 50, 63
	mov	fs, _temp_seg2
	mov	gs, _temp_seg1
	mov	si, offset credits_soften_one
	call	soften
	new_screen
	ret
credits_name_zden	endp

credits_name_moshe	proc
	call	clear_temp_seg1
	call	clear_temp_seg2
	mov	es, _temp_seg2
	call	drawobject c, offset obj_mos, 2, 50, 63
	call	drawobject c, offset obj_he, 185, 50, 63
	mov	fs, _temp_seg2
	mov	gs, _temp_seg1
	mov	si, offset credits_soften_one
	call	soften
	new_screen
	ret
credits_name_moshe	endp

credits_up_xor	proc
	mov	fs, _temp_seg1
	mov	eax, 03f3f3f3fh
	xor	si, si
	mov	cx, 32000/4
@@l1:
	xor	fs:[si], eax
	add	si, 4
	dec	cx
	jnz	@@l1
	ret
credits_up_xor	endp

credits_down_xor	proc
	mov	fs, _temp_seg1
	mov	eax, 03f3f3f3fh
	mov	si, 32000
	mov	cx, 32000/4
@@l1:
	xor	fs:[si], eax
	add	si, 4
	dec	cx
	jnz	@@l1
	ret
credits_down_xor	endp

credits_mid_xor	proc
	mov	fs, _temp_seg1
	mov	eax, 03f3f3f3fh
	mov	si, 16000
	mov	cx, 32000/4
@@l1:
	xor	fs:[si], eax
	add	si, 4
	dec	cx
	jnz	@@l1
	ret
credits_mid_xor	endp

credits_soften_both	dw 2, 2, 320-4, 200-4
credits_soften_one	dw 2, 40, 320-4, 100+20
credits_status	db 0

end_star	proc
	call	clear_temp_seg1
	cmp	end_star_status, 1
	jz	@@e
	mov	es, _temp_seg1
	mov	end_star_add, 10+50
	mov	cx, 32 ; +32
@@l1:
	push	cx
	call	end_star_ray
	pop	cx
	loop	@@l1
	mov	end_star_add, 10
	mov	cx, 32 ; +32
@@l2:
	push	cx
	call	end_star_ray
	pop	cx
	loop	@@l2

	store_command	97
@@e:
	new_screen
	ret
end_star	endp

end_star_ray	proc
	mov	word ptr [end_star_data], 160
	mov	word ptr [end_star_data+2], 100
	mov	ax, 256
	call	random
	mov	bx, ax
	shl	bx, 1
	add	bx, offset rotate_sin
	mov	ax, 60
	call	random
	add	ax, end_star_add
	mov	cx, ax

	mov	ax, word ptr [bx]
	imul	cx
	sar	ax, 8
	add	ax, 160
	mov	word ptr [end_star_data+4], ax

	mov	ax, word ptr [bx+128]
	imul	cx
	sar	ax, 8
	add	ax, 100
	mov	word ptr [end_star_data+6], ax

	mov	si, offset end_star_data
	mov	cx, 1
	call	aaline
	ret
end_star_ray	endp
end_star_data	dw 4 DUP (0)
end_star_status	db 0
end_star_add	dw 0

end_star_stop	proc
	mov	end_star_status, 1
	ret
end_star_stop	endp