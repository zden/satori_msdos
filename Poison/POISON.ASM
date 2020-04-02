; === P@!$0Ô *******************************************************************
code	segment
	assume cs:code, ds:code
	.386
	jumps
	locals
include	poison.mac

; === EXTERNS ******************************************************************
extrn	module
; === CODE *********************************************************************
start:
	cld
	mov	ax, _stack
	mov	ss, ax
	mov	sp, offset _stack+1024-4
	mov	ax, ds:[2]
	mov	cs:[mem_top], ax
	push	ds
	pop	ax
	mov	cs:[dsp_seg], ax
	call	check_command_line

	push	cs
	pop	ds
	push	cs
	pop	es

	call	setup
	call	basic_inits
	call	setup_card

	call	poison_wait_1st
	cmp	sound_card, 1
	jz	@@sb1
	call	InitPlayer
@@sb1:
	call	poison_wait
	call	basic_precalcs
	cmp	sound_card, 1
	jz	@@sb2
	call	StartPlaying
	jmp	@@sb22
@@sb2:
	call	StartPlaying_sb
@@sb22:
	call	install_handlers
again:
;	reset_counter2
	call	check_commands
	call	screen_manager
	call	do_kaos_screen

;	mov	cx, 7+10
;@@l1:
;	push	cx
;	mov	cx, 64000
;@@l2:
;	nop
;	loop	@@l2
;	pop	cx
;	loop	@@l1

;	get_counter2
;	cmp	ax, 120
;	jb	@@noflow
;	mov	overflow, 1
;	mov	quit, 1
;@@noflow:

	cmp	quit, 0
	jz	again

	call	uninstall_handlers
	mov	ax, 3
	int	10h
	cmp	sound_card, 1
	jz	@@sb3
	call	StopPlaying
	jmp	@@sb33
@@sb3:
	call	StopPlaying_sb
@@sb33:

	cmp	overflow, 1
	jz	@@of
	call	end_msg
	mov	ax, 4c00h
	int	21h

@@of:
	mov	bx, offset overflow_msg
	call	print_msg
	mov	ax, 4c01h
	int	21h

check_command_line	proc
	xor	ch, ch
	mov	cl, ds:[128]
	or	cx, cx
	jz	@@ncl
	mov	si, 129
@@cll1:
	cmp	byte ptr [si], 'i'
	jnz	@@ni
	mov	cs:[invert], 1
@@ni:
	cmp	byte ptr [si], 'n'
	jnz	@@nk
	mov	cs:[kaos], 1
@@nk:
	inc	si
	loop	@@cll1
@@ncl:
	ret
check_command_line	endp

do_kaos_screen	proc
	cmp	kaos, 0
	jz	@@e
	cmp	kaos_status, 0
	jz	@@j1
	mov	ax, @@v1
	mov	rd_var1, ax
	mov	ax, @@v2
	mov	rd_var2, ax
	call	kaos_screen
@@j1:
	mov	ax, rd_var1
	mov	@@v1, ax
	mov	ax, rd_var2
	mov	@@v2, ax
	call	kaos_screen
	mov	kaos_status, 1
@@e:
	ret
@@v1	dw 0
@@v2	dw 0
do_kaos_screen	endp

kaos_screen	proc
	mov	ax, 0a000h
	mov	gs, ax
	mov	cx, 64
@@kl1:
	mov	ax, 64000
	call	random
	mov	si, ax
	push	cx
	and	cx, 111111b
	inc	cl
@@kl2:
	mov	ax, 16
	call	random
	xor	byte ptr gs:[si], al
	inc	si
	loop	@@kl2
	pop	cx
	loop	@@kl1
	ret
kaos_screen	endp

overflow_msg	db '≠ MÍ[H≥ÔÓ È˚Ó©üúÈW !',13,10,0
quit_msg	db '˘ pÂ≠$Â„ - õÂpò©≠gh≈ (õ) mcmxcv - ·ò ',0
quit_zden	db 'zÎÓ„',0
quit_moshe	db 'mÂ$hÓ',0
quit_&		db ' & ',0
quit_end	db ' ˘',13, 10, 13, 10, 0
setup_msg	db '1- gÊ$ 2- $· 0- qÊ≠≈',13,10,0
setup_irq_msg	db '≠©q: 2 3 5 7',13,10,0

