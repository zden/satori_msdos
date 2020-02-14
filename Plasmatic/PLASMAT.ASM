; **************************************
; *                                    *
; *             PLASMATIC              *
; *                                    *
; **************************************

code segment
	assume cs:code, ds:code
	org 100h
	.386

release	EQU

INCLUDE	xmode.mac

VSPS1	EQU 1;2
VSPS2	EQU -3
HSPS1	EQU 2;1
HSPS2	EQU -1;-2

VS1	EQU 2
VS2	EQU -1
HS1	EQU -2
HS2	EQU 1

plasmapalette	EQU 62000
plasmasin	EQU 65000
blackpal	EQU 60000
whitepal	EQU 61000

start:
	call initplasmatic
	call openxmode
	call light
	call satori

	call doubleplasma
	call fadeplasma
	call infopage
	call credits
IFDEF	release
	call creator
ENDIF

ende:
	call deinitplasmatic
	mov ax, 3
	int 10h
IFDEF	release
	mov ah, 9
	mov dx, offset endinfo
	int 21h
ENDIF
	ret
IFDEF	release
endinfo	db 'PLASMATiC by Zden of Sunrise Software', 13, 10, '$'
ENDIF
; **************************************
initplasmatic:
	call hook8
	call meminit
	call makesin
	in al, 21h
	mov port21, al
	mov al, 11111110b
	out 21h, al
	ret

deinitplasmatic:
	call unhook8
	push cs
	pop ds
	mov al, port21
	out 21h, al
	ret

port21	db 0

meminit:
	push cs
	pop es
	mov di, offset endofcom
	mov cx, 65000
	sub cx, offset endofcom
	shr cx, 1
	xor ax, ax
	rep stosw
	mov di, whitepal
	mov cx, 768/2
	mov ax, 3f3fh
	rep stosw
	ret

setblackpal:
	xor al, al
	mov bx, 256
	mov si, blackpal
	call setpalblock
	ret

; **************************************
; ======================================

IFDEF	release

creator:
	mov al, 1
	mov bx, 3
	mov si, offset creatorpal
	call setpalblock
	dec bx
	dec bx
	mov al, 130
	mov si, offset creatorpal+3
	call setpalblock

	mov ax, cs
	add ax, 1000h
	mov es, ax
	xor di, di
	mov al, 1
crjj1:
	mov cx, 34*160
	rep stosb
	inc al
	cmp al, 4
	jnz crjj1

	push es
	pop ds
	push cs
	pop es
	mov cx, 16000/2
	mov di, satoriframe
	xor si, si
	rep movsw
	push cs
	pop ds

	call showaddframe
	mov si, offset text3
	mov di, 39*160+4
	call showtext
	xor di, di
	call showsatoriframe
	mov si, offset creatorpal+3
	mov di, offset creatorpal2
	mov al, 130
	mov bx, 1
	call _fadepal1to2
	mov [timer], 0
crj1:
	cmp timer, 180/2 + 180/4
	jb crj1
	call clearsatoriframe
	call clearaddframe
	call showaddframe
	ret
ENDIF

; **************************************
; ======================================

credits:
	call meminit

;	mov word ptr [countsatoriframe_decode1], 9090h
;	mov word ptr [initsatori_decode1], 9090h
;	mov word ptr [satoriMOVEdecode], 09090h ; !!!

;	_setyzoom 3
;	_setsplitscreen 400
	xor ax, ax
	call setstartaddress

	mov si, offset credits_initsub1
	call initsatori
	mov si, offset credits_phasedata
	mov di, offset credits_phasesub
	call initphaser

	call makesatoripal
	call setblackpal

	call clearsatoriframe
	call clearaddframe
	xor di, di
	call showsatoriframe
	mov si, plasmapalette+1000
	mov al, 128
	mov bx, 128
	call setpalblock

	mov si, offset text2
	mov di, 32*160 + 16
	call showtext
	call showaddframe

credits_again:

	mov ax, 16000
	call makesatoriframe
	call nextphase
	cmp phasenum, 0
	jz credits_end

	xor ax, ax
	call makesatoriframe
	call nextphase
	cmp phasenum, 0
	jz credits_end
	jmp credits_again

credits_end:

	call clearaddframe
	call clearsatoriframe
	xor ax, ax
	call setstartaddress
	call showaddframe

	ret

showaddframe:
	mov ax, 0a000h
	mov es, ax
	mov dx, 3c4h
	mov ax, cs
	add ax, 01000h
	mov ds, ax
	mov cx, 65535
safl1:
	call random
	shr ax, 1
	shr ax, 1
	mov si, ax
	shr ax, 1
	mov di, ax
	mov ax, 0302h
	jnc safj1
	shl ah, 1
	shl ah, 1
safj1:	out dx, ax
	movsb
	loop safl1
	push cs
	pop ds
	ret

random:
	push cx

	mov cx, 10h
	mov ax, cs:[randata]
