	Comment &

	Gus play routine V 1.5 by Robert Adolfsson - CASCADA
	Not for release!

	NosieTracker/SoundTracker/StarTrekker/FastTracker/OctaComposer compatible

	&

	Jumps
	.286

_Code	Segment Para 'PlaySeg'

	assume	cs:_Code

;	Publics

Public	MainVolume,PlayingPattern,RealPPattern,PatternRow,RealRow,OrderLen,ChanOn,NumChans,Time,SampChans,Bar,BasePort,ErrorCode
Public	LoadMod,ClearMem,StartPlaying,StopPlaying,Init,InitDevice

public	end_gus_play, BasePort

;	Externs

;	Structures

ChanSize	Equ 52

ChanInfo	Struc

SampOff		dd 0
FreqVal 	dw 0
Vol 		dw 0
OldVol		dw 0
Fine 		dw 0
Repeat 		dw 0
RepLen 		dw 0
Len 		dw 0
Amiga 		dw 0
Effect 		dw 0
VibratoPek 	dw 0
OldVibrato 	dw 0
PortTo 		dw 0
OldPortTo 	dw 0
EffectTime	dw 0
CurrSamp	dw 0
OldSamp		dw 0
Arp 		dw 0,0,0
ArpCounter 	dw 0
InstSet		dw 0
LoopOnOff	dw 0
OffsetAdd	dw 0
Bar		dw 0

ChanInfo	EndS

; Number of voices to use

NumVoices	Equ 20

; UltraSound Ports

StatusPort	Equ 6h
TimerCtrlPort	Equ 8h
TimerDataPort	Equ 9h
MidiCtrlPort	Equ 100h
MidiDataPort	Equ 101h
ActiveVoicePort	Equ 102h
CommandPort	Equ 103h
DataLowPort	Equ 104h
DataHighPort	Equ 105h
DRAMIOPort	Equ 107h

; UltraSound Commands

WriteVoiceMode	Equ 00h
SetVoiceFreq	Equ 01h		; Value=Freq/Divisor
LoopStartLo	Equ 02h
LoopStartHi	Equ 03h
SampleEndLo	Equ 04h
SampleEndHi	Equ 05h
VolRampRate	Equ 06h
VolRampStart	Equ 07h
VolRampEnd	Equ 08h
SetVolume	Equ 09h
SampleStartLo	Equ 0Ah
SampleStartHi	Equ 0Bh
VoiceBalance	Equ 0Ch
VolumeCtrl	Equ 0Dh
VoicesActive	Equ 0Eh
DMACtrl		Equ 41h
DRAMAddrLo	Equ 43h
DRAMAddrHi	Equ 44h
Initialize	Equ 4Ch
ReadVolume	Equ 89h
VoicePosLo	Equ 8Ah
VoicePosHi	Equ 8Bh
ReadVolCtrl	Equ 8Dh

; Divisors

Voices14	Equ 4300
Voices15	Equ 4000
Voices16	Equ 3700
Voices17	Equ 3500
Voices18	Equ 3300
Voices19	Equ 3100
Voices20	Equ 3030
Voices21	Equ 2800
Voices22	Equ 2700
Voices23	Equ 2600
Voices24	Equ 2500
Voices25	Equ 2400
Voices26	Equ 2300
Voices27	Equ 2200
Voices28	Equ 2100
Voices29	Equ 2000
Voices30	Equ 2000
Voices31	Equ 1900
Voices32	Equ 1800

;	Variables

MKSign		db 'M.K.FLT46CHN8CHNOCTA'
MKMod		db 0
Info		db 1084 dup (0)
OrderLen	db 0
Restart		db 0
PatternOrder	db 128 dup (0)
SinTab		db 0,25,50,74,98,120,142,162,180,197,212,225,236
		db 244,250,254,255,254,250,244,236,225,212,197,180
		db 162,142,120,98,74,50,25

ArpTable	db 70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70
		db 70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,68,68,68,68,68,68,68,66,66,66,66,66,66,66,64,64,64,64,64,64,64,64,62,62,62,62,62,62,62,62,60,60,60,60,60,60,60,60,58,58,58,58,58,58
		db 58,58,58,56,56,56,56,56,56,56,56,56,56,54,54,54,54,54,54,54,54,54,54,52,52,52,52,52,52,52,52,52,52,50,50,50,50,50,50,50,50,50,50,50,50,48,48,48,48,48,48,48,48,48,48,48,48,46,46,46,46,46,46,46,46,46,46,46,46,44,44,44,44,44,44,44,44,44,44
		db 44,44,44,44,42,42,42,42,42,42,42,42,42,42,42,42,42,42,40,40,40,40,40,40,40,40,40,40,40,40,40,40,40,38,38,38,38,38,38,38,38,38,38,38,38,38,38,38,38,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,34,34,34,34,34,34,34,34,34,34,34,34,34
		db 34,34,34,34,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,28,28,28,28,28,28,28,28,28,28,28,28,28,28,28,28,28,28,28,28,28,26,26,26,26,26,26,26,26,26,26,26,26,26
		db 26,26,26,26,26,26,26,26,26,26,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20
		db 20,20,20,20,20,20,20,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,14,14,14,14,14,14,14,14,14,14,14,14,14,14
		db 14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10
		db 10,10,10,10,10,10,10,10,10,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4
		db 4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0

PanRegs		db 6,9,9,6,0,0,0,0,0,0,0,0,0,0,0,0
		db 6,6,9,9,9,6,0,0,0,0,0,0,0,0,0,0
		db 6,6,9,9,9,9,6,6,0,0,0,0,0,0,0,0

EvenData