setup_card	proc
	cmp	sound_card, 1
	jz	@@sb
	call	gus_detect
	jc	@@nogus_e
	mov	bx, ax
	mov	ax, seg _code
	mov	fs, ax
	mov	fs:[offset BasePort], bx
	ret
@@sb:
	call	getSBport
;	call	getIRQ&DMA
	mov	ax, seg __code
	mov	fs, ax
	mov	ax, SBport
	mov	fs:[BaseAddress], ax
	mov	al, SBIRQ
	mov	fs:[IntNumber], al
	ret

@@nogus_e:
	mov	bx, offset nogus_msg
	jmp	@@j
@@nosb_e:
	mov	dx, offset nosb_msg
@@j:
	mov	ax, 3h
	int	10h
	call	print_msg
	mov	ax, 4c01h
	int	21h

nogus_msg	db '≠ ÔÈ Gö$ - ÔÈ $HÈW !', 13, 10, 0
nosb_msg	db '≠ ÔÈ $· - ÔÈ $HÈW !', 13, 10, 0
setup_card	endp

setup	proc
	mov	bx, offset setup_msg
	call	print_msg
@@j1:
	mov	ah, 0
	int	16h
	cmp	al, '0'
	jz	@@end
	cmp	al, '1'
	jz	@@e
	cmp	al, '2'
	jnz	@@j1
	mov	sound_card, 1
	mov	bx, offset setup_irq_msg
	call	print_msg
@@sba:
	mov	ah, 0
	int	16h
	cmp	al, '2'
	jz	@@sb2
	cmp	al, '3'
	jz	@@sb3
	cmp	al, '5'
	jz	@@sb5
	cmp	al, '7'
	jnz	@@sba
	mov	SBIRQ, 7
	jmp	@@e
@@sb2:
	mov	SBIRQ, 2
	jmp	@@e
@@sb3:
	mov	SBIRQ, 3
	jmp	@@e
@@sb5:
	mov	SBIRQ, 5
@@e:
	ret
@@end:
	mov	ax, 4c00h
	int	21h
setup	endp

end_msg	proc
	mov	bx, offset quit_msg
	call	print_msg
	in	al, 40h
	and	al, 2
	shr	al, 1
	jnz	@@m
	mov	bx, offset quit_zden
	call	print_msg
	mov	bx, offset quit_&
	call	print_msg
	mov	bx, offset quit_moshe
	call	print_msg
	jmp	@@e
@@m:
	mov	bx, offset quit_moshe
	call	print_msg
	mov	bx, offset quit_&
	call	print_msg
	mov	bx, offset quit_zden
	call	print_msg
@@e:
	mov	bx, offset quit_end
	call	print_msg
	ret
end_msg	endp

print_msg	proc
	mov	ah, 2
@@a:
	mov	dl, [bx]
	or	dl, dl
	jz	@@e
	int	21h
	inc	bx
	jmp	@@a
@@e:
	ret
print_msg	endp

; === COMMANDS DEFINTIONS ******************************************************
function:
; palette stuff
	db 1
	dw offset black_palette
	db 2
	dw offset white_palette
	db 3
	dw offset grey_palette
	db 4
	dw offset grey2_palette
	db 5
	dw offset grey3_palette
	db 6
	dw offset grey4_palette
	db 7
	dw offset grey_inv_palette
	db 8
	dw offset flash1_palette
	db 9
	dw offset flash2_palette
	db 10
	dw offset flash3_palette
	db 11
	dw offset hardflash
	db 12
	dw offset hardflash_stop
	db 13
	dw offset flash4_palette
	db 14
	dw offset hardflash_zeroint
	db 15
	dw offset hardflash_intup
	db 16
	dw offset hardflash_fullint
	db 17
	dw offset hardflash_intdown
	db 18
	dw offset head_down
	db 19
	dw offset hardflash2

; show poison name
	db 30
	dw offset poison_1
	db 31
	dw offset poison_2
	db 32
	dw offset poison_3
	db 33
	dw offset poison_4
	db 34
	dw offset poison_5
	db 35
	dw offset poison_6
	db 36
	dw offset poison
; show signs
	db 20
	dw offset intro_sign1
	db 21
	dw offset intro_sign2
	db 22
	dw offset intro_sign3
;	db 23
;	dw offset intro_sign4
	db 26
	dw offset intro_fade