ranl1:
	shl ax, 1
	jnb ranj1
	xor al, 2dh
ranj1:
	loop ranl1
	mov cs:[randata], ax

	pop cx
	ret
randata	dw 0d066h

credits_phasedata:
	dw 0, offset credits_phasesub1
	dw 180, offset credits_phasesub2
	dw 180+180/2, offset credits_phasesub3
	dw 180+180+180/2, offset credits_phasesub4
	dw 65000

credits_phasesub1:
	mov ah, al
	mov al, 1
	mov bx, 6
	mov si, blackpal
	mov di, offset credits_pal1
	call fadepal1to2
	ret


credits_phasesub2:
	mov ah, al
	mov al, 1
	mov bx, 6
	mov di, blackpal
	mov si, offset credits_pal1
	call fadepal1to2
	ret

credits_phasesub3:
	or al, al
	jnz c_ps3j1
	mov si, offset credits_initsub2
	call initsatori
c_ps3j1:
	mov ah, al
	mov al, 1
	mov bx, 6
	mov si, blackpal
	mov di, offset credits_pal1
	call fadepal1to2
	ret

credits_phasesub4:
	mov ah, al
	mov al, 1
	mov bx, 6
	mov di, blackpal
	mov si, offset credits_pal1
	call fadepal1to2
	ret

credits_phasesub:
	ret

; **************************************
; ======================================

infopage:
	mov word ptr [countsatoriframe_decode1], 9090h
	mov word ptr [initsatori_decode1], 9090h
;	mov word ptr [satoriMOVEdecode], 09090h ; !!!

;	_setyzoom 3
;	_setsplitscreen 400
	call makesatoripal

	mov si, offset infopage_initsub
	call initsatori
	mov si, offset infopage_phasedata
	mov di, offset infopage_phasesub
	call initphaser

	mov si, offset text1
	mov di, 16*160
	call showtext
infopage_again:

	call shiftinfopagepal
	mov ax, 16000
	call makesatoriframe
	call nextphase
	cmp phasenum, 11000000b
	jz infopage_end

	call shiftinfopagepal
	xor ax, ax
	call makesatoriframe
	call nextphase
	cmp phasenum, 11000000b
	jz infopage_end
	jmp infopage_again

infopage_end:
	ret

infopage_phasedata:
	dw 0, offset infopage_phasesub1
	dw 180, offset infopage_phasesub2
	dw 180*2, offset infopage_phasesub3
	dw 65000

infopage_phasesubs_sub1:
	mov ah, al
	mov al, 1
	mov bx, 127
	mov si, blackpal
	ret

infopage_phasesub1:
	mov ah, al
	mov al, 1
	mov bx, 127
	mov si, blackpal
	mov di, plasmapalette+3
	call fadepal1to2
	mov al, 129
	call fadepal1to2
	ret

infopage_phasesub2:
	mov ah, al
	mov bx, 127
	mov al, 129
	mov si, plasmapalette+3
	mov di, plasmapalette+1000
	call fadepal1to2
	mov al, 1
	call setpalblock
	ret

infopage_phasesub3:
	mov ah, al
	mov al, 1
	mov bx, 127
	mov di, blackpal
	mov si, plasmapalette+3
	call fadepal1to2
	mov al, 129
	mov si, plasmapalette+1000
	call fadepal1to2
	ret

infopage_phasesub:
	mov al, 1
	mov bx, 127
	mov si, plasmapalette+3
	call setpalblock
	mov al, 129
	cmp phasenum, 10000000b
	jz ip_psj1
	call setpalblock
ip_psj1:
	ret

shiftinfopagepal:
	mov si, plasmapalette
	mov cx, 3
	call shiftpalette
	ret

showtext:
	push di
	call clearaddframe
	pop di
	push cs
	pop ax
	add ax, 01000h
	mov es, ax
;	mov di, 0 ;satoriaddframe+1
	mov dx, di

st_a1:
	lodsb
	or al, al
	jz st_end

	cmp al, 32
	jnz st_jj1
	add di, 8
	jmp st_a1
st_jj1:	cmp al, 13
	jnz st_jj2
	add dx, 160*8
	mov di, dx
	jmp st_a1
st_jj2:

	sub al, 65
	mov bl, 7
	mul bl
	mov bx, offset font
	add bx, ax

	push di

	mov cx, 7
st_l1:
	mov ah, [bx]
	push di
	mov ch, 128
st_l2:
	test ah, ch
	jnz st_j1
	xor al, al
	jmp st_j2
st_j1:	mov al, 128
st_j2:	stosb
	shr ch, 1
	jnz st_l2
	pop di
	add di, 160
	inc bx
	loop st_l1

	pop di

	add di, 8
	jmp st_a1

st_end:
	ret

; **************************************
; ======================================