AmigaVals	dw 856,808,762,720,678,640,604,570,538,508,480,453 ;C-1 to B-1 Finetune +0.
		dw 428,404,381,360,339,320,302,285,269,254,240,226 ;C-2 to B-2 Finetune +0.
		dw 214,202,190,180,170,160,151,143,135,127,120,113 ;C-3 to B-3 Finetune +0.

		dw 850,802,757,715,674,637,601,567,535,505,477,450 ;C-1 to B-1 Finetune +1.
		dw 425,401,379,357,337,318,300,284,268,253,239,225 ;C-2 to B-2 Finetune +1.
		dw 213,201,189,179,169,159,150,142,134,126,119,113 ;C-3 to B-3 Finetune +1.

		dw 844,796,752,709,670,632,597,563,532,502,474,447 ;C-1 to B-1 Finetune +2.
		dw 422,398,376,355,335,316,298,282,266,251,237,224 ;C-2 to B-2 Finetune +2.
		dw 211,199,188,177,167,158,149,141,133,125,118,112 ;C-3 to B-3 Finetune +2.

		dw 838,791,746,704,665,628,592,559,528,498,470,444 ;C-1 to B-1 Finetune +3.
		dw 419,395,373,352,332,314,296,280,264,249,235,222 ;C-2 to B-2 Finetune +3.
		dw 209,198,187,176,166,157,148,140,132,125,118,111 ;C-3 to B-3 Finetune +3.

		dw 832,785,741,699,660,623,588,555,524,495,467,441 ;C-1 to B-1 Finetune +4.
		dw 416,392,370,350,330,312,294,278,262,247,233,220 ;C-2 to B-2 Finetune +4.
		dw 208,196,185,175,165,156,147,139,131,124,117,110 ;C-3 to B-3 Finetune +4.

		dw 826,779,736,694,655,619,584,551,520,491,463,437 ;C-1 to B-1 Finetune +5.
		dw 413,390,368,347,328,309,292,276,260,245,232,219 ;C-2 to B-2 Finetune +5.
		dw 206,195,184,174,164,155,146,138,130,123,116,109 ;C-3 to B-3 Finetune +5.

		dw 820,774,730,689,651,614,580,547,516,487,460,434 ;C-1 to B-1 Finetune +6.
		dw 410,387,365,345,325,307,290,274,258,244,230,217 ;C-2 to B-2 Finetune +6.
		dw 205,193,183,172,163,154,145,137,129,122,115,109 ;C-3 to B-3 Finetune +6.

		dw 814,768,725,684,646,610,575,543,513,484,457,431 ;C-1 to B-1 Finetune +7.
		dw 407,384,363,342,323,305,288,272,256,242,228,216 ;C-2 to B-2 Finetune +7.
		dw 204,192,181,171,161,152,144,136,128,121,114,108 ;C-3 to B-3 Finetune +7.

		dw 907,856,808,762,720,678,640,604,570,538,504,480 ;C-1 to B-1 Finetune -8.
		dw 453,428,404,381,360,339,320,302,285,269,254,240 ;C-2 to B-2 Finetune -8.
		dw 226,214,202,190,180,170,160,151,143,135,127,120 ;C-3 to B-3 Finetune -8.

		dw 900,850,802,757,715,675,636,601,567,535,505,477 ;C-1 to B-1 Finetune -7.
		dw 450,425,401,379,357,337,318,300,284,268,253,238 ;C-2 to B-2 Finetune -7.
		dw 225,212,200,189,179,169,159,150,142,134,126,119 ;C-3 to B-3 Finetune -7.

		dw 894,844,796,752,709,670,632,597,563,532,502,474 ;C-1 to B-1 Finetune -6.
		dw 447,422,398,376,355,335,316,298,282,266,251,237 ;C-2 to B-2 Finetune -6.
		dw 223,211,199,188,177,167,158,149,141,133,125,118 ;C-3 to B-3 Finetune -6.

		dw 887,838,791,746,704,665,628,592,559,528,498,470 ;C-1 to B-1 Finetune -5.
		dw 444,419,395,373,352,332,314,296,280,264,249,235 ;C-2 to B-2 Finetune -5.
		dw 222,209,198,187,176,166,157,148,140,132,125,118 ;C-3 to B-3 Finetune -5.

		dw 881,832,785,741,699,660,623,588,555,524,494,467 ;C-1 to B-1 Finetune -4.
		dw 441,416,392,370,350,330,312,294,278,262,247,233 ;C-2 to B-2 Finetune -4.
		dw 220,208,196,185,175,165,156,147,139,131,123,117 ;C-3 to B-3 Finetune -4.

		dw 875,826,779,736,694,655,619,584,551,520,491,463 ;C-1 to B-1 Finetune -3.
		dw 437,413,390,368,347,338,309,292,276,260,245,232 ;C-2 to B-2 Finetune -3.
		dw 219,206,195,184,174,164,155,146,138,130,123,116 ;C-3 to B-3 Finetune -3.

		dw 868,820,774,730,689,651,614,580,547,516,487,460 ;C-1 to B-1 Finetune -2.
		dw 434,410,387,365,345,325,307,290,274,258,244,230 ;C-2 to B-2 Finetune -2.
		dw 217,205,193,183,172,163,154,145,137,129,122,115 ;C-3 to B-3 Finetune -2.

		dw 862,814,768,725,684,646,610,575,543,513,484,457 ;C-1 to B-1 Finetune -1.
		dw 431,407,384,363,342,323,305,288,272,256,242,228 ;C-2 to B-2 Finetune -1.
		dw 216,203,192,181,171,161,152,144,136,128,121,114 ;C-3 to B-3 Finetune -1.

;GusVol		dw 20000,39120,41376,42656,43936,45072,45696,46240,46848,47408
;		dw 47952,48528,49072,49360,49632,49920,50160,50432,50704,50928
;		dw 51168,51424,51680,51952,52160,52448,52672,52912,53152,53312
;		dw 53440,53584,53664,53808,53952,54048,54144,54288,54400,54496
;		dw 54608,54720,54832,54944,55072,55184,55312,55440,55552,55696
;		dw 55760,55888,56016,56096,56240,56304,56448,56528,56672,56752
;		dw 56896,56976,57136,57216
GusVol		dw 20000,45120,47376,48656,49936,51072,51696,52240,52848,53408
		dw 53952,54528,55072,55360,55632,55920,56160,56432,56704,56928
		dw 57168,57424,57680,57952,58160,58448,58672,58912,59152,59312
		dw 59440,59584,59664,59808,59952,60048,60144,60288,60400,60496
		dw 60608,60720,60832,60944,61072,61184,61312,61440,61552,61696
		dw 61760,61888,62016,62096,62240,62304,62448,62528,62672,62752
		dw 62896,62976,63136,63216

SampOffset	dd 31 dup (0)
SampLen		dw 31 dup (0)
SampVol		dw 31 dup (0)
SampFine	dw 31 dup (0)
SampRep		dw 31 dup (0)
SampRepLen	dw 31 dup (0)

BasePort	dw 220h
FileHandle	dw 0
NumChans	dw 4			; 4,6 or 8 (Defualt = 4)
ChanOn		dw 0FFh
ChanOnCount	dw 0
PatternSpeed	dw 0
PatternCount	dw 0
PatternRow	dw 0
RealRow		dw 0
PlayingPattern	dw 0
RealPPattern	dw 0
MainVolume	dw 64
TempVol		dw 0
ErrorCode	dw 0

FreqTable	dw 908 dup (0)

PatternPek	equ this DWord
BytePattern	dw 0
SegPattern	dw 0

SampMem		dw 0
PatternMem	dw 0
BreakData	dw 0
OldTimer	dd 0
GUSMem		dd 0
Speed		dd 0
SpeedAdd	dd 0
Counter		dw 0
Time		dd 0
JumpOldTimer	dw 0

SampChans	ChanInfo 8 dup (<>)

; 	Macros

NopLoop		Macro	Nops
Local	NopLoop

	push	cx
	mov	cx,Nops
NopLoop:
	nop
	loop	NopLoop
	pop	cx

		EndM

;	The Code

Even

	assume	ds:_Code

Pattern		Proc	Near

	pusha
	push	ds
	push	es

	mov	ax,_Code
	mov	ds,ax

	mov	al,20h
	out	20h,al
	sti

	add	Word Ptr [Time],1
	adc	Word Ptr [Time+2],0

	inc	[Counter]
	mov	ax,Word Ptr [Speed+2]
	add	Word Ptr [SpeedAdd+2],ax
	mov	ax,Word Ptr [Speed]
	adc	Word Ptr [SpeedAdd],ax
	jc	JumpPattern
	cmp	[Counter],4
	je	ClearNotes
	cmp	[Counter],5
	je	SetNotes
	jmp	EndPattern

ClearNotes:
	mov	di,offset [SampChans]
	mov	cx,[NumChans]
	mov	dx,[BasePort]
	add	dx,CommandPort

StopVoices:
	dec	dx
	mov	al,Byte Ptr [NumChans]
	sub	al,cl
	out	dx,al
	inc	dx

	cmp	[di.InstSet],2
	je	VoiceOff1
	cmp	[di.InstSet],1
	je	VoiceOff2
	cmp	[di.OffsetAdd],0
	jne	VoiceOff1
	jmp	NoVoiceOff

VoiceOff2:
	mov	ax,[di.CurrSamp]
	cmp	ax,[di.OldSamp]
	jne	VoiceOff1
	jmp	NoVoiceOff