;	db 27
;	dw offset kaos_sign
;	db 28
;	dw offset kaos_sign_stop
; shakes, crash & deviations :)
	db 40
	dw offset shake1
	db 41
	dw offset shake2
	db 42
	dw offset shake_boom
	db 43
	dw offset shake3
	db 45
	dw offset xor_crash
	db 46
	dw offset xor_crash_cont
	db 47
	dw offset xor_crash_cont_stop
	db 49
	dw offset bassdrum
	db 50
	dw offset bassdrum_progress
	db 51
	dw offset synchro_crash
; part - rotate
	db 60
	dw offset rotate_start
	db 61
	dw offset rotate_progress
	db 62
	dw offset rotate_x_change
	db 63
	dw offset rotate_y_change
	db 64
	dw offset rotate_z_change
	db 65
	dw offset rotate_pauseoff
	db 66
	dw offset rotate_p1p
	db 67
	dw offset rotate_p2o
	db 68
	dw offset rotate_p3i
	db 69
	dw offset rotate_p4s
	db 70
	dw offset rotate_p5o
	db 71
	dw offset rotate_p6n
	db 74
	dw offset rotate_sign1_init
	db 75
	dw offset rotate_sign1_progress
; part - end
	db 80
	dw offset end_frame1
	db 81
	dw offset end_frame2
	db 82
	dw offset end_frame3
	db 83
	dw offset end_frame4
	db 84
	dw offset end_frame5
	db 85
	dw offset end_frame6
	db 86
	dw offset end_frame7
	db 87
	dw offset end_frame8
	db 88
	dw offset end_random_lines
; part - credits
	db 90
	dw offset credits_init
	db 91
	dw offset credits_both_names1
	db 92
	dw offset credits_both_names2
	db 93
	dw offset credits_name1
	db 94
	dw offset credits_name2
	db 95
	dw offset credits_up_xor
	db 96
	dw offset credits_down_xor
	db 97
	dw offset end_star
	db 98
	dw offset end_star_stop
	db 99
	dw offset credits_mid_xor
; part - lights
	db 100
	dw offset light_start
	db 101
	dw offset light_progress
	db 102
	dw offset light_p
	db 103
	dw offset light_o
	db 104
	dw offset light_i
	db 105
	dw offset light_s
	db 106
	dw offset light_o2
	db 107
	dw offset light_n
	db 108
	dw offset light_unpause
;	db 109
;	dw offset light_rotdir_toggle
;	db 110
;	dw offset light_ligdir_toggle
;	db 111
;	dw offset light_forward
	db 112
	dw offset light_switch1_on
	db 113
	dw offset light_flash
	db 114
	dw offset light_shake
	db 115
	dw offset light_head_down_funct
	db 116
	dw offset light_next_turn_funct
; part - plasma
	db 120
	dw offset plasma_start
	db 121
	dw offset plasma_progress
	db 122
	dw offset plasma_sin_destruct
	db 123
	dw offset plasma_normal
	db 124
	dw offset plasma_xor
	db 125
	dw offset plasma_flash
	db 126
	dw offset plasma_grey
	db 127
	dw offset plasma_mid_destruct
;	db 128
;	dw offset plasma_pump_here
	db 128
	dw offset plasma_pic_next_line
	db 129
	dw offset plasma_nou
	db 130
	dw offset plasma_pic_mirror_toggle
; part - blur rotator
	db 140
	dw offset blur_toggle1
	db 141
	dw offset blur_progress1
	db 142
	dw offset blur_triton_toggle
	db 143
	dw offset plasma_pump_here
	db 144
	dw offset blur_deuxton_flash_toggle
	db 145
	dw offset blur_normal_pal
	db 146
	dw offset blur_plasma_pal
	db 147
	dw offset blur_plasma_pal_flash
	db 149
	dw offset blur_do_stop
	db 150
	dw offset blur_toggle2
	db 151
	dw offset blur_progress2
	db 152
	dw offset blur_part2_claps
	db 153
	dw offset blur_clear
	db 154
	dw offset blur_palette
; intro dots
	db 160
	dw offset dots_part1
	db 161
	dw offset dots_part1_progress
	db 162
	dw offset dots_part2
	db 163
	dw offset dots_part2_progress
	db 164
	dw offset dots_part3
	db 165
	dw offset dots_part3_progress