light:
	mov word ptr [countsatoriframe_decode1], 09090h
	mov word ptr [initsatori_decode1], 9090h

	_setyzoom 3
	_setsplitscreen 400

	call setblackpal

	mov si, offset light_pal1
	mov al, 2
	mov bx, 1
	call setpalblock
	mov si, offset light_pal2
	mov al, 129
	inc bx
	call setpalblock

	call clearsatoriframe
	call showintroname
	xor di, di
	call showsatoriframe

	mov si, offset light_initsub
	call initsatori

	mov cx, -30
light_again:
	push cx
	mov ax, 16000
	call makelightframe
	pop cx
;	inc cx
	inc cx
	push cx
	xor ax, ax
	call makelightframe
	pop cx
;	inc cx
	inc cx

	cmp cx, 160+30+4
	jnz light_again

	ret

makelightframe:
	push ax
	push cx
	call clearsatoriframe
	pop cx

	mov word ptr [countsf_data1], cx
	mov si, plasmasin
	add cx, 30
	add si, cx
	lodsb
	add al, 128
	shr al, 2
	add al, 20
	mov dh, al
	call countSF

	pop di
	push di
	call showsatoriframe
	pop ax
	call setstartaddress
	call wait4retrace
	ret

; **************************************
; ======================================

satori:
	mov word ptr [countsatoriframe_decode1], 0d8f6h
	mov word ptr [initsatori_decode1], 04ebh

;	_setyzoom 3
;	_setsplitscreen 400
	call makesatoripal

	call setblackpal

	call clearsatoriframe
	call showintroname
	xor di, di
	call showsatoriframe

	mov si, plasmapalette+1000
	mov al, 129
	mov bx, 127
	call setpalblock

	call wait4retrace
	call wait4retrace

	mov si, plasmapalette+1000
	mov di, blackpal
	xor al, al
	mov bx, 2
	call _fadepal1to2

	mov si, offset satori_initsub
	call initsatori
	mov si, offset satoriphases
	mov di, offset satori_phasesub
	call initphaser
satori_again:
	call shiftsatoripal
	mov ax, 16000
	call makesatoriframe
	call nextphase
	cmp phasenum, 0
	jz satori_end

	call shiftsatoripal
	xor ax, ax
	call makesatoriframe
	call nextphase
	cmp phasenum, 0
	jz satori_end
	jmp satori_again
satori_end:
	ret

satoriphases:
	dw 0, offset satori_palsub1
	dw 180+90, offset satori_movesub1
	dw 90+180*2, offset satori_palsub2
	dw 180*4, offset satori_palsub3
; *****
initphaser:
	mov timer, 0
	mov phasenum, 0
	mov phasedata, si
	mov phasesub, di
	ret

nextphase:
	mov si, phasedata
	mov cl, phasenum
	mov al, cl
	and al, 128+64
	shr al, 4
	xor ah, ah
	add si, ax
	mov al, cl

	and al, 63
	jnz npj1
	mov ax, [si]
	cmp ax, [timer]
	jnb npend
	xor al, al
npj1:
	inc si
	inc si
	call [si]
	inc phasenum
	ret
npend:	mov si, phasesub
	call si
	ret
phasenum db 0
phasedata dw 0
phasesub dw 0

; *****
satori_palsub1:
	mov si, blackpal
	mov di, plasmapalette+3
	mov ah, al
	mov al, 1
	mov bx, 127
	call fadepal1to2
	mov si, plasmapalette+1000
	mov di, plasmapalette+500+3
	mov al, 129
	call fadepal1to2
	ret

satori_movesub1:
	call satori_phasesub
	mov word ptr [satoriMOVEdecode], 09090h
	ret

satori_palsub2:
	push ax
	mov si, plasmapalette+500+3
	mov di, plasmapalette+3
	mov ah, al
	mov al, 129
	mov bx, 127
	call fadepal1to2
	mov si, plasmapalette+3
	mov al, 1
	mov bx, 127
	call setpalblock
	pop ax
	cmp al, 63
	jnz s_ps2j1
	push cs
	pop ax
	add ax, 01000h
	mov es, ax
	xor ax, ax
	xor di, di
	mov cx, 8000
	xor ax, ax
	rep stosw
s_ps2j1:
	ret

satori_palsub3:
	mov si, plasmapalette+3
	mov di, blackpal
	mov ah, al
	mov al, 1
	mov bx, 127
	call fadepal1to2
	ret

satori_phasesub:
	mov si, plasmapalette+3
	mov al, 1
	mov bx, 127
	call setpalblock
	mov si, plasmapalette + 500 + 3
	mov al, 129
	call setpalblock
	ret

shiftsatoripal:
	mov si, plasmapalette
	mov cx, 3*6
	call shiftpalette
	mov si, plasmapalette+500
	mov cx, 3*6
	call shiftpalette
	ret

; *****

makesatoriframe:
	push ax
	call clearsatoriframe
	call countsatoriframe
	pop di
	push di
	call showsatoriframe
	pop ax
	call setstartaddress
satoriMOVEdecode:
	jmp masfj1
	call movesatori