VoiceOff1:
	mov	ax,[di.OldVol]
	mov	[di.OldVol],0
	mov	bx,[MainVolume]
	mul	bx
	shr	ax,6
	adc	ax,0
	mov	bx,ax
	shl	bx,1
	mov	bx,[GusVol+bx]

	mov	dx,[BasePort]
	add	dx,CommandPort

	mov	al,VolumeCtrl
	out	dx,al
	add	dx,2
	mov	al,3
	out	dx,al
	sub	dx,2

	mov	bp,[GusVol]
	mov	ah,0
	cmp	bx,bp
	jb	NoFixVolDir1
	mov	ah,01000000b
	xchg	bx,bp
NoFixVolDir1:

	mov	al,VolRampStart
	out	dx,al
	xchg	ax,bx
	inc	dx
	out	dx,ax
	dec	dx

	mov	al,VolRampEnd
	out	dx,al
	mov	ax,bp
	inc	dx
	out	dx,ax
	dec	dx

	mov	al,VolumeCtrl
	out	dx,al
	mov	al,bh
	add	dx,2
	out	dx,al
	sub	dx,2
NoVoiceOff:

	add	di,ChanSize
	dec	cx
	jnz	StopVoices
	jmp	EndPattern

SetNotes:
	mov	[Counter],1
	mov	di,offset [SampChans]
	mov	cx,[NumChans]
	mov	dx,[BasePort]
	add	dx,CommandPort

ChangeSamps:
	dec	dx
	mov	al,Byte Ptr [NumChans]
	sub	al,cl
	out	dx,al
	inc	dx

	cmp	[di.InstSet],2
	je	SampChange1
	cmp	[di.InstSet],1
	je	SampChange2
	cmp	[di.OffsetAdd],0
	jne	ChangeOffset
	jmp	NoChangeSamp

ChangeOffset:
	mov	al,SampleStartLo
	out	dx,al
	inc	dx
	mov	ax,Word Ptr [di.SampOff]
	mov	bp,Word Ptr [di.SampOff+2]
	add	ax,[di.OffsetAdd]
	adc	bp,0
	shr	ax,7
	shl	bp,9
	or	ax,bp
	out	dx,ax
	dec	dx
	mov	al,SampleStartHi
	out	dx,al
	inc	dx
	mov	ax,Word Ptr [di.SampOff]
	add	ax,[di.OffsetAdd]
	shl	ax,9
	out	dx,ax
	dec	dx
	sub	dx,CommandPort
	Rept	6
	in	al,dx
	EndM
	add	dx,CommandPort
	jmp	NoChangeSamp

SampChange1:
	mov	al,WriteVoiceMode
	out	dx,al
	add	dx,2
	mov	al,3
	out	dx,al
	sub	dx,DataHighPort
	Rept	6
	in	al,dx
	EndM
	add	dx,CommandPort

	mov	al,SampleStartLo
	out	dx,al
	inc	dx
	mov	ax,Word Ptr [di.SampOff]
	mov	bp,Word Ptr [di.SampOff+2]
	add	ax,[di.OffsetAdd]
	adc	bp,0
	shr	ax,7
	shl	bp,9
	or	ax,bp
	out	dx,ax
	dec	dx
	mov	al,SampleStartHi
	out	dx,al
	inc	dx
	mov	ax,Word Ptr [di.SampOff]
	add	ax,[di.OffsetAdd]
	shl	ax,9
	out	dx,ax
	sub	dx,DataLowPort
	Rept	6
	in	al,dx
	EndM
	add	dx,CommandPort

	mov	ax,[di.OldSamp]
	cmp	ax,[di.CurrSamp]
	je	NoChangeSamp

	mov	al,SampleEndLo
	out	dx,al
	inc	dx
	mov	ax,Word Ptr [di.SampOff]
	mov	bp,Word Ptr [di.SampOff+2]
	add	ax,[di.Len]
	adc	bp,0
	shr	ax,7
	shl	bp,9
	or	ax,bp
	out	dx,ax
	dec	dx
	mov	al,SampleEndHi
	out	dx,al
	inc	dx
	mov	ax,Word Ptr [di.SampOff]
	add	ax,[di.Len]
	shl	ax,9
	out	dx,ax
	dec	dx

	mov	al,LoopStartLo
	out	dx,al
	inc	dx
	mov	ax,Word Ptr [di.SampOff]
	mov	bp,Word Ptr [di.SampOff+2]
	add	ax,[di.Repeat]
	adc	bp,0
	shr	ax,7
	shl	bp,9
	or	ax,bp
	out	dx,ax
	dec	dx
	mov	al,LoopStartHi
	out	dx,al
	inc	dx
	mov	ax,Word Ptr [di.SampOff]
	add	ax,[di.Repeat]
	shl	ax,9
	out	dx,ax
	dec	dx

	mov	al,WriteVoiceMode
	out	dx,al
	add	dx,2
	mov	al,3
	out	dx,al
	sub	dx,2
	jmp	NoChangeSamp

SampChange2:
	mov	ax,[di.OldSamp]
	cmp	ax,[di.CurrSamp]
	je	NoChangeSamp

	mov	al,VoicePosLo
	out	dx,al
	inc	dx
	in	ax,dx
	mov	bx,ax
	dec	dx
	mov	al,VoicePosHi
	out	dx,al
	inc	dx
	in	ax,dx
	dec	dx
	xchg	ax,bx
	shl	ax,7
	shr	bx,9
	and	bx,7Fh
	or	ax,bx
	mov	bx,[di.OldSamp]
	dec	bx
	shl	bx,2
	sub	ax,Word Ptr [SampOffset+bx]
	mov	bx,ax
	cmp	bx,[di.Len]
	jb	NoFixStart
	mov	bx,[di.Len]
	dec	bx
NoFixStart:

	cmp	[di.OffsetAdd],0
	jne	NoOffsetEff
	mov	[di.OffsetAdd],bx
NoOffsetEff:

	mov	al,SampleStartLo
	out	dx,al
	inc	dx
	mov	ax,Word Ptr [di.SampOff]
	mov	bp,Word Ptr [di.SampOff+2]
	add	ax,[di.OffsetAdd]
	adc	bp,0
	shr	ax,7
	shl	bp,9
	or	ax,bp
	out	dx,ax
	dec	dx
	mov	al,SampleStartHi
	out	dx,al
	inc	dx
	mov	ax,Word Ptr [di.SampOff]
	add	ax,[di.OffsetAdd]
	shl	ax,9
	out	dx,ax
	sub	dx,DataLowPort
	Rept	6
	in	al,dx
	EndM
	add	dx,CommandPort

	mov	al,SampleEndLo
	out	dx,al
	inc	dx
	mov	ax,Word Ptr [di.SampOff]
	mov	bp,Word Ptr [di.SampOff+2]
	add	ax,[di.Len]
	adc	bp,0
	shr	ax,7
	shl	bp,9
	or	ax,bp
	out	dx,ax
	dec	dx
	mov	al,SampleEndHi
	out	dx,al
	inc	dx
	mov	ax,Word Ptr [di.SampOff]
	add	ax,[di.Len]
	shl	ax,9
	out	dx,ax
	dec	dx

	mov	al,LoopStartLo
	out	dx,al
	inc	dx
	mov	ax,Word Ptr [di.SampOff]
	mov	bp,Word Ptr [di.SampOff+2]
	add	ax,[di.Repeat]
	adc	bp,0
	shr	ax,7
	shl	bp,9
	or	ax,bp
	out	dx,ax
	dec	dx
	mov	al,LoopStartHi
	out	dx,al
	inc	dx
	mov	ax,Word Ptr [di.SampOff]
	add	ax,[di.Repeat]
	shl	ax,9
	out	dx,ax
	dec	dx

	mov	al,WriteVoiceMode
	out	dx,al
	add	dx,2
	mov	al,Byte Ptr [di.LoopOnOff]
	out	dx,al
	sub	dx,DataHighPort
	Rept	6
	in	al,dx
	EndM
	add	dx,CommandPort