; part - tunel
	db 170
	dw offset tunel_toggle
	db 171
	dw offset tunel_progress
	db 172
	dw offset tunel_windows_toggle
	db 173
	dw offset tunel_sick_toggle
	db 174
	dw offset tunel_spiral_toggle
	db 175
	dw offset tunel_leaving_toggle
; part - tunnel
	db 180
	dw offset tunnel_start
	db 181
	dw offset tunnel_progress
	db 182
	dw offset tunnel_norm_pal
	db 183
	dw offset tunnel_flash1_pal
	db 184
	dw offset tunnel_flash2_pal
	db 185
	dw offset tunnel_flash3_pal
	db 186
	dw offset tunnel_flash4_pal
	db 187
	dw offset tunnel_norm_inv_pal
	db 188
	dw offset tunnel_soften
; quit & set
;	db 250
;	dw offset set_toggle
;	db 251
;	dw offset set1
;	db 252
;	dw offset set2
;	db 253
;	dw offset set1
;	db 254
;	dw offset set2
	db 254
	dw offset ende

	db -1

priorities:
; pal priors
	db 1, 10
	db 10, 10
	db 11, 2
	db 19, 2
	db 12, 1
;
;	db 27, 1
;	db 28, 1
	db 46, 3
	db 49, 5
	db 50, 6
	db 51, 4
; part - rotate
	db 61, 3
	db 62, 2
	db 63, 2
	db 64, 2
	db 65, 1
	db 66, 1
	db 67, 1
	db 68, 1
	db 69, 1
	db 70, 1
	db 71, 1
	db 74, 1
	db 75, 2
; part - blur rotator
	db 91, 2
	db 92, 2
	db 93, 2
	db 94, 2
; part - light
	db 101, 2
	db 114, 1
	db 115, 2
; part - plasma
	db 120, 2
	db 123, 2
	db 124, 2
	db 125, 3
; part - blur
	db 140, 3
	db 141, 3
	db 149, 1
	db 150, 1
	db 151, 2
	db 152, 1
; part - tunel
	db 170, 1
	db 171, 4
	db 174, 5
; part - tunnel
	db 181, 1
	db 188, 2

;	db 250, 10
;	db 251, 1
;	db 252, 1
;	db 253, 10
;	db 254, 10
	db 255, 10

	db -1

control_commands		db 12 DUP (0)
control_commands_count		dw 0
control_commands_new		db 12 DUP (0)
control_commands_count_new	dw 0
control_commands_store_new	db 0
control_commands_set1		db 1
control_commands_set2		db 0
prepared_screen			db 0
prepared_screen_segment		dw 0
prepared_screen_offset_write	dw 0
prepared_screen_offset_read	dw 0
;prepared_screen_len		dw 64000
prepared_screen_clear_offset	dw 0
prepared_screen_clear_len	dw 0
prepared_palette		db 0
prepared_palette_offset		dw 0
prepared_palette_from	db 0
prepared_palette_len	dw 0
;screen_master			dw 0
;screen_master_present		db 0

include	parts.inc

; ******************************************************************************
screen_manager	proc
	call	wait4retrace
	cmp	[prepared_palette], 0
	jz	@@j1
	mov	al, [prepared_palette_from]
	mov	bx, [prepared_palette_len]
	mov	si, [prepared_palette_offset]
	cmp	invert, 1
	jz	@@i
	call	setpalblock
	jmp	@@i2
@@i:
	call	setpalblock_i
@@i2:
	mov	[prepared_palette], 0
@@j1:
	cmp	[prepared_screen], 0
	jz	@@e
	mov	ax, [prepared_screen_segment]
	mov	fs, ax
	mov	ax, 0a000h
	mov	gs, ax
	mov	si, [prepared_screen_offset_read]
	mov	di, [prepared_screen_offset_write]

;	mov	cx, [prepared_screen_len]
;	shr	cx, 2
	mov	cx, 16384
@@l1:
	mov	eax, fs:[si]
	mov	gs:[di], eax
	add	si, 4
	add	di, 4
	dec	cx
	jnz	@@l1

	mov	kaos_status, 0

	mov	cx, [prepared_screen_clear_len]
	or	cx, cx
	jz	@nc
	shr	cx, 2
	xor	eax, eax
	mov	di, [prepared_screen_clear_offset]
@@l2:
	mov	gs:[di], eax
	add	di, 4
	dec	cx
	jnz	@@l2