masfj1:
	call wait4retrace
	ret

makesatoripal:
	call makeDPpal
	push cs
	pop es
	mov si, plasmapalette
	mov di, plasmapalette+500
	mov cx, 384/2
	push di
	rep movsw
	pop di
	mov cx, 128
	mov al, 30
sml1:
	inc di
	inc di
	stosb
	loop sml1
	mov di, plasmapalette+1000
	mov cx, 130
sml2:
	mov al, 50
	stosb
	stosb
	mov al, 63
	stosb
	loop sml2
	ret
; * * * * * * * * * * * * * * * * * * *

movesatori:
	mov si, offset satoridata
	xor bh, bh

	xor ch, ch
_msl2:	xor cl, cl
_msl1:	cmp cl, ch
	jz _msj1

	call movesatoriSUB
	inc si
	inc si
	call movesatoriSUB
	dec si
	dec si

_msj1:
	inc cl
	cmp cl, 3
	jnz _msl1
	inc ch
	cmp ch, 3
	jnz _msl2

	mov cx, 3
	mov bx, 64
_msll1:
	mov ax, [si + 4]
	cwd
	idiv bx
	add [si], al

	mov ax, [si + 6]
	cwd
	idiv bx
	add [si+2], al

	add si, 8
	loop _msll1
	ret

movesatoriSUB:
	mov bl, cl
	shl bl, 3
	mov ax, [si+bx]
	mov bl, ch
	shl bl, 3
	sub ax, [si+bx]
	add bx, 4
	add [si+bx], ax
	ret
; **************************************
showintroname:
	call clearaddframe
	push cs
	pop ax
	add ax, 01000h
	mov es, ax
	mov si, offset plasmatic
	mov di, (37*160) + 1
	mov cx, 500
sinl1:
	lodsb
	mov ah, al
	mov bl, 128
sinl2:
	test ah, bl
	jz sinB
	mov al, 128
	jmp sinW
sinB:	xor al, al
sinW:	stosb
	shr bl, 1
	jnz sinl2
	loop sinl1
	ret

clearaddframe:
	push cs
	pop ax
	add ax, 01000h
	mov es, ax
	xor di, di
	xor ax, ax
	mov cx, 17000/2
	rep stosw
	ret

; **************************************
countsatoriframe:
	push cs
	pop es
	mov si, offset satoridata
	mov cx, 3
csfloop1:
	lodsw
;	inc si
;	mov dl, al
	mov word ptr [countsf_data1], ax
	lodsb
	inc si
	mov dh, al
	call countSF
	add si, 4
	loop csfloop1
	ret
; *****
countSF:
	pusha
	mov di, satoriframe
	xor ch, ch
cSFl2:
	xor cl, cl
cSFl1:
	db 0bbh
countsf_data1:
	dw 0

	xor ah, ah
	mov al, cl

	sub bx, ax
	jns cSFj1
	neg bx
	mov bh, 128
cSFj1:
	push bx
	xor ah, ah
	xor bh, bh
	mov al, ch
	mov bl, dh
	sub ax, bx
	pop bx
	jns cSFj2
	neg ax
	xor bh, 128
cSFj2:
	mov ah, al
	mov al, bl
	mov si, offset satoritable
	add si, ax

	lodsb
	test bh, 128
	jz cSFj666
countsatoriframe_decode1:
	neg al
cSFj666:
	add [di], al
	inc di

	inc cl
	cmp cl, 160
	jnz cSFl1
	inc ch
	cmp ch, 100
	jnz cSFl2
	popa
	ret

; **************************************
showsatoriframe:
	push cs
	pop ax
	add ax, 01000h
	mov ss, ax

	mov ax, 0a000h
	mov es, ax

	mov bx, satoriframe
	inc bx
	neg bx

	mov si, satoriframe
	_setbitplain 3
	call ssframesub
	_setbitplain 12
	inc si
	call ssframesub

	push cs
	pop ss
	ret

ssframesub:
	pusha
	mov cx, 80*100
ssfl1:
	lodsb
	and al, 127
	jnz ssfj1
	inc al
ssfj1:
	add al, ss:[si+bx]
	stosb
	inc si
	loop ssfl1
	popa
	ret

; **************************************
clearsatoriframe:
	push cs
	pop es
	mov di, satoriframe
	xor ax, ax
	mov cx, 8000
	rep stosw
	ret
; **************************************
initsatori:
	push cs
	pop es
	mov di, offset satoritable
	xor bh, bh
	xor dx, dx
	mov ch, 100
is_j2:
	mov cl, 220
is_j1:
	mov al, 220
	sub al, cl
	mul al
	mov bx, ax
	mov al, 100
	sub al, ch
	mul al
	add bx, ax

initsatori_decode1:
	jmp is_decodej1
	jmp is_decodej2
is_decodej1:
	nop
	nop

	mov al, 100
	sub al, ch
	mul al
	xor dx, dx
	mov dl, ah
	mov ah, al
	xor al, al

	cmp bx, 0
	jz is_j666
	div bx
