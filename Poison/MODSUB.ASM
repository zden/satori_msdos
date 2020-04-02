	jumps
	.286

__code	segment para 'playsegsb'
	ASSUME  cs:__code, ds:__code, es:__code

	TestSpeed EQU 1375 ; +10000

;=======-  DMA 286 GLOBALS
;	GLOBAL  SetSampleRate:PROC, StartTransferDMA:PROC, TurnOffSpeaker:PROC
;	GLOBAL  TurnOnSpeaker:PROC, CalcForDMA :PROC
;	GLOBAL  HaltDma      :PROC, DspReset   :PROC  

;	GLOBAL  Status       :WORD, QuitDma      :WORD, BufferSeg    :Word
;	GLOBAL  Buffer1size  :WORD, Buffer2Size  :WORD
;	GLOBAL  BufferOffset1:WORD, BufferOffset2:WORD

;	GLOBAL  IntNumber:BYTE, BaseAddress:WORD

;=======-   MOD SUB GLOBALS

;	GLOBAL  StartPlaying_sb:NEAR, StopPlaying_sb:NEAR
;	GLOBAL  Mute:NEAR, UnMute:NEAR
;	GLOBAL  UpDate:NEAR

;	GLOBAL  ModSeg:WORD, ComputerSpeed:BYTE ;range 1-6?
;	GLOBAL  ModName:BYTE, MasterVolume:byte
	
;=======-   Grunge Work -====

	UPCOUNTDEF      equ 1
	VOL100          equ 256
	MAXBuffSize     equ 7000
	BUFFSIZE        equ 1320       ;speed 7=1484 6=1320 @ 10000hz
	
	; count    = 933,120,000/HZ (stored as DD) 
	;       116640 for 8000 hz 016C80h for 10000hz chart is below
	; Buffsize = HZ/10000*SPEED_CONST (stored as DW)

;*****************      
;***=- DATA -=****
;*****************      

	ComputerSpeed db    6

	COUNT   DW      06C80H  ;8000 HZ 116640
		DW      00001H

	COUNTTBL DW 0C780H  ;8000 HZ 116640     figures out the step for each
		DW      00001H                   ;  note that it plays- this number
		DW      06C80H  ;10000 HZ 93312     is divided by the FREQ of the 
		DW      00001H                  ;   note
		DW      02FC0H  ;12000 HZ 77760
		DW      00001H
		DW      0045BH  ;14000 HZ 66651
		DW      00001H
		DW      0E3D0H  ;16000 HZ 58320
		DW      00000H
		DW      0CA80H  ;18000 HZ 51840
		DW      00000H
		DW      0A5AEH  ;22000 HZ 42414
		DW      00000H

	Baseseg         dw      0           ;this may work, but I should have
	PatternSeg      dw      0           ;used records instead
	SampleSEG       dw      32 dup (0)

	MODTITLE        db      17h dup ("$"),0
	
	samplenames     db      32*22 dup (0)
	samLength       dw      32 dup (0)
	_volume          dw      32 dup (0)
	startloop       dw      32 dup (0)
	looplength      dw      32 dup (0)

	Waitforfreqvel  dw      0
	
	Buffer          db      20h dup (0)
	
	numofpatterns   db      0

	numofsequences  db      0
	idunno          db      0
	sequence        db      128 dup (0)
	
	DMABUFFER1      db      MAXBUFFSIZE dup (128)    ;first block
	DMABUFFER2      db      MAXBUFFSIZE dup (128)    ;2nd block
			;the size of the 'TOPLAY' buffers.  Note that there is two of 
			; them.. one plays while the other is updated
	
	CurByte         dw      4 dup (0)   ;more data for each of the 4 traks
	CurSeg          dw      4 dup (0)
	CurTrak         dw      4 dup (0)
	CurLeng         dw      4 dup (0)
	Special         dw      4 dup (0)
	CurLoopLength   dw      4 dup (0)
	CurStartLoop    dw      4 dup (0)
	Voloverride     dw      4 dup (0)

	TempFreq        dw      100h,100h,100h,100h 
	Freq            dw      100h,100h,100h,100h 

	CurVolume       db      12 dup (64)      ;volume is from 0 to 64

	CurOff          dw      offset DMABUFFER1
	NotCurOff       dw      offset DMABUFFER2

	curpattern      dw      0       ;curent pattern
	curnote         dw      0       ;current note in pattern
	cursequence     dw      0       ;current sequence

	MASTERVOLUME    db      255

	upcount         dw      UPCOUNTDEF
	Graphoff        dw      0
	
	TrakUpdateNum   db      0