@nc:

	mov	[prepared_screen], 0
@@e:
	ret
screen_manager	endp

; ******************************************************************************
basic_inits	proc
	mov	ax, seg module
	mov	bx, offset module
	shr	bx, 4
	add	ax, bx
	mov	[_module_seg], ax

;	mov	ax, seg _code
;	mov	bx, offset end_gus_play
	mov	ax, seg _stack
	mov	bx, offset endstack
	shr	bx, 4
	add	ax, bx
	inc	ax
	mov	[_temp_seg1], ax
	mov	prepared_screen_segment, ax

	add	ax, 01000h
	mov	[_temp_seg2], ax
	add	ax, 01000h
	mov	[_temp_seg3], ax
	add	ax, 01000h
	mov	[_temp_seg4], ax
	add	ax, 01000h
	mov	[_temp_seg5], ax
	add	ax, 01000h
	mov	[_temp_seg6], ax
	cmp	sound_card, 0
	jz	@@g
	add	ax, 01000h
	mov	[_module_seg2], ax
@@g:
	add	ax, 01000h
	cmp	ax, mem_top
	ja	@@nomem

	call	init_palettes

	mov	ax, 13h
	int	10h

	mov	al, 0
	mov	bx, 256
	mov	si, offset grey_pal
	cmp	invert, 1
	jz	@@i
	call	setpalblock
	jmp	@@i2
@@i:
	call	setpalblock_i
@@i2:

	ret

@@nomem:
	mov	bx, offset nomem_msg
	call	print_msg
	mov	ax, 4c01h
	int	21h

nomem_msg	db '≠ ÔÈÁ ÓÔÈöGH MÓMÈRù !', 13, 10, 0
basic_inits	endp

init_palettes	proc
; white pal
	mov	di, offset white_pal
	mov	cx, 768/4
	mov	eax, 3f3f3f3fh
	rep	stosd
; grey pal
	mov	di, offset grey_pal
	mov	bx, 03f00h
	mov	cx, 64
	call	palette_grad
	mov	eax, 03f3f3f3fh
	mov	cx, (768-64*3)/4
	rep	stosd
; grey pal intensity 3/4
	mov	di, offset grey2_pal
	mov	bx, 03000h
	mov	cx, 64
	call	palette_grad
; grey pal intensity 1/2
	mov	di, offset grey3_pal
	mov	bx, 02000h
	mov	cx, 64
	call	palette_grad
; grey pal intensity 1/4
	mov	di, offset grey4_pal
	mov	bx, 01000h
	mov	cx, 64
	call	palette_grad
; flash pal 1
	mov	di, offset flash1_pal
	mov	bx, 03f10h
	mov	cx, 64
	call	palette_grad
; flash pal 2
	mov	di, offset flash2_pal
	mov	bx, 03f20h
	mov	cx, 64
	call	palette_grad
; flash pal 3
	mov	di, offset flash3_pal
	mov	bx, 03f30h
	mov	cx, 64
	call	palette_grad
; grey inv pal
	mov	di, offset grey_inv_pal
	mov	bx, 0003fh
	mov	cx, 64
	call	palette_grad
	xor	eax, eax
	mov	cx, (768-64*3)/4
	rep	stosd
; plasma pal
	mov	di, offset plasma_pal
	mov	bx, 03f00h
	mov	cx, 64
	call	palette_grad
	mov	bx, 0003fh
	mov	cx, 64
	call	palette_grad
	mov	bx, 03f00h
	mov	cx, 64
	call	palette_grad
	mov	bx, 0003fh
	mov	cx, 64
	call	palette_grad
; plasma flash
	mov	di, offset plasma_fl_pal
	mov	bx, 01f30h
	mov	cx, 64
	call	palette_grad
	mov	bx, 0301fh
	mov	cx, 64
	call	palette_grad
	mov	bx, 01f30h
	mov	cx, 64
	call	palette_grad
	mov	bx, 0301fh
	mov	cx, 64
	call	palette_grad

	ret
init_palettes	endp

; di - dest , cx - count , bl - start , bh - end
palette_grad	proc
	mov	al, bh
	sub	al, bl
	js	@@j1
	inc	al
	xor	bh, bh
	jmp	@@j2
@@j1:
	dec	al
	mov	bh, 255