is_j666:
is_decodej2:
	call si
	stosb

	dec cl
	jnz is_j1
	add di, 256 - 220; 160
	dec ch
	jnz is_j2

	ret

satori_initsub:
	shr al, 1
	ret
infopage_initsub:
	mov al, bh
	shr al, 1
	ret
credits_initsub1:
	mov al, bh
	and al, 2
	ret
credits_initsub2:
	mov ax, bx
	call sqrt
	shr al, 2
	and al, 2
	ret
light_initsub:
	xor al, al
	cmp bx, (25*25)-1
	ja l_isj1
	inc al
	inc al
l_isj1:
	ret

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
; **************************************
; ======================================

fadeplasma:
	call meminit

	mov word ptr [FPinc], 9090h
	_setyzoom 3
	call makeFPpal
	mov al, 128
	mov bx, 128
	mov si, whitepal
	call setpalblock
	call makeshiftframe
	_setsplitscreen 400

	mov si, offset fplasma_phasedata
	mov di, offset fplasma_phasesub
	call initphaser
FPagain:

	mov ax, 16000
	call make_fadeplasmaframe

	xor ax, ax
	call make_fadeplasmaframe

	call nextphase

	cmp phasenum, 10000000b
	jnz FPagain

	mov si, whitepal
	mov di, blackpal
	xor al, al
	mov bx, 256
	call _fadepal1to2

	ret

fplasma_phasedata:
	dw 0, offset fplasma_phasesub1
	dw 180*2, offset fplasma_phasesub2
	dw 65000

fplasma_phasesub1:
	mov ah, al
	mov al, 128
	mov bx, 128
	mov si, whitepal
	mov di, plasmapalette
	call fadepal1to2
	ret

fplasma_phasesub2:
	mov ah, al
	mov al, 128
	mov bx, 128
	mov si, plasmapalette
	mov di, whitepal
	call fadepal1to2
	ret

fplasma_phasesub:
	ret

make_fadeplasmaframe:
	push ax
	call movesin
	call countframe
	call convertframe
	pop di
	push di
	call FPshowframe
	pop ax
	call setstartaddress
	call wait4retrace
	ret

; **************************************
makeshiftframe:
	push cs
	pop es
	mov si, offset rectangles
	mov di, offset shiftframe
	mov cl, 160
	mov ch, 100
	mov dx, 17
msfj1:
	lodsb
	call makerectangle
	sub cx, 202h
	add di, 161
	dec dx
	jnz msfj1
	ret
rectangles	db 63,63,57,57,51,51,45,39,33,27,21,16,10,5,3,2,1
makerectangle:
	pusha
	push di cx
	xor ch, ch
	rep stosb
	pop cx di
mrj1:
	mov [di], al
	mov bl, cl
	xor bh, bh
	dec bx
	add di, bx
	mov [di], al
	sub di, bx
	add di, 160
	dec ch
	jnz mrj1
	sub di, 160
	rep stosb
	popa
	ret
; **************************************
makeFPpal:
	mov ah, 20h
	mov bx, 0200h
	xor cx, cx
	xor dx, dx
	mov di, plasmapalette
	call _makepal
	mov bx, 003eh
	mov cx, 0200h
	add di, 32*3
	call _makepal
	mov ah, 40h
	mov bx, 003eh
	mov cx, 003eh
	mov dx, 003eh
	add di, 32*3
	call _makepal
	ret
; **************************************
FPshowframe:
	mov ax, 0a000h
	mov es, ax

	mov si, offset plasmaframe
	_setbitplain 3
	call fpsfsub
	_setbitplain 12
	inc si
	call fpsfsub
	ret

fpsfsub:
	pusha
	mov cx, 80*100
sfl1:
	lodsb
	stosb
	inc si
	loop sfl1
	popa
	ret

; **************************************
convertframe:
	mov si, offset shiftframe
	mov di, offset plasmaframe
	mov cx, 16000
cofl1:
	lodsb
	add [di], al
	inc di
	loop cofl1
	ret
; **************************************
doubleplasma:
	call openxmode
	_setyzoom 0
	call setblackpal
	call showviewframe
	call makeDPpal
	_setsplitscreen 400
	_setpage 0

	mov si, offset dplasma_phasedata
	mov di, offset dplasma_phasesub
	call initphaser
DPagain:

	mov ax, 32000
	call make_doubleplasmaframe

	xor ax, ax
	call make_doubleplasmaframe

	mov si, plasmapalette
	mov cx, 3
	call shiftpalette

	call nextphase

	cmp phasenum, 10000000b
	jnz DPagain

	mov si, plasmapalette-32
	mov di, whitepal
	xor al, al
	mov bx, 4
	call _fadepal1to2

	ret

dplasma_phasedata:
	dw 0, offset dplasma_phasesub1
	dw 180*2, offset dplasma_phasesub2
	dw 65000