NoChangeSamp:

	mov	ax,[di.Vol]
	cmp	ax,[di.OldVol]
	je	NoSetBar
	mov	[di.Bar],ax
NoSetBar:

	mov	[TempVol],ax
	mov	ax,[Counter]
	test	[ChanOn],ax
	jnz	NoChanOff
	mov	[TempVol],0
	mov	[di.Bar],0
NoChanOff:

	mov	ax,[di.OldVol]
	mov	bx,[MainVolume]
	mul	bx
	shr	ax,6
	adc	ax,0
	mov	bx,ax
	shl	bx,1
	mov	bx,[GusVol+bx]
	mov	ax,[TempVol]
	mov	bp,[MainVolume]
	mul	bp
	shr	ax,6
	adc	ax,0
	mov	bp,ax
	shl	bp,1
	mov	bp,[GusVol+bp]

	mov	dx,bp
	mov	dl,bh
	cmp	dl,dh
	jne	SetVol

	mov	dx,[BasePort]
	add	dx,CommandPort

	mov	al,VolumeCtrl
	out	dx,al
	add	dx,2
	mov	al,3
	out	dx,al
	sub	dx,2

	mov	al,SetVolume
	out	dx,al
	mov	ax,bp
	inc	dx
	out	dx,ax
	dec	dx
	jmp	NoSetVol

SetVol:
	mov	dx,[BasePort]
	add	dx,CommandPort

	mov	al,VolumeCtrl
	out	dx,al
	add	dx,2
	mov	al,3
	out	dx,al
	sub	dx,2

	mov	ah,0
	cmp	bx,bp
	jb	NoFixVolDir
	mov	ah,01000000b
	xchg	bx,bp
NoFixVolDir:

	mov	dx,[BasePort]
	add	dx,CommandPort
	mov	al,VolRampStart
	out	dx,al
	xchg	ax,bx
	inc	dx
	out	dx,ax
	dec	dx

	mov	al,VolRampEnd
	out	dx,al
	mov	ax,bp
	inc	dx
	out	dx,ax
	dec	dx

	mov	al,VolumeCtrl
	out	dx,al
	mov	al,bh
	add	dx,2
	out	dx,al
	sub	dx,2
NoSetVol:

	mov	al,SetVoiceFreq
	out	dx,al
	inc	dx
	mov	ax,[di.FreqVal]
	out	dx,ax
	dec	dx

	shl	[Counter],1
	add	di,ChanSize
	dec	cx
	jnz	ChangeSamps

	mov	di,offset [SampChans]
	mov	cx,[NumChans]

StartVoices:
	dec	dx
	mov	al,Byte Ptr [NumChans]
	sub	al,cl
	out	dx,al
	inc	dx

	cmp	[di.InstSet],2
	jne	NoVoiceStart

	mov	al,WriteVoiceMode
	out	dx,al
	add	dx,2
	mov	al,Byte Ptr [di.LoopOnOff]
	out	dx,al
	sub	dx,DataHighPort
	Rept	6
	in	al,dx
	EndM
	add	dx,CommandPort
NoVoiceStart:

	mov	[di.InstSet],0
	mov	[di.OffsetAdd],0

	add	di,ChanSize
	dec	cx
	jnz	StartVoices
	jmp	EndPattern

JumpPattern:

	mov	[Counter],0

	dec	Word Ptr [PatternCount]
	jnz	DoEffects
	jmp	DoPattern
DoEffects:

	mov	di,offset [SampChans]
	mov	cx,[NumChans]

EffectLoop:

	mov	ax,[di.Vol]
	mov	[di.OldVol],ax

	inc	[di.EffectTime]
	add	[di.ArpCounter],2
	cmp	[di.ArpCounter],6
	jb	NoWrapArp
	mov	[di.ArpCounter],0
NoWrapArp:
	mov	ax,[di.Effect]
	cmp	ax,0
	jne	DoEffect
	jmp	NoEffect
DoEffect:
	mov	bl,al
	mov	bh,0
	cmp	bl,0Eh
	je	DoEEffects
	shl	bx,1
	jmp	[EffectJumps+bx]
DoEEffects:
	mov	bl,ah
	and	ah,0Fh
	shr	bl,4
	shl	bx,1
	jmp	[EEffectJumps+bx]
;	effects here
NoEffect:

	add	di,ChanSize
	dec	cx
	jz	EndEffects
	jmp	EffectLoop
EndEffects:
	jmp	EndPattern

DoPattern:
	mov	ax,[PatternSpeed]
	mov	[PatternCount],ax
	cmp	[PatternRow],64
	jb	NoPatternWrap
	mov	ax,[PlayingPattern]
	cmp	al,[OrderLen]
	jb	NoTrackWrap
	mov	ax,0
	cmp	[Restart],78h
	jae	NoRestart
	mov	al,[Restart]
NoRestart:
	mov	[PlayingPattern],ax
NoTrackWrap:
; mine
	mov	bx, [PlayingPattern]
	mov	bl, [PatternOrder+bx]
	xor	bh, bh
	inc	[played_pattern+bx]
; mine
	mov	bx,[PlayingPattern]
	mov	[RealPPattern],bx
	inc	[RealPPattern]
	mov	ah,0
	mov	al,[PatternOrder+bx]
	mov	dx,[NumChans]
	shl	dx,8
	mul	dx
	shl	dx,12
	add	dx,[PatternMem]
	mov	[SegPattern],dx
	mov	[BytePattern],ax
	mov	ax,[BreakData]
	mov	[PatternRow],ax
	shl	ax,4
	add	[BytePattern],ax
	mov	[BreakData],0
	inc	[PlayingPattern]
NoPatternWrap:

	les	si,[PatternPek]
	mov	di,offset [SampChans]
	mov	cx,[NumChans]
	mov	ax,[PatternRow]
	mov	[RealRow],ax

PattLoop:

	mov	ax,[di.Vol]
	mov	[di.OldVol],ax

	mov	bh,es:[si]		; New Sample
	mov	bl,es:[si+2]
	and	bh,0F0h
	shr	bl,4
	add	bl,bh
	je	NoNewSample
	mov	bh,Byte Ptr [di.CurrSamp]
	mov	Byte Ptr [di.OldSamp],bh
	mov	Byte Ptr [di.CurrSamp],bl
	mov	bh,0
	dec	bx
	shl	bx,1
	mov	ax,[SampVol+bx]
	cmp	ax,40h
	jb	NoHighVol
	mov	ax,3Fh
NoHighVol:
	mov	[di.Vol],ax
	mov	[di.InstSet],1
	shl	bx,1
	mov	ax,Word Ptr [SampOffset+bx]
	mov	Word Ptr [di.SampOff],ax
	mov	ax,Word Ptr [SampOffset+bx+2]
	mov	Word Ptr [di.SampOff+2],ax
	shr	bx,1
	mov	ax,[SampFine+bx]
	shl	ax,3
	mov	[di.Fine],ax
	shl	ax,3
	add	[di.Fine],ax
	mov	[di.LoopOnOff],0
	mov	ax,[SampLen+bx]
	mov	[di.Len],ax
	mov	ax,[SampRep+bx]
	mov	[di.Repeat],ax
	mov	ax,[SampRepLen+bx]
	mov	[di.RepLen],ax
	cmp	ax,2
	jbe	NoNewSample
	mov	[di.LoopOnOff],8
	mov	ax,[di.Repeat]
	add	ax,[di.RepLen]
	cmp	ax,[di.Len]
	ja	NoNewSample
	mov	[di.Len],ax