;=====- SpecialData -======
	CommandChart:
		dw offset comArepeggiation ,offset ComSlideUp
		dw      offset comSlideDown ,offset _ControlEffect_g
		dw      offset _ControlEffect_s2 ,offset Donecom
		dw      offset Donecom      ,offset Donecom
		dw      offset _ControlEffect_s1,offset Donecom
		dw      offset comVolSlide  ,offset _TunnelMove
		dw      offset comVOLUMESET ,offset comBREAK
		dw      offset Donecom      ,offset comSetSpeed

	FreqVelocity    dw 4 dup (0)
	VolumeSlideFlag  dw 4 dup (0)
	VolumeSlideCount dw 4 dup (0)
	VolumeSlideCountTemp dw 4 dup (0)

;====- DMA DAC PLAYER DATA -====

	CurBuffsize     dw      0
	NewBufferFlag   dw      0
	NewBufferSize   dw      0
	
	BUFFSIZECHart   dw      336, 500     ;spd 00,01
					i=2
		REPT 30
				dw      i*TestSpeed/7
			i=i+1
		ENDM

	;speed 7=1484 6=1320 @ 10000hz
	; count    = 933,120,000/HZ (stored as DD) 
	;       116640 for 8000 hz 016C80h for 10000hz chart is below
	; Buffsize = HZ/10000*SPEED_CONST (stored as DW)

;========- Char Data -========
	
	HzTable dw      8000,10000,12000,14000,16000,18000,22000
	Hz      dw      0
	FreqWait dw     0
	SampleRate db   0

;=====SUBROUTINES=====

SelectHz proc near
	pusha
	mov     ax,cs
	mov     ds,ax
	mov     es,ax
	mov     bl,cs:[ComputerSpeed]   ;1-6
	xor     bh,bh
	push    bx
	shl     bx,1
	mov     ax,[hzTable+bx]
	mov     [hz],ax
	push    ax
	mov     cx,350
	mul     cx
	mov     cx,10000
	div     cx
	mov     [freqwait],ax
	pop     cx
	mov     dx,10000
	mov     ax,100
	mul     dx
	div     cx
	neg     al
	mov     [samplerate],al
	pop     bx
	shl     bx,2
	mov     ax,[counttbl+bx]
	mov     [count],ax
	mov     ax,[counttbl+bx+2]
	mov     [count+2],ax
	mov     bl,6    ;default speed 6
	xor     bh,bh
	shl     bx,1
	mov     ax,[BUFFSIZECHart+bx]
	mov     cx,[hz]
	mul     cx
	mov     cx,10000
	div     cx
	mov     [NEWBuffersize],ax
	popa
	ret
endp selectHz

Readnotes proc near

	mov     ax,cs
	mov     ds,ax
	mov     es,ax
	mov     bx,0
	cmp     [NewBufferFlag],2
	jne     Clearloop
	mov     [NewBufferFlag],bx     ;0
	mov     [newbuffersize],bx   ;0
Clearloop:
	mov     [FreqVelocity+bx],0
	mov     [VolumeSlideFlag+bx],0
	mov     [VolumeSlideCount+bx],600    ;any large number will do
										;in other words, I don't know how
	add     bx,2                       ;to do this
	cmp     bx,8
	jb      clearloop
readnotestart:
	mov     si,[curpattern]
	shl     si,10           ;multiply by 1024
	mov     ax,[curnote]
	shl     ax,4            ;mult by 16
	add     si,ax           ;pointer to next notes si holds it

	mov     ax,[patternseg]
	mov     ds,ax
	mov     es,ax

	mov     bp,0    ;counter 