dplasma_phasesub1:
	mov ah, al
	mov al, 128
	mov bx, 128
	mov si, blackpal
	mov di, plasmapalette
	call fadepal1to2
	ret

dplasma_phasesub2:
	mov ah, al
	mov al, 128
	mov bx, 128
	mov si, plasmapalette
	mov di, whitepal
	call fadepal1to2
	ret

dplasma_phasesub:
	mov al, 128
	mov bx, 128
	mov si, plasmapalette
	call setpalblock
	ret

; *****
make_doubleplasmaframe:
	push ax
	call countframe
	pop di
	push di
	add di, 8000+20
	call showframe
	call wait4retrace
	pop ax
	call setstartaddress
	ret

; **************************************
; si = source   cx = num of bytes
shiftpalette:
	push cs
	pop es
	mov di, si
	add di, 384
	push cx si
	rep movsb
	pop si cx
	mov di, si
	add si, cx
	mov cx, 384/2
	rep movsw
	ret
; **************************************
makeDPpal:
	mov ah, 20h
	mov bx, 003eh
	mov cx, 0200h
	mov dx, 0fe3eh
	mov di, plasmapalette
	call _makepal
	mov bx, 0fe3eh
	mov cx, 003eh
	mov dx, 0200h
	add di, 32*3
	call _makepal
	xor bx, bx
	mov cx, 0fe3eh
	mov dx, 003eh
	add di, 32*3
	call _makepal
	mov bx, 0200h
	xor cx, cx
	mov dx, 003eh
	add di, 32*3
	call _makepal
	ret
; **************************************
showviewframe:
	mov cx, 33			; frame
	mov di, 45*2*80 + 17+6
	mov dx, 0501h
udfl1:
	mov si, offset u_fr
	call packed_bitmap
	mov si, offset d_fr
	add di, 105*2*80
	call packed_bitmap
	sub di, (105*2*80) - 1
	loop udfl1

	mov cx, 10
	mov di, 61*2*80 + 18
	mov dx, 0802h
lrfl1:
	mov si, offset l_fr
	call packed_bitmap
	mov si, offset r_fr
	add di, 42
	call packed_bitmap
	add di, (160*8) - 42
	loop lrfl1

	mov si, offset lu_cor
	mov di, 40*2*80 + 17
	mov dx, 1507h
	call packed_bitmap
	mov si, offset ru_cor
	mov di, 40*2*80 + 17+39
	call packed_bitmap
	mov si, offset ld_cor
	mov di, 139*2*80 + 17
	call packed_bitmap
	mov si, offset rd_cor
	mov di, 139*2*80 + 17+39
	call packed_bitmap

	push es				; copy page 0 to page 2
	pop ds

	mov cx, 4
cpl1:
	mov ax, 1002h
	shr ah, cl
	mov dx, 3c4h
	out dx, ax
	mov ax, 0404h
	sub ah, cl
	mov dx, 3ceh
	out dx, ax
	push cx
	xor si, si
	mov di, 32000
	mov cx, 16000
	rep movsw
	pop cx
	loop cpl1
	push cs
	pop ds

	mov ax, 0301h
	mov bx, 0a10h
	mov cx, 0a10h
	mov dx, 0a23h
	mov di, plasmapalette-32+3
	call _makepal
	mov al, 1
	mov bx, 3
	mov si, blackpal
	mov di, plasmapalette-32+3
	call _fadepal1to2
	ret

; **************************************
showframe:
	mov ax, 0a000h
	mov es, ax

	mov si, offset plasmaframe
	_setbitplain 1
	push di
	call showframe1
	pop di

	mov si, offset plasmaframe + 2
	_setbitplain 4
	push di
	call showframe1
	pop di

	mov si, offset plasmaframe + 1
	_setbitplain 2
	push di
	add di, 80
	call showframe1
	pop di

	mov si, offset plasmaframe + 3
	_setbitplain 8
	push di
	add di, 80
	call showframe1
	pop di
; ***
	mov si, offset plasmaframe+15999-1
	_setbitplain 2
	push di
	call showframe2
	pop di

	mov si, offset plasmaframe+15999-3
	_setbitplain 8
	push di
	call showframe2
	pop di

	mov si, offset plasmaframe+15999
	_setbitplain 1
	push di
	add di, 80
	call showframe2
	pop di

	mov si, offset plasmaframe+15999-2
	_setbitplain 4
	push di
	add di, 80
	call showframe2
	pop di

	ret

showframe1:
	mov bl, 100
sf1j1:
	mov cx, 40
sf1l1:
	lodsb
	stosb
	add si, 3
	loop sf1l1
	add di, 40 + 80
	dec bl
	jnz sf1j1
	ret

showframe2:
	mov bl, 100
sf2j1:
	mov cx, 40
sf2l1:
	mov al, [si]
	stosb
	sub si, 4
	loop sf2l1
	add di, 40 + 80
	dec bl
	jnz sf2j1

	ret