NoNewSample:

	mov	bx,es:[si]
	xchg	bh,bl
	and	bx,0FFFh
	je	NoNewNote
	
	mov	bl,[ArpTable+bx]
	mov	bh,0
	add	bx,[di.Fine]
	mov	bx,[AmigaVals+bx]
	mov	[di.PortTo],bx
	mov	ax,es:[si+2]
	and	al,0Fh
	and	ah,0F0h
;	cmp	al,03h
;	je	PortToP
;	cmp	al,05h
;	je	PortToP
	jmp	NoPortToP
PortToP:
	push	cx
	neg	cx
	add	cx,[NumChans]
	mov	ax,1
	shl	ax,cl
	test	ax,[ChanOn]
	je	NoSetPortBar
	mov	ax,[di.Vol]
	mov	[di.Bar],ax
NoSetPortBar:
	pop	cx
	jmp	NoNewNote
NoPortToP:
	cmp	ax,0D00Eh
	jne	NewNote
	mov	al,es:[si+2]
	cmp	al,0
	je	NewNote
	mov	[di.InstSet],0
	jmp	NoNewNote
NewNote:
	mov	[di.InstSet],2
	mov	[di.VibratoPek],0
	mov	[di.Amiga],bx
	shl	bx,1
	mov	bx,[FreqTable+bx]
	mov	[di.FreqVal],bx
NoNewNote:

	mov	[di.ArpCounter],0
	mov	ax,es:[si+2]
	and	al,0Fh
	mov	[di.Effect],ax
	cmp	al,7
	jb	NoSetOldFreq
	mov	bx,ax
	and	bh,0F0h
	cmp	bx,0C00Eh
	je	NoSetOldFreq
	cmp	bx,0D00Eh
	je	NoSetOldFreq
	mov	bx,[di.Amiga]
	shl	bx,1
	mov	bx,[FreqTable+bx]
	mov	[di.FreqVal],bx
NoSetOldFreq:
	mov	bh,0
	mov	bl,al
	cmp	bl,0Eh
	je	DoEPattEffects
	shl	bx,1
	jmp	[PattJumps+bx]
DoEPattEffects:
	mov	bl,ah
	and	ah,0Fh
	shr	bl,4
	shl	bx,1
	jmp	[EPattJumps+bx]

assume	ds:nothing

_store_command	proc
	mov	bx, PatternSpeed
	cmp	PatternCount, bx
	jnz	@@ControlEffect1j
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
	mov	ds:[control_commands+bx], ah
@@nostore1:
	inc	ds:[control_commands_count]
	jmp	@@ControlEffect1SN2
@@ControlEffect1SN:
	cmp	ds:[control_commands_count_new], 12
	jz	@@nostore2
	mov	bx, ds:[control_commands_count_new]
	mov	ds:[control_commands_new+bx], ah
@@nostore2:
	inc	ds:[control_commands_count_new]
@@ControlEffect1SN2:
	pop	ds
@@ControlEffect1j:
	ret
_store_command	endp

store_command_set1	proc
	push	bx
	mov	bx, [RealPPattern]
	dec	bx
	mov	bl, [PatternOrder+bx]
	xor	bh, bh
	cmp	[played_pattern+bx], 0
	pop	bx
	jnz	@@e
	call	_store_command
@@e:
	ret
store_command_set1	endp

store_command_set2	proc
	push	bx
	mov	bx, [RealPPattern]
	dec	bx
	mov	bl, [PatternOrder+bx]
	xor	bh, bh
	cmp	[played_pattern+bx], 1
	pop	bx
	jnz	@@e
	call	_store_command
@@e:
	ret
store_command_set2	endp

ControlEffect_s1	proc
	call	store_command_set1
	jmp	NoPattEffect
ControlEffect_s1	endp

ControlEffect2_s1	proc
	call	store_command_set1
	jmp	NoEffect
ControlEffect2_s1	endp

ControlEffect_s2	proc
	call	store_command_set2
	jmp	NoPattEffect
ControlEffect_s2	endp

ControlEffect2_s2	proc
	call	store_command_set2
	jmp	NoEffect
ControlEffect2_s2	endp

TunnelMove	proc
	or	ah, ah
	jz	@@e
	push	ds
	mov	ax, seg code
	mov	ds, ax
	inc	ds:[tunnel_pos]
;	inc	ds:[tunnel_pos]
	pop	ds
@@e:
	jmp	NoPattEffect
	ret
TunnelMove	endp

TunnelMove2	proc
	or	ah, ah
	jz	@@e
	push	ds
	mov	ax, seg code
	mov	ds, ax
;	inc	ds:[tunnel_pos]
	inc	ds:[tunnel_pos]
	pop	ds
@@e:
	jmp	NoEffect
	ret
TunnelMove2	endp

ControlEffect_g	proc
	call	_store_command
	jmp	NoPattEffect
ControlEffect_g	endp

ControlEffect2_g	proc
	call	_store_command
	jmp	NoEffect
ControlEffect2_g	endp

PosJump:
	mov	[PatternRow],63
	mov	Byte Ptr [PlayingPattern],ah
	jmp	NoPattEffect
Volume:
	cmp	ah,40h
	jb	NoFixVol1
	mov	ah,3Fh
NoFixVol1:
	mov	Byte Ptr [di.Vol],ah
	jmp	NoPattEffect
BreakPatt:
	mov	[PatternRow],63
	cmp	ah,64h
	jb	NoFixBreak
	mov	ah,63h
NoFixBreak:
	mov	al,ah
	and	al,0Fh
	shr	ah,4
	shl	ah,1
	mov	Byte Ptr [BreakData],al
	add	Byte Ptr [BreakData],ah
	shl	ah,2
	add	Byte Ptr [BreakData],ah
	jmp	NoPattEffect
SpeedSet:
	cmp	ah,0
	je	NoPattEffect
	cmp	ah,1Fh
	jbe	UsualSpeed

	xchg	ah,al
	mov	ah,0
	shl	ax,1			;
	mov	bl,5			; Denna bit „r f”r att st„lla
	div	bl			; Effects till annat „n 50Hz
	mov	dl,al			; Detta ger automatiskt en annan
	mov	dh,0			; patternspeed.; Hz=2*BPM/5
	mov	ax,0
	mov	bx,1000
	div	bx
	mov	Word Ptr cs:[Speed],ax
	mov	ax,0
	div	bx
	mov	Word Ptr cs:[Speed+2],ax
	jmp	NoPattEffect
UsualSpeed:
	mov	Byte Ptr [PatternSpeed],ah
	mov	Byte Ptr [PatternCount],ah
	jmp	NoPattEffect
FinePortUp:
	xchg	ah,al
	mov	ah,0
	sub	[di.Amiga],ax
	mov	bx,[di.Fine]
	mov	bx,[AmigaVals+35*2+bx]
	cmp	[di.Amiga],bx
	jae	NoFixFineUp
	mov	[di.Amiga],bx
NoFixFineUp:
	jmp	NoPattEffect
FinePortDown:
	xchg	ah,al
	mov	ah,0
	add	[di.Amiga],ax
	mov	bx,[di.Fine]
	mov	bx,[AmigaVals+bx]
	cmp	[di.Amiga],bx
	jbe	NoFixFineDown
	mov	[di.Amiga],bx