getnotesloop:
	mov     bx,bp
	shl     bx,1 
	lodsb           ;high nybble for inst/ low nybble for high freq
	mov     dh,al
	lodsb           ;byte for freq
	mov     dl,al   ;dx holds freq
	lodsb           ;high nybble for low inst/ low nybble  command
	mov     cl,al
	xor     ch,ch
	shr     cx,4
	mov     ah,cl
	mov     ch,al
	lodsb           ;xy for command
	mov     cl,al
	push    cx
	mov     al,dh
	and     al,11110000b
	or      al,ah
	xor     ah,ah
	or      ax,ax
	je      dontstoreinst
	mov     cs:[curtrak+bx],ax      ;store inst
dontstoreinst:
	mov     al,dh
	and     dh,01111b
	mov     cx,dx
	cmp     cx,5
	jbe     Dontdofreq

	mov     ax,cs:[cOUNT]
	mov     dx,cs:[COUNT+2]
	div     cx
	mov     cs:[freq+bx],ax      ;store freq
dontdofreq:                
	mov     ax,bx
	mov     cs:[voloverride+bx],0
	pop     cx
	and     ch,1111b
	mov     bl,ch   ;ch=command #
	xor     bh,bh
	shl     bx,1
	jmp     word ptr cs:[CommandChart+bx]
Donecom:
	inc     bp
	cmp     bp,4
	jae     noupdatenote
	jmp     getnotesloop
Noupdatenote:

	ret      
readnotes endp


	 ;cl = commandXY upon entry
	 ;AX = offset for trak*2 (0,2,4,6)
comSlideUp:
	xor     ch,ch
	mov     bx,ax   ;step increases, raising freq
	mov     cs:[FreqVelocity+bx],cx
	jmp     Donecom

comSlideDown:
	xor     ch,ch
	not     cx      ;make it so step value decreases, lowering freq
	mov     bx,ax
	mov     cs:[FreqVelocity+bx],cx
	jmp     Donecom

comArepeggiation:
	jmp     Donecom

comSlide2Note:
	jmp     Donecom

comVibrato:
	jmp     Donecom

comVolSlide:
	xor     ch,ch
	mov     bx,ax
	test    cl,11110000b    ;see if 
	jnz     vslideup
	mov     cs:[volumeSlideFlag],-1
	jmp     SetVolRate
Vslideup:
	shr     cl,4
	mov     cs:[volumeSlideFlag],1
SetvolRate:
	and     cl,1111b
	xor     cl,1111b        ;reverse
	inc     cl
	shl     cx,4
	mov     cs:[VolumeSlideCount+bx],cx
	jmp     Donecom

comSetSpeed:
	cmp     cl,1fh
	jae     nospeedchange
	mov     bl,cl
	xor     bh,bh
	shl     bx,1
	mov     ax,cs:[BUFFSIZECHart+bx]
	mov     cx,cs:[hz]
	mul     cx
	mov     cx,10000
	div     cx
	mov     cs:[NEWBuffersize],ax
NoSpeedChange:
	jmp     Donecom

comVOLUMESet:
	mov     bx,ax
	xor     ch,ch
	or      cl,cl
	je      sokos
	dec     cl
	and     cl,111111b
sokos:
	mov     cs:[curvolume+bx],cl
	mov     cs:[voloverride+bx],1
	jmp     Donecom

comJUMP:
	mov     cS:[curnote],0
	mov     bl,cl
	xor     bh,bh
	mov     al,cS:[sequence+bx]
	xor     ah,ah
	mov     cS:[curpattern],ax
	jmp     readnotestart

comBREAK:
	mov     cs:[curnote],64
	jmp     Donecom

;add commands above here and enter them on offset chart above

Update  proc    far
	push	ds
	cmp     cS:[status],3
	je      LetsDoit3
	CMP     cs:[status],4
	je      LetsDoit4
	pop	ds
	ret
LetsDoit3:
	pusha
	mov     ax,offset DMABUFFER1
	mov     cs:[curoff],ax
	mov     ax,offset DMABUFFER2
	mov     cs:[notcuroff],ax
	cmp     cs:[NewBufferSize],0
	je      skipnewsize
	mov     ax,cs:[NewBuffersize]
	mov     cs:[buffer1size],ax
	call    calcforDma
	inc     cs:[NewBufferFlag]