; **************************************
countframe:
	mov bx, offset plasmasin
	push cs
	pop es
	mov di, offset plasmaframe

	mov cl, VSP1
	mov ch, VSP2
	mov ypos, 100
cfjmp2:
	mov dl, HSP1
	mov dh, HSP2
	mov xpos, 160
cfjmp1:
	xor ah, ah

	mov al, cl
	xlat
	add ah, al

	mov al, ch
	xlat
	add ah, al

	mov al, dl
	xlat
	add ah, al
	add dl, HS1

	mov al, dh
	xlat
	add ah, al
	sub dh, HS2

	mov al, ah
	or al, 128
FPinc:
	jmp noFPinc
	cmp al, 192
	jb __no192
	xor al, 127
__no192:
noFPinc:
	stosb
	dec xpos
	jnz cfjmp1

	add cl, VS1
	add ch, VS2

	dec ypos
	jnz cfjmp2

	add VSP1, VSPS1
	sub VSP2, VSPS2
	add HSP1, HSPS1
	sub HSP2, HSPS2
	ret

; ****************************

_makepal:
; inputs   al = start color ah = num bx = startpos&speed, cx = ..., dx = ...
;          di = dest
	pusha
	push cs
	pop es
mpj1:
	mov al, bl
	add bl, bh
	stosb
	mov al, cl
	add cl, ch
	stosb
	mov al, dl
	add dl, dh
	stosb
	dec ah
	jnz mpj1
	popa
	ret

makepal:
	mov di, plasmapalette
	call _makepal
	mov bl, ah
	xor bh, bh
	mov si, di
	call setpalblock
	ret
; ****************************
makesin:
	mov ebx, 01000000h
	xor ecx, ecx
	mov di, plasmasin
	mov bp, 400h
msj1:
	imul eax, ebx, 0005h
	sar eax, 11h
	sub ecx, eax
	add ebx, ecx
	imul eax, ebx, 000bh
	sar eax, 16h
	test bp, 3
	jnz msj2
	stosb
msj2:
	dec bp
	jne msj1
	ret
; ****************************
movesin:
	push cs
	pop es
	mov si, offset plasmasin
	mov di, offset plasmasin
	mov cx, 256
_cfl1:
	lodsb
	add al, 3
	stosb
	loop _cfl1
	ret
; ****************************
packed_bitmap:
; inputs: si = source   di = dect   dl = wid dh = hei
	pusha

	mov ax, 0a000h
	mov es, ax
ppj3:
	push dx
	push di
ppj2:
	mov cx, 0c006h
	mov ax, 0102h
ppj1:
	push dx
	mov dx, 3c4h
	out dx, ax
	pop dx
	mov bl, [si]
	and bl, ch
	shr bl, cl
	mov byte ptr es:[di], bl
	mov byte ptr es:[di+80], bl
	shl ah, 1
	shr ch, 2
	sub cl, 2
	cmp ch, 0
	jnz ppj1
	inc si
	inc di
	dec dl
	jnz ppj2
	pop di
	pop dx
	add di, 160
	dec dh
	jnz ppj3

	popa
	ret
; ****************************
timer	dw 0
old_int dd 0
resident8:
	inc cs:[timer]
	jmp cs:[old_int]

hook8:
	mov ax, 3508h
	int 21h
	mov word ptr [old_int], bx
	mov word ptr [old_int + 2], es
	mov ax, 2508h
	mov dx, offset resident8
	int 21h
	ret

unhook8:
	mov dx, word ptr [old_int]
	mov ax, word ptr [old_int + 2]
	mov ds, ax
	mov ax, 2508h
	int 21h
	ret
; ****************************

INCLUDE xmode.inc

lu_cor	db 15,255,255,192,0,0,0,14,170,170,192,0,0,0,14,85,86,192,21,85,64,14,85,86,192,26,170,64,14,85,86,192,43,254,128,14,87,255,255,255,255,255,14,87,255,255,255,255,255,14
	db 87,255,255,255,255,255,14,171,249,85,85,85,85,15,255,246,85,85,85,85,0,3,245,0,0,0,0,0,3,245,0,0,0,0,0,3,245,0,0,0,0,0,3,245,0,0,0,0,0,91
	db 245,0,0,0,0,0,107,245,0,0,0,0,0,111,245,0,0,0,0,0,111,245,0,0,0,0,0,111,245,0,0,0,0,0,107,245,0,0,0,0,0,91,245,0,0,0,0
u_fr	db 255,255,255,85,85
ru_cor	db 0,0,0,3,255,255,240,0,0,0,3,170,170,176,1,85,84,3,149,85,176,1,170,164,3,149,85,176,2,191,232,3,149,85,176,255,255,255,255,255,229,176,255,255,255,255,255,101,176,255
	db 255,255,255,253,101,176,85,85,85,85,125,106,176,85,85,85,85,253,111,240,0,0,0,0,253,64,0,0,0,0,0,253,64,0,0,0,0,0,253,64,0,0,0,0,0,253,64,0,0,0
	db 0,0,253,111,0,0,0,0,0,253,107,0,0,0,0,0,253,91,0,0,0,0,0,253,91,0,0,0,0,0,253,91,0,0,0,0,0,253,107,0,0,0,0,0,253,111,0