NoFixFineDown:
	jmp	NoPattEffect

NoPattEffect:

	add	si,4
	jnc	NoChangeSeg1
	mov	dx,es
	add	dx,1000h
	mov	es,dx
NoChangeSeg1:
	mov	[di.EffectTime],0
	add	di,ChanSize
	dec	cx
	jz	EndChannels
	jmp	PattLoop
EndChannels:

	inc	[PatternRow]
	mov	ax,[NumChans]
	shl	ax,2
	add	[BytePattern],ax
	jnc	EndPattern
	add	[SegPattern],1000h
EndPattern:

	dec	[JumpOldTimer]
	jnz	NoJumpOld
	mov	[JumpOldTimer],55
	pop	es
	pop	ds
	popa
	jmp	DWord Ptr cs:[OldTimer]
NoJumpOld:
	pop	es
	pop	ds
	popa
	iret

Pattern		EndP

PattJumps	dw offset cs:[NoPattEffect]	; 0
		dw offset cs:[NoPattEffect]	; 1
		dw offset cs:[NoPattEffect]	; 2
		dw offset cs:[ControlEffect_g]	; 3
		dw offset cs:[ControlEffect_s2]; 4
		dw offset cs:[NoPattEffect]	; 5
		dw offset cs:[NoPattEffect]	; 6
		dw offset cs:[NoPattEffect]	; 7
		dw offset cs:[ControlEffect_s1]	; 8
		dw offset cs:[NoPattEffect]	; 9
		dw offset cs:[NoPattEffect]	; A
		dw offset cs:[TunnelMove]	; B
		dw offset cs:[Volume]		; C
		dw offset cs:[BreakPatt]	; D
		dw offset cs:[NoPattEffect]	; E
		dw offset cs:[SpeedSet]		; F

EPattJumps	dw offset cs:[NoPattEffect]	; 0
		dw offset cs:[FinePortUp]	; 1
		dw offset cs:[FinePortDown]	; 2
		dw offset cs:[NoPattEffect]	; 3
		dw offset cs:[NoPattEffect]	; 4
		dw offset cs:[NoPattEffect]	; 5
		dw offset cs:[NoPattEffect]	; 6
		dw offset cs:[NoPattEffect]	; 7
		dw offset cs:[NoPattEffect]	; 8
		dw offset cs:[NoPattEffect]	; 9
		dw offset cs:[NoPattEffect]	; A
		dw offset cs:[TunnelMove]	; B
		dw offset cs:[NoPattEffect]	; C
		dw offset cs:[NoPattEffect]	; D
		dw offset cs:[NoPattEffect]	; E
		dw offset cs:[NoPattEffect]	; F

EffectJumps	dw offset cs:[NoEffect]	; 0
		dw offset cs:[NoEffect]		; 1
		dw offset cs:[NoEffect]		; 2
		dw offset cs:[ControlEffect2_g]	; 3
		dw offset cs:[ControlEffect2_s2]; 4
		dw offset cs:[NoEffect]		; 5
		dw offset cs:[NoEffect]		; 6
		dw offset cs:[NoEffect]		; 7
		dw offset cs:[ControlEffect2_s1]; 8
		dw offset cs:[NoEffect]		; 9
		dw offset cs:[NoEffect]		; A
		dw offset cs:[TunnelMove2]	; B
		dw offset cs:[NoEffect]		; C
		dw offset cs:[NoEffect]		; D
		dw offset cs:[NoEffect]		; E
		dw offset cs:[NoEffect]		; F

EEffectJumps	dw offset cs:[NoEffect]		; 0
		dw offset cs:[NoEffect]		; 1
		dw offset cs:[NoEffect]		; 2
		dw offset cs:[NoEffect]		; 3
		dw offset cs:[NoEffect]		; 4
		dw offset cs:[NoEffect]		; 5
		dw offset cs:[NoEffect]		; 6
		dw offset cs:[NoEffect]		; 7
		dw offset cs:[NoEffect]		; 8
		dw offset cs:[NoEffect]		; 9
		dw offset cs:[NoEffect]		; A
		dw offset cs:[NoEffect]		; B
		dw offset cs:[NoEffect]		; C
		dw offset cs:[NoEffect]		; D
		dw offset cs:[NoEffect]		; E
		dw offset cs:[NoEffect]		; F

	assume	ds:nothing

StartPlaying	Proc	Far

	pusha
	push	ds
	push	es

	cli

	mov	[JumpOldTimer],55
	mov	di,offset cs:[SampChans]
	mov	cx,cs:[NumChans]
ClearVarLoop:
	mov	cs:[di.FreqVal],0
	mov	cs:[di.Vol],0
	add	di,ChanSize
	loop	ClearVarLoop

	mov	ax,0
	mov	es,ax

	mov	bx,es:[20h]
	mov	ax,es:[22h]
	mov	Word Ptr cs:[OldTimer],bx
	mov	Word Ptr cs:[OldTimer+2],ax

	push	cs
	pop	ds
	mov	dx,offset cs:[Pattern]

	mov	es:[20h],dx
	mov	es:[22h],ds

	mov	al,36h
	out	43h,al
	mov	al,169
	out	40h,al
	mov	al,4
	out	40h,al

	mov	ax,0
	mov	dx,50
	mov	bx,1000
	div	bx
	mov	Word Ptr cs:[Speed],ax
	mov	ax,0
	div	bx
	mov	Word Ptr cs:[Speed+2],ax
	mov	Word Ptr cs:[SpeedAdd],0
	mov	Word Ptr cs:[SpeedAdd+2],0
	mov	[Counter],20
	mov	Word Ptr [Time],0
	mov	Word Ptr [Time+2],0

	mov	dx,cs:[BasePort]	; Speaker On!!!
	mov	al,00000001b
	out	dx,al

	sti

	pop	es
	pop	ds
	popa
	ret

StartPlaying	EndP

StopPlaying	Proc	Far

	pusha
	push	ds
	push	es

	cli

	mov	dx,cs:[BasePort]	; Speaker Off!!!
	mov	al,00000011b
	out	dx,al

	mov	ax,0
	mov	es,ax

	mov	al,36h
	out	43h,al
	mov	al,0
	out	40h,al
	mov	al,0
	out	40h,al

	mov	dx,Word Ptr cs:[OldTimer]
	mov	ds,Word Ptr cs:[OldTimer+2]
	mov	es:[20h],dx
	mov	es:[22h],ds
	sti

	mov	dx,cs:[BasePort]
	add	dx,ActiveVoicePort
	mov	cx,NumVoices
VoiceClearL:
	mov	al,cl
	dec	al
	out	dx,al
	inc	dx
	mov	al,0
	out	dx,al
	add	dx,2
	mov	al,3	; Voice Off
	out	dx,al
	sub	dx,2
	mov	al,0Dh
	out	dx,al
	add	dx,2
	mov	al,3	; Ramp Off
	out	dx,al
	sub	dx,3
	loop	VoiceClearL

	pop	es
	pop	ds
	popa
	ret

StopPlaying	EndP

Init		Proc	Far

	pusha
	push	ds
	push	es
	mov	cs:[PatternRow],64
	mov	cs:[PlayingPattern],0
	mov	cs:[PatternSpeed],6
	mov	cs:[PatternCount],1
	pop	es
	pop	ds
	popa
	ret

Init		EndP

InitDevice	Proc	Far

	pusha
	push	es
	push	ds

	mov	si,108*2
	mov	bp,108
	mov	cx,908-108+1