Skipnewsize:
	mov     ax,cs:[buffer1size]
	mov     cs:[curBUFFSIZE],ax
	
	mov     ax,cs
	mov     ds,ax
	mov     es,ax
	mov     di,[curoff]
	mov     ax,8080h
	mov     cx,cs:[CurBUFFSIZE]
	shr     cx,1
	jnc     Nostosb
	stosb
Nostosb:
	rep     stosw                   ;1st thing:clear data area to 128's
	
	jmp     updateitnow
Letsdoit4:
	pusha
	mov     ax,offset DMABUFFER2
	mov     cs:[curoff],ax
	mov     ax,offset DMABUFFER1
	mov     cs:[notcuroff],ax
	
	cmp     cs:[NewBufferSize],0
	je      skipnewsize2
	mov     ax,cs:[NewBuffersize]
	mov     cs:[buffer2size],ax
	call    calcforDma
	inc     cs:[NewBufferFlag]
Skipnewsize2:
	mov     ax,cs:[buffer2size]
	mov     cs:[curBUFFSIZE],ax

	mov     ax,cs
	mov     ds,ax
	mov     es,ax
	mov     di,[curoff]
	mov     ax,8080h
	mov     cx,cs:[CurBUFFSIZE]
	shr     cx,1
	jnc     NoStosb2
	stosb
Nostosb2:
	rep     stosw                   ;1st thing:clear data area to 128's

Updateitnow:
	mov     ax,cs
	mov     ds,ax
	mov     es,ax
	add     [status],2
	
	mov     bp,4            ;start on trak 4
MainUpdateLoop:
	mov     bx,bp
	dec     bx
	shl     bx,1
	mov     ax,[CurTrak+bx]
	or      ax,ax
	jz      nextup
	inc     [trakupdatenum]
	jmp     TrakNotEmpty     
NextUp:
	mov     ax,cs
	mov     ds,ax
	mov     es,ax
	dec     bp
	jne     MainUpdateLoop  ;done w/data- now get next notes
	
	mov     ax,cs
	mov     ds,ax
	mov     es,ax

;now do mainvolume
;  cmp     [MASTERVOLUME],255
;  je      SkipMasterVolume
;  mov     si,[Curoff]
;  mov     di,si
;  mov     cx,[CurBuffSize]
;  mov     bl,[MasterVolume]
;  shr     bl,2
;  mov     bh,128
;MastLoop:
;  mov     al,[si]
;  inc     si            ;same as lodsb
;  add     al,bh
;  imul    bl
;  sar     ax,6
;  sub     al,bh
;  mov     [di],al    ;same as stosb
;  inc     di
;  dec     cx
;  jne     MastLoop

SkipMasterVolume:    
	inc     [curnote]
	cmp     [curnote],64
	jb      nonewpattern

	mov     [curnote],0
	inc     [Cursequence]
	mov     bx,[cursequence]
	cmp     bl,[numofsequences]
	jbe     notendofsong
	mov     [cursequence],0
	xor     bx,bx
notendofsong:
	mov     al,[sequence+bx]
	xor     ah,ah
	mov     [curpattern],ax
	mov	bx, ax
	int 3
	inc	[_played_pattern+bx]
nonewpattern:
	call    readnotes
	popa
	pop	ds
	ret

TrakNotEmpty:
	or      ah,ah
	jz      TraknoCont
	jmp     trakcont
TraknoCont:
	push    ax
	mov     ax,cS:[freqwait]
	mov     cs:[waitforfreqvel],ax
	pop     ax
	mov     cx,ax
	mov     ah,al           ;copy voice number to high to say 'now playing'
	mov     cS:[CurTrak+bx],ax
	xchg    bx,cx
	dec     bx                        ;bx has file#, cx has trak#
	shl     bx,1
	mov     ax,cS:[Sampleseg+bx]
	mov     dx,cs:[Samlength+bx]
	mov     si,cS:[_volume+bx]
	xchg    bx,cx
	mov     cS:[Curseg+bx],ax     ;set up for playing
	mov     cs:[CurLeng+bx],dx
	mov     cs:[CurByte+bx],0
	cmp     cS:[voloverride+bx],0
	jne     novoltoday
	mov     ax,si
	xor     ah,ah
	dec     al
	and     al,111111b
	mov     cs:[curvolume+bx],al