l_fr	db 3,245,3,245,3,245,3,245,3,245,3,245,3,245,3,245
r_fr	db 253,64,253,64,253,64,253,64,253,64,253,64,253,64,253,64
ld_cor	db 0,91,245,0,0,0,0,0,107,245,0,0,0,0,0,111,245,0,0,0,0,0,111,245,0,0,0,0,0,111,245,0,0,0,0,0,107,245,0,0,0,0,0,91,245,0,0,0,0,0
	db 3,245,0,0,0,0,0,3,245,0,0,0,0,0,3,245,0,0,0,0,0,3,245,0,0,0,0,15,255,247,255,255,255,255,14,171,255,255,255,255,255,14,87,255,255,255,255,255,14,87
	db 213,85,85,85,85,14,87,85,85,85,85,85,14,86,170,128,41,86,128,14,85,86,192,58,170,192,14,85,86,192,63,255,192,14,170,170,192,0,0,0,15,255,255,192,0,0,0
rd_cor	db 0,0,0,0,253,111,0,0,0,0,0,253,107,0,0,0,0,0,253,91,0,0,0,0,0,253,91,0,0,0,0,0,253,91,0,0,0,0,0,253,107,0,0,0,0,0,253,111,0,0
	db 0,0,0,253,64,0,0,0,0,0,253,64,0,0,0,0,0,253,64,0,0,0,0,0,253,64,0,255,255,255,255,253,127,240,255,255,255,255,253,106,176,255,255,255,255,253,101,176,85,85
	db 85,85,86,101,176,85,85,85,85,85,229,176,2,149,104,3,170,165,176,3,170,172,3,149,85,176,3,255,252,3,149,85,176,0,0,0,3,170,170,176,0,0,0,3,255,255,240
d_fr	db 255,255,255,85,85

plasmatic:
INCLUDE plasmat.pix

font:
	db 124,238,238,254,238,238,238,252,238,238,252,238,238,252,124,238,238,224,238,238,124,252,238,238,238,238,238,252,254,224,224,248,224,224,254,254,224,224,248,224,224,224,124,238,224,254,238,238,124,238
	db 238,238,254,238,238,238,124,56,56,56,56,56,124,14,14,14,14,238,238,124,238,238,236,248,236,238,238,224,224,224,224,224,224,254,198,238,254,238,238,238,238,252,238,238,238,238,238,238,124,238
	db 238,238,238,238,124,252,238,238,252,224,224,224,124,238,238,238,254,238,126,252,238,238,252,238,238,238,124,238,224,124,14,238,124,254,56,56,56,56,56,56,238,238,238,238,238,238,254,238,238,238
	db 238,238,124,56,238,238,238,254,254,254,108,238,238,124,56,124,238,238,238,238,238,124,56,56,56,254,14,30,60,120,240,254,0,0,0,254,254,0,0,56,56,0,56,56,56,56,56,56,56,56
	db 0,56,56,28,28,56,0,0, 0,0,254,238,238,254,14,14,14,224,224,238,254,14,14,14

light_pal1 db 60, 60, 60
light_pal2 db 2, 2, 2, 30, 30, 25

creatorpal db 50, 50, 50,  0, 0, 50,  50, 0, 0
creatorpal2 db 20, 20, 63

credits_pal1:
	db 10, 10, 10
	db 30, 30, 30
	db 0, 0, 0
	db 40, 40, 40
	db 0, 0, 0
	db 50, 50, 50

satoridata:
	dw 20, 80
	dw 0, 0

	dw 90, 20
	dw 0, 0

	dw 120, 70
	dw 0, 0

text1:
	db "   YOU HAVE SEEN"
	db 13, 13
	db "   [ PLASMATIC ["
	db 13, 13
	db "THIS INTRO TOOK PART", 13
	db "  IN `KINTRO COMPO", 13
	db "   ON ASSEMBLY^_`", 13, 13
	db '  HOPE YOU LIKE IT', 0

text2:
	db "GREETINGS TO ALL", 13, 13
	db "  VISITORS OF", 13, 13
	db "  ASSEMBLY^_`", 0

text3:
	db "INTRO CODED BY ZDEN", 13
	db "OF SUNRISE SOFTWARE", 13
	db " MADE IN SLOVAKIA]", 0

xpos	db 0
ypos	db 0

VSP1	db 1
VSP2	db 22
HSP1	db 33
HSP2	db 199;77

endofcom:
plasmaframe:
satoritable:
shiftframe	EQU	offset plasmaframe + 16000
satoriframe	EQU	offset satoritable + 256*100
        code ends
	end start