CountLoop:
	mov	dx,36h
	mov	ax,9DE4h
	div	bp
	mov	dx,100
	mul	dx
	mov	bx,Voices20
	div	bx
	shr	bx,1
	adc	bx,0
	cmp	dx,bx
	jb	NoHigherFreq
	inc	ax
NoHigherFreq:
	mov	Word Ptr cs:[FreqTable+si],ax
	inc	bp
	add	si,2
	loop	CountLoop

;	Init the UltraSound

	mov	bx,cs:[BasePort]
	mov	cx,bx
	add	bx,CommandPort
	add	cx,DataHighPort
	mov	dx,bx
	mov	al,Initialize
	out	dx,al
	mov	dx,cx
	mov	al,0
	out	dx,al
	mov	dx,cs:[BasePort]
	Rept	6
	in	al,dx
	EndM
	mov	dx,bx
	mov	al,4Ch
	out	dx,al
	mov	dx,cx
	mov	al,1
	out	dx,al
	mov	dx,cs:[BasePort]
	Rept	6
	in	al,dx
	EndM

	mov	dx,bx
	mov	al,DMACtrl
	out	dx,al
	mov	dx,cx
	mov	al,0
	out	dx,al
	mov	dx,bx
	mov	al,45h
	out	dx,al
	mov	dx,cx
	mov	al,0
	out	dx,al
	mov	dx,bx
	mov	al,49h
	out	dx,al
	mov	dx,cx
	mov	al,0
	out	dx,al

	mov	dx,bx
	mov	al,VoicesActive
	out	dx,al
	mov	dx,cx
	mov	al,NumVoices
	dec	al
	or	al,0C0h
	out	dx,al

	mov	dx,cs:[BasePort]
	add	dx,StatusPort
	in	al,dx
	mov	dx,bx
	mov	al,DMACtrl
	out	dx,al
	mov	dx,cx
	in	al,dx
	mov	dx,bx
	mov	al,49h
	out	dx,al
	mov	dx,cx
	in	al,dx
	mov	dx,bx
	mov	al,8Fh
	out	dx,al
	mov	dx,cx
	in	al,dx

	push	cx
	mov	cx,32
VoiceClearLoop:
	mov	dx,cs:[BasePort]
	add	dx,ActiveVoicePort
	mov	al,cl
	dec	al
	out	dx,al
	inc	dx
	mov	al,0
	out	dx,al
	add	dx,2
	mov	al,3	; Voice Off
	out	dx,al
	sub	dx,2
	mov	al,0Dh
	out	dx,al
	add	dx,2
	mov	al,3	; Ramp Off
	out	dx,al
	loop	VoiceClearLoop
	pop	cx

	mov	dx,bx
	mov	al,DMACtrl
	out	dx,al
	mov	dx,cx
	in	al,dx
	mov	dx,bx
	mov	al,49h
	out	dx,al
	mov	dx,cx
	in	al,dx
	mov	dx,bx
	mov	al,8Fh
	out	dx,al
	mov	dx,cx
	in	al,dx

	mov	dx,bx
	mov	al,Initialize
	out	dx,al
	mov	dx,cx
	mov	al,7
	out	dx,al

	mov	cx,NumVoices
SetRampRateLoop:
	mov	dx,cs:[BasePort]
	add	dx,ActiveVoicePort
	mov	al,NumVoices
	sub	al,cl
	out	dx,al

	mov	dx,cs:[BasePort]
	add	dx,CommandPort
	mov	al,VolRampRate
	out	dx,al
	mov	al,00111111b
	mov	dx,cs:[BasePort]
	add	dx,DataHighPort
	out	dx,al

	mov	dx,cs:[BasePort]
	add	dx,CommandPort
	mov	al,SetVolume
	out	dx,al
	mov	ax,cs:[GusVol]
	mov	dx,cs:[BasePort]
	add	dx,DataLowPort
	out	dx,ax
	loop	SetRampRateLoop

;	Finished Initialize (Pheewww!!!!!!!!!!!!!)

	pop	es
	pop	ds
	popa
	ret

InitDevice	EndP

ClearMem	Proc	Far

;	pusha
;	push	es
;	mov	es,cs:[PatternMem]
;	call	DeAlloc
;	pop	es
;	popa
	ret

ClearMem	EndP

LoadMod		Proc	Far
; Error if CF set! ax=errorcode.
; ax=1, File or path not found.
; ax=2, Error loading file.
; ax=3, Out of memory.

	call	ModLoad
	ret

	call	FileOpen
	jnc	NoOpenError
	mov	ax,[ErrorCode]
	jmp	NoMod
NoOpenError:
	call	ModLoad
	jnc	NoLoadError
	mov	ax,[ErrorCode]
	jmp	NoMod
NoLoadError:
	call	FileClose
NoMod:
	ret

LoadMod		EndP

ModLoad		Proc	Near

	pusha
	push	ds
	push	es

	mov	cs:[MainVolume],64
	mov	cx,1084
	mov	dx,offset cs:[Info]
	push	cs
	pop	ds
	call	FileRead
	mov	cs:[NumChans],4
	mov	bp,offset cs:[Info+1080]
	mov	bx,0
	mov	cx,4
TestInst1:
	mov	al,cs:[bp]
	cmp	al,cs:[MKSign+bx]
	jne	NoMK1
	inc	bp
	inc	bx
	loop	TestInst1
	mov	si,offset cs:[Info+952]
	mov	cs:[MKMod],1
	jmp	Load
NoMK1:
	mov	bp,offset cs:[Info+1080]
	mov	bx,4
	mov	cx,4
TestInst2:
	mov	al,cs:[bp]
	cmp	al,cs:[MKSign+bx]
	jne	NoMK2
	inc	bp
	inc	bx
	loop	TestInst2
	mov	si,offset cs:[Info+952]
	mov	cs:[MKMod],1
	jmp	Load
NoMK2:
	mov	bp,offset cs:[Info+1080]
	mov	bx,8
	mov	cx,4
TestInst3:
	mov	al,cs:[bp]
	cmp	al,cs:[MKSign+bx]
	jne	No6CHN2
	inc	bp
	inc	bx
	loop	TestInst3
	mov	cs:[MainVolume],48
	mov	cs:[NumChans],6
	mov	si,offset cs:[Info+952]
	mov	cs:[MKMod],1
	jmp	Load
No6CHN2:
	mov	bp,offset cs:[Info+1080]
	mov	bx,12
	mov	cx,4
TestInst4:
	mov	al,cs:[bp]
	cmp	al,cs:[MKSign+bx]
	jne	No8CHN3
	inc	bp
	inc	bx
	loop	TestInst4
	mov	cs:[MainVolume],32
	mov	cs:[NumChans],8
	mov	si,offset cs:[Info+952]
	mov	cs:[MKMod],1
	jmp	Load
No8CHN3:
	mov	bp,offset cs:[Info+1080]
	mov	bx,16
	mov	cx,4
TestInst5:
	mov	al,cs:[bp]
	cmp	al,cs:[MKSign+bx]
	jne	STMod
	inc	bp
	inc	bx
	loop	TestInst5
	mov	cs:[MainVolume],32
	mov	cs:[NumChans],8
	mov	si,offset cs:[Info+952]
	mov	cs:[MKMod],1
	jmp	Load
STMod:
	call	MovePoint
	mov	dx,offset cs:[Info]
	mov	cx,600
	call	FileRead
	mov	si,offset cs:[Info+472]
	mov	cs:[MKMod],0

Load:
	mov	ax,cs:[si-2]
	mov	Word Ptr cs:[OrderLen],ax
	mov	di,offset cs:[PatternOrder]
	mov	cx,128