novoltoday:
	mov     cs:[special+bx],0
	xchg    bx,cx
	cmp     cs:[looplength+bx],10
	jb      trakcont
	mov     ax,cs:[looplength+bx]
	mov     dx,cS:[startloop+bx]
	xchg    bx,cx
	mov     cs:[curlooplength+bx],ax
	mov     cs:[curstartloop+bx],dx
	mov     cs:[special+bx],1
TrakCont:
	mov     bx,bp
	dec     bx
	shl     bx,1
	mov     di,cs:[CurOff]     ;set back to start of data block
	mov     si,cs:[CurByte+bx]
	mov     ax,cs:[curseg+bx]
	mov     es,ax
	xor     dh,dh
	xor     cx,cx
LittleLoop:
	mov     al,es:[si]         ;from seg es
	mov     ah,cs:[curVolume+bx]
	imul    ah
	add     [di],ah           ;add in the new volume adjust piece of data

	inc     di
	inc     cx
	cmp     cx,[curBUFFSIZE]
	jae     cleanup
	mov     ax,[Freq+bx]
				;below does the freq slides
	dec     [waitforfreqvel+bx]
	jne     skipfreqchange
	push    ax
	mov     ax,[freqwait]
	mov     [waitforfreqvel+bx],ax
	pop     ax
	add     ax,[freqvelocity+bx]
	mov     [freq+bx],ax

	;FreqVelocity    dw 
	;VolumeVelocity  dw
SkipFreqChange:        
	add     ax,cs:[tempfreq+bx]
	mov     dl,ah
	xor     dh,dh
	add     si,dx
	xor     ah,ah
	mov     cs:[tempfreq+bx],ax

	cmp     si,cs:[CurLeng+bx]
	jb      LittleLoop
	cmp     cs:[special+bx],1
	jne     norepeat

	mov     si,cs:[curstartloop+bx]
	mov     ax,cs:[curlooplength+bx]
	add     ax,si     
	cmp     ax,cs:[curleng+bx]
	jae     looptoobig
	mov     cs:[curleng+bx],ax
looptoobig:
	jmp     littleloop
norepeat:
	mov     cs:[CurTrak+bx],0
	jmp     nextup
Cleanup:
	mov     cS:[CurByte+bx],si
	jmp     nextup
Update endp

	oldintoff       dw      0
	oldintseg       dw      0
	keypress        db      0
Doint proc near     ;sets up the keyboard interrupt
	pusha
	mov     ah,35h
	mov     al,15h
	int     21h
	mov     cs:[oldintoff],bx
	mov     cs:[oldintseg],es
	mov     al,15h
	mov     ah,25h
	mov     dx,offset keybdint
	mov     bx,__code
	mov     ds,bx
	int     21h
	popa
	ret
Doint endp

unDoint proc near   ;undoes the key interupt
	pusha
	mov     dx,cs:[oldintoff]
	mov     ax,cs:[oldintseg]
	mov     ds,ax
	mov     al,15h
	mov     ah,25h
	int     21h
	popa
	ret
unDoint endp

keybdint:
	cmp     ah,4fh
	jne     noint
	PUSHA
	mov     bx,__code
	mov     ds,bx
	mov     ds:[keypress],al
	mov     al,7
isesc:
	stc
	popa
	iret
noint:
	jmp    dword ptr cS:[oldintoff]

	;bx= number of screen updates to wait
Delay proc near
	pusha

	mov     dx,03dah
vr12:
	in      al,dx
	test    al,08
	jz      vr12        ;wait for a verticle retrace
	
Nvr12:
	in      al,dx
	test    al,08
	jnz     Nvr12       ;wait for a verticle retrace to end

	dec     bx
	jne     vr12
	popa
	ret
endp delay

;=====END=SUBROUTINES=====

Mute PROC NEAR
	call    TurnOffSpeaker
	ret
Mute ENDP

UnMute PROC NEAR
	call    TurnOnSpeaker
	ret
UnMute ENDP

StartPlaying_sb PROC	far
	push	ds
	pusha
	mov     ax,cs
	mov     ds,ax
	mov     es,ax