@@j2:
	cbw
	sal	ax, 8
	cwd
	idiv	cx
;	xor	bh, bh
@@l1:
	mov	[di], bl
	mov	[di+1], bl
	mov	[di+2], bl
	add	di, 3
	add	bh, al
	adc	bl, ah
	loop	@@l1
	ret
palette_grad	endp

; ******************************************************************************
check_commands	proc
	mov	control_commands_count_new, 0
	mov	control_commands_store_new, 1
	call	sort_priorities
	mov	di, offset control_commands
	mov	cx, control_commands_count
	or	cx, cx
	jz	@@ee

@@l1:
	mov	si, offset function
@@j1:
	lodsb
	cmp	al, -1
	jz @@e
	cmp	al, [di]
	lodsw
	jnz @@j1

	pusha
	or	ax, ax
	jz	@nf
	call	ax
@nf:
	popa

@@e:
	inc	di
	loop	@@l1
@@ee:
	cli
	mov	ax, [control_commands_count_new]
	mov	[control_commands_count], ax
	mov	eax, dword ptr [control_commands_new]
	mov	dword ptr [control_commands], eax
	mov	eax, dword ptr [control_commands_new+4]
	mov	dword ptr [control_commands+4], eax
	mov	eax, dword ptr [control_commands_new+8]
	mov	dword ptr [control_commands+8], eax
	sti
	mov	control_commands_store_new, 0
	ret
check_commands	endp

sort_priorities	proc
	mov	cx, control_commands_count
	cmp	cx, 1
	jbe	@@e
	dec	cx
	mov	dx, cx
@@l1:
	push	dx
	mov	si, offset control_commands
@@l2:
	mov	ah, [si]
	call	get_priority
	mov	bl, al
	mov	ah, [si+1]
	call	get_priority
	cmp	bl, al
	jna	@@j1
	mov	al, [si]
	xchg	[si+1], al
	mov	[si], al
@@j1:
	inc	si
	dec	dx
	jnz	@@l2
	pop	dx
	loop	@@l1
@@e:
	ret
sort_priorities	endp

get_priority	proc
	push	si
	mov	si, offset priorities
@@j2:
	lodsb
	cmp	al, -1
	jz	@@j1
	cmp	ah, al
	lodsb
	jnz	@@j2
	jmp	@@e
@@j1:
	mov	al, 255
@@e:
	pop	si
	ret
get_priority	endp

; ******************************************************************************
InitPlayer	proc
	call	Init
	call	InitDevice
;	mov	BasePort, 240h
	call	LoadMod
	push	cs
	pop	ds
	ret
InitPlayer	endp

; ******************************************************************************
gus_detect	proc
        mov si, 2ch
	mov fs, dsp_seg
        mov ax, fs:[si]
        mov es, ax
        mov di, 0
@@j1:
        mov al, es:[di]
        inc di
        or al, al
        jnz @@j1

        mov al, es:[di]
        or al, al
        jnz @@j11
        xor ax, ax
        stc
        jmp @@end               ;no GUS detected!!! (AX=0, CF=1);

@@j11:
;        mov ax, seg string
;        mov ds, ax
	push	cs
	pop	ds

        mov dx, di
        mov si, offset string
        mov cx, 8
@@j2:
        mov al, es:[di]
        cmp al, ds:[si]
        je @@j3
        mov di, dx
        jmp @@j1
@@j3:
        inc si
        inc di
        dec cx
        jnz @@j2

@@j4:
        mov al, es:[di]
        inc di
        cmp al, '0'
        jb @@j4
        cmp al, '9'
        ja @@j4

        xor ah, ah
        sub al, '0'
        shl ax, 4
        add al, es:[di]
        sub al, '0'
        shl ax, 4
        add al, es:[di+1]
        sub al, '0'
        clc                     ;GUS detected (AX=port, CF=0);
@@end:
	ret
string  db "ULTRASND"
gus_detect	endp

; ******************************************************************************
basic_precalcs	proc
	call	count_distances
	call	poison_wait
	call	count_angles
	call	poison_wait
	call	light_tabs
	call	poison_wait
	call	poison_wait_clear
	ret
basic_precalcs	endp