CopyOrder:
	mov	al,cs:[si]
	mov	cs:[di],al
	inc	si
	inc	di
	loop	CopyOrder
	mov	si,offset cs:[PatternOrder]
	mov	cx,128
	mov	ax,0
CheckPatt:
	mov	al,cs:[si]
	cmp	al,ah
	jb	NoHigh
	xchg	ah,al
NoHigh:
	inc	si
	loop	CheckPatt
	inc	ah
	xor	al,al
	mov	bx,cs:[NumChans]
	mul	bx
	mov	si,dx
	mov	di,ax
	mov	bx,16
	div	bx
	mov	bx,ax
;	call	Alloc
;	jnc	NoOutHunk
;	stc
;	mov	cs:[ErrorCode],3
;	jmp	ErrorLoad		; Fixa n†n slags error-flagga.
;NoOutHunk:
;	mov	cs:[PatternMem],ax
	mov	ax, seg patterns_seg
	mov	cs:[PatternMem], ax

	mov	ds,ax

LoadLoop:
	dec	si
	js	EndBigLoad
	mov	dx,0
	mov	cx,65535
	call	FileRead
	mov	dx,65535
	mov	cx,1
	call	FileRead
	mov	dx,ds
	add	dx,1000h
	mov	ds,dx
	jmp	Short LoadLoop
EndBigLoad:
	mov	cx,di
	mov	dx,0
	call	FileRead

	mov	cx,15
	cmp	cs:[MKMod],1
	jne	NoMK
	mov	cx,31
NoMK:
;	mov	bx,4096
;	call	Alloc
	assume	ds:nothing
;	mov	ax, seg temp_seg1
;	mov	bx, offset temp_seg1
;	shr	bx, 4
;	add	ax, bx
	mov	ax, seg code
	mov	ds, ax
	mov	ax, ds:[_temp_seg1]
	push	cs
	pop	ds
	mov	cs:[SampMem],ax
	mov	Word Ptr cs:[GUSMem],0
	mov	Word Ptr cs:[GUSMem+2],0
	mov	si,offset cs:[Info+42]
	mov	bp,0
LoadIns:
	push	cx
	mov	Word Ptr cs:[SampLen+bp],0
	mov	cx,cs:[si]
	add	si,2
	xchg	ch,cl
	cmp	cx,2
	jbe	NoIns
	shl	cx,1
	mov	cs:[SampLen+bp],cx
	mov	ds,cs:[SampMem]
	mov	dx,0
	call	FileRead

;	Lets move the sample to the UltaraSound!

	push	bp
	mov	cx,cs:[SampLen+bp]
	shl	bp,1
	mov	di,Word Ptr cs:[GUSMem]
	mov	bl,Byte Ptr cs:[GUSMem+2]
	mov	Word Ptr [SampOffset+bp],di
	mov	Byte Ptr [SampOffset+bp+2],bl
	mov	bp,di
	mov	di,0
	mov	dx,cs:[BasePort]
	add	dx,CommandPort
MSamp2GUS:
	mov	al,DRAMAddrLo
	out	dx,al
	inc	dx
	mov	ax,bp
	out	dx,ax
	dec	dx
	mov	al,DRAMAddrHi
	out	dx,al
	add	dx,2
	mov	al,bl
	out	dx,al
	add	dx,2
	mov	al,[di]
	out	dx,al
	sub	dx,4
	inc	di
	add	bp,1
	adc	bl,0
	loop	MSamp2GUS
	dec	di
	mov	al,DRAMAddrLo
	out	dx,al
	inc	dx
	mov	ax,bp
	out	dx,ax
	dec	dx
	mov	al,DRAMAddrHi
	out	dx,al
	add	dx,2
	mov	al,bl
	out	dx,al
	add	dx,2
	mov	al,[di]
	out	dx,al
	sub	dx,4
	add	bp,1
	adc	bl,0
	mov     Word Ptr cs:[GUSMem],bp
	mov	Byte Ptr cs:[GUSMem+2],bl
	pop	bp

NoIns:
	mov	ah,0
	mov	al,cs:[si]
	inc	si
	mov	cs:[SampFine+bp],ax
	mov	al,cs:[si]
	inc	si
	mov	Byte Ptr cs:[SampVol+bp],al

	mov	dx,cs:[SampLen+bp]
	mov	ax,cs:[si]
	add	si,2
	xchg	ah,al
	mov	cx,cs:[si]
	add	si,2
	xchg	ch,cl
	cmp	cs:[MKMod],1
	jne	STMod1
	shl	ax,1
	shl	cx,1
STMod1:
	mov	cs:[SampRepLen+bp],cx
	add	cx,ax
	cmp	cx,dx
	jbe	NoRepFix
	mov	cx,dx
	sub	cx,ax
	mov	cs:[SampRepLen+bp],cx
NoRepFix:
	mov	cs:[SampRep+bp],ax

	add	si,22
	add	bp,2
	pop	cx
	dec	cx
	jz	Loaded
	jmp	LoadIns
Loaded:
;	mov	es,cs:[SampMem]
;	call	DeAlloc
;	mov	cs:[ErrorCode],0
;	clc

;	Set Pan Regs!!

ErrorLoad:
	mov	cx,cs:[NumChans]
	mov	bx,cx
	shr	bx,1
	sub	bx,2
	shl	bx,4
SetPanLoop:
	mov	dx,cs:[BasePort]
	add	dx,ActiveVoicePort
	mov	al,Byte Ptr cs:[NumChans]
	sub	al,cl
	out	dx,al

	mov	dx,cs:[BasePort]
	add	dx,CommandPort
	mov	al,VoiceBalance
	out	dx,al
	mov	dx,cs:[BasePort]
	add	dx,DataHighPort
	mov	al,cs:[PanRegs+bx]
	out	dx,al
	inc	bx
	loop	SetPanLoop

	pop	es
	pop	ds
	popa
	ret

ModLoad		EndP

;Alloc		Proc	Near
;
;	push	bx
;	mov	ax,4800h
;	int	21h
;	pop	bx
;	ret
;
;Alloc		EndP

;DeAlloc		Proc	Near
;
;	push	ax
;	push	es
;	mov	ax,4900h
;	int	21h
;	pop	es
;	pop	ax
;	ret
;
;DeAlloc		EndP

FileRead	Proc	Near
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
	mov	si, cs:[mod_pos]
	mov	di, dx
	add	cs:[mod_pos], cx
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

mod_pos	dw 0

FileRead	EndP

FileOpen	Proc	Near

	push	dx
	push	ds
	mov	ax,3D00h
	int	21h
	mov	cs:[FileHandle],ax
	jnc	NoErrorOpen
	mov	cs:[ErrorCode],1
NoErrorOpen:
	pop	ds
	pop	dx
	ret

FileOpen	EndP

FileClose	Proc	Near

	push	ax
	push	bx
	mov	ax,3E00h
	mov	bx,cs:[FileHandle]
	int	21h
	pop	bx
	pop	ax
	ret

FileClose	EndP

MovePoint	Proc	Near

	mov	cs:[mod_pos], 0
;	push	ax
;	push	bx
;	push	cx
;	push	dx
;	mov	ax,4200h
;	mov	bx,cs:[FileHandle]
;	mov	cx,0
;	mov	dx,0
;	int	21h
;	pop	dx
;	pop	cx
;	pop	bx
;	pop	ax
	ret

MovePoint	EndP

played_pattern	db -1,-1,-1,-1,-1, -1,-1,-1,-1,-1

end_gus_play	label

_Code	EndS

;	End