;	mov     bx,[ModSeg]
	assume	ds:nothing
	mov	ax,code
	mov	ds,ax
	mov	bx, ds:[_module_seg2]
	assume	ds:__code
	push	cs
	pop	ds
	mov     [patternseg],bx
	add     bx,64*10
	mov     [baseseg],bx

	mov     [BufferSeg],cs
	mov     [Buffer1Size],BUFFSIZE
	mov     [Buffer2Size],BUFFSIZE
	mov     [BufferOffset1],offset DMABUFFER1
	mov     [BufferOffset2],offset DMABUFFER2

	call    FILESTUFF

	or      ax,ax
	jne     testsound
ERROR:
	popa
	mov     ax,-1         ;says Hey, I failed.
	pop	ds
	ret

TESTSOUND:      
	call    SelectHz
	call    DspReset 
	cmp     al,0
	je      NoDMAError
	jmp     ERROR
NoDMAError:
	call    turnonspeaker
	mov     ah,cs:[samplerate]
	call    SetSampleRate
	call    CalcForDma

	mov     [curnote],0
	mov     bx,[cursequence]
	mov     al,[sequence+bx]
	xor     ah,ah
	mov     [curpattern],ax
	mov	bx, ax
	inc	[_played_pattern+bx]

	call    readnotes
	call    StartTransferDma     ;start output
	popa
	xor     ax,ax
	pop	ds
	ret
StartPlaying_sb ENDP

StopPlaying_sb PROC	far
	push	ds
	MOV     cs:[quitdma],1
	call    turnoffspeaker
	mov     bx,40
	call    delay   ;wait until the interrupt resets itself..
	pop	ds
	ret
StopPlaying_sb ENDP

include	dmasub.asm

mod_read	proc
;assume	ds:nothing
	pusha
	push	ds
	pop	es
	assume	ds:nothing
	mov	ax, seg module
	mov	bx, offset module
	shr	bx, 4
	add	ax, bx
	mov	ds, ax
	mov	si, cs:[_mod_pos]
	mov	di, dx
	add	cs:[_mod_pos], cx
	rep	movsb
	push	es
	pop	ds
	popa
	ret

;	push	ax
;	push	bx
;	push	cx
;	push	dx
;	push	ds
;	mov	ax,3F00h
;	mov	bx,cs:[FileHandle]
;	int	21h
;	pop	ds
;	pop	dx
;	pop	cx
;	pop	bx
;	pop	ax
;	ret

_mod_pos	dw 0

mod_read	endp

FILESTUFF proc near
	jmp     startfiles
ABORTER:
	xor     ax,ax
	ret
startfiles:
	mov     ax,cs
	mov     ds,ax
	mov     es,ax
	
;	mov     dx,offset modname       ;where name is
;	mov     ax,3d00h        ;open file
;	int     21h
;	jnc     noaborter
;	jmp     aborter
;noaborter:
;	mov     bx,ax           ;put filehandle in BX
	
	mov     dx,offset modtitle
	mov     cx,014h
	call	mod_read
;	mov     ax,3f00h
;	int     21h             ;reads title

	push    bp
	mov     bp,0
sampleloop:
	mov     DX,offset buffer
	mov     cx,01eh
	call	mod_read
;	mov     ax,3f00h
;	int     21h

	mov     si,offset buffer
	mov     di,bp
	imul    di,di,22
	add     di,offset samplenames
	mov     cx,22
copynameloop:
	lodsb
	cmp     al,158
	jbe     okprint1
	mov     al," "
okprint1:
	cmp     al," "
	jae     okprint2
	mov     al," "
okprint2:
	stosb
	loop    copynameloop

	mov     DI,bp
	shl     di,1
	add     di,offset samlength

	lodsb
	mov     ah,al
	lodsb
	shl     ax,1
	stosw

	mov     di,bp
	shl     di,1
	add     di,offset _volume
	lodsb
	mov     ah,al
	lodsb
	stosw

	mov     di,bp
	shl     di,1
	add     di,offset startloop
	lodsb
	mov     ah,al
	lodsb
	stosw

	mov     di,bp
	shl     di,1
	add     di,offset looplength
	lodsb
	mov     ah,al
	lodsb
	shl     ax,1
	stosw

	inc     bp
	cmp     bp,31
	jb      sampleloop
	pop     bp

	mov     dx,offset numofsequences
	mov     cx,66
	call	mod_read