; ******************************************************************************
install_handlers	proc
	mov ax, 3508h
	int 21h
	mov word ptr [old_int8], bx
	mov word ptr [old_int8 + 2], es
	mov ax, 3509h
	int 21h
	mov word ptr [old_int9], bx
	mov word ptr [old_int9 + 2], es

	cmp	sound_card, 1
	jz	@@sb
	mov ax, 2508h
	mov dx, offset resident8
	int 21h
	jmp	@@sb2

@@sb:
	mov	al,36h
	out	43h,al
	mov	al,169
	out	40h,al
	mov	al,4
	out	40h,al

	mov ax, 2508h
	mov dx, offset resident8_2
	int 21h
@@sb2:
	mov ax, 2509h
	mov dx, offset resident9
	int 21h
	ret

install_handlers	endp

uninstall_handlers	proc
	cmp	sound_card, 0
	jz	@@g

	mov	al,36h
	out	43h,al
	xor al, al
	out	40h,al
	out	40h,al
@@g:
	mov dx, word ptr [old_int8]
	mov ds, word ptr [old_int8 + 2]
	mov ax, 2508h
	int 21h
	push cs
	pop ds
	mov dx, word ptr [old_int9]
	mov ds, word ptr [old_int9 + 2]
	mov ax, 2509h
	int 21h
	push cs
	pop ds
	ret

uninstall_handlers	endp

time_counter	dd 0
time_counter2	dd 0
counter8	db 0

old_int8	dd 0
old_int9	dd 0

resident8	proc
	inc cs:[time_counter]
	inc cs:[time_counter2]
	jmp dword ptr cs:[old_int8]
resident8	endp

update_now	db 0
tunnel_counter	db 0
resident8_2	proc
	cmp	cs:[sb_tunnel], 0
	jz	@@nt2
	cmp	[tunnel_counter], 0
	jnz	@@nt
	inc	cs:[tunnel_pos]
@@nt:
	inc	cs:[tunnel_counter]
	cmp	cs:[tunnel_counter], 20
	jnz	@@nt2
	mov	cs:[tunnel_counter], 0
@@nt2:

	inc cs:[time_counter]
	inc cs:[time_counter2]
	inc cs:[counter8]
	cmp cs:[counter8], 55
	jz @@j1
	push ax
	mov al, 20h
	out 20h, al
	pop ax

	cmp	update_now, 1
	jz	@@j2
	mov	update_now, 1
	pusha
	push	es ds
	call	update
	pop	ds es
	popa
	mov	update_now, 0
@@j2:
	iret
@@j1:
	mov cs:[counter8], 0
	jmp dword ptr cs:[old_int8]
resident8_2	endp

resident9	proc
	push ax bx
	in al, 60h
	mov bl, al

	in	al,61h
	or	al,80h
	out	61h,al
	and	al,7Fh
	out	61h,al
	mov	al,20h
	out	20h,al

	cmp bl, 1
	jnz @@j1
	mov	cs:[quit], 1
@@j1:
	pop bx ax
	iret
resident9	endp

quit		db 0
overflow	db 0
mem_top		dw 0
sound_card	db 0 ; 0 - gus = 1 - sb
sb_tunnel	db 0
dsp_seg		dw 0
invert		db 0
kaos		db 0
kaos_status	db 0

; ******************************************************************************

INCLUDE	xmode.inc
_temp_seg1	dw 0
_temp_seg2	dw 0
_temp_seg3	dw 0
_temp_seg4	dw 0
_temp_seg5	dw 0
_temp_seg6	dw 0
_module_seg	dw 0
_module_seg2	dw 0

black_pal	db 768 DUP (0)
white_pal	db 768 DUP (0)
grey_pal	db 256*3 DUP (0)
grey2_pal	db 64*3 DUP (0)
grey3_pal	db 64*3 DUP (0)
grey4_pal	db 64*3 DUP (0)
flash1_pal	db 64*3 DUP (0)
flash2_pal	db 64*3 DUP (0)
flash3_pal	db 64*3 DUP (0)
grey_inv_pal	db 256*3 DUP (0)
plasma_pal	db 768 DUP (0)
plasma_fl_pal	db 768 DUP (0)

INCLUDE	sbtools.asm

	code ends


; ******************************************************************************

patterns_seg	segment
	db	10*1024 DUP (0)
patterns_seg	ends

INCLUDE	gusplay.inc

INCLUDE	modsub.asm

_stack	segment	stack 'stack'
	db 1024 DUP (0)
endstack	label
_stack	ends

	end start