;	mov     ax,3f00h
;	int     21h

;	sub     cx,cx                  
	mov     dx,043ch                
	mov	_mod_pos, dx
;	mov     ax,4200h             
;	int     21h             ;move file pointer to 1st pattern              

	mov     si,offset sequence
	xor     cx,cx
findbiggest:
	lodsb   
	cmp     al,cS:[numofpatterns]
	jbe     notbiggest
	mov     cs:[numofpatterns],al
Notbiggest:
	inc     cx
	cmp     cl,cS:[numofsequences]
	jb      findbiggest
	inc     cs:[numofpatterns]

	push    ds
	mov     ax,cs:[patternseg]
	mov     ds,ax
	mov     cl,cs:[numofpatterns]
	xor     ch,ch
	imul    cx,cx,1024
	xor     dx,dx
	call	mod_read
;	mov     ax,3f00h
;	int     21h             ;reads all the patterns
	pop     ds              ;file pointer now points to 1st sample

	push    ds
	push    bp

	xor     bp,bp
loadsampleloop:
	mov     si,bp
	shl     si,1
	mov     ax,cS:[baseseg]
	mov     cs:[si+sampleSEG],ax
	mov     ds,ax
	mov     dx,0
	mov     cx,cS:[si+samlength]
	or      cx,cx
	je      skipsampleload

	call	mod_read
;	mov     ax,3f00h
;	int     21h     ;reads the current sample
			; and puts it at offset 0 of a segment

	mov     cx,cs:[si+samlength]
	add     cx,15
	shr     cx,4
	add     cs:[baseseg],cx ;move the baseseg pointer past end of sample
skipsampleload:        
	inc     bp
	cmp     bp,31
	jb      loadsampleloop

	pop     bp
	pop     ds        

;	mov     ax,3e00h
;	int     21h             ;Close filehandle (BX)
;	mov     ax,1
	ret
FILESTUFF endp

assume	ds:nothing

__store_command	proc
;	mov	bx, PatternSpeed
;	cmp	PatternCount, bx
;	jnz	@@ControlEffect1j
	push	ds
	push	ax
	mov	ax, seg code
	mov	ds, ax
	pop	ax
	cmp	ds:[control_commands_store_new], 1
	jz	@@ControlEffect1SN
	cmp	ds:[control_commands_count], 12
	ja	@@nostore1
	mov	bx, ds:[control_commands_count]
	mov	ds:[control_commands+bx], cl
@@nostore1:
	inc	ds:[control_commands_count]
	jmp	@@ControlEffect1SN2
@@ControlEffect1SN:
	cmp	ds:[control_commands_count_new], 12
	jz	@@nostore2
	mov	bx, ds:[control_commands_count_new]
	mov	ds:[control_commands_new+bx], cl
@@nostore2:
	inc	ds:[control_commands_count_new]
@@ControlEffect1SN2:
	pop	ds
@@ControlEffect1j:
	ret
__store_command	endp

_store_command_set1	proc
	push	bx
	mov	bx, cs:[curpattern]
	cmp	cs:[_played_pattern+bx], 0
	pop	bx
	jnz	@@e
	call	__store_command
@@e:
	ret
_store_command_set1	endp

_store_command_set2	proc
	push	bx
	mov	bx, cs:[curpattern]
	cmp	cs:[_played_pattern+bx], 1
	pop	bx
	jnz	@@e
	call	__store_command
@@e:
	ret
_store_command_set2	endp

_ControlEffect_s1	proc
	call	_store_command_set1
	jmp	Donecom
_ControlEffect_s1	endp

_ControlEffect_s2	proc
	call	_store_command_set2
	jmp	Donecom
_ControlEffect_s2	endp

_TunnelMove	proc
;	or	cl, cl
;	jz	@@e
	push	ds
	mov	ax, seg code
	mov	ds, ax
	mov	ds:[sb_tunnel], 1
;	inc	ds:[tunnel_pos]
	pop	ds
;@@e:
	jmp	Donecom
_TunnelMove	endp

_ControlEffect_g	proc
	call	__store_command
	jmp	Donecom
	ret
_ControlEffect_g	endp

_played_pattern	db -1,-1,-1,-1,-1, -1,-1,-1,-1,-1

__code	ends