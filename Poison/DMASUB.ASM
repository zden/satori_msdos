;    GLOBAL  SetSampleRate:Near, StartTransferDMA:Near, TurnOffSpeaker:Near
;    GLOBAL  TurnOnSpeaker:Near, CalcForDMA :Near
;    GLOBAL  HaltDma      :Near, DspReset   :Near  

;    GLOBAL  Status       :WORD, QuitDma      :WORD, BaseAddress :Word
;    GLOBAL  IntNumber    :BYTE, BufferSeg    :Word
;    GLOBAL  Buffer1size  :WORD, Buffer2Size  :WORD
;    GLOBAL  BufferOffset1:WORD, BufferOffset2:WORD

    ;To Initialize:
    ;   DspReset
    ;   TurnOnSpeaker
    ;   SetSampleRate
    ;   CalcForDMA => call whenever song speed changes
    ;
    ;To Start:
    ;   StartTransferDma
    ;
    ;To End:
    ;   mov [QuitDMA],1
    ;   TurnOffSpeaker
    
    BufferSeg       dw  0
    BufferOffset1   dw  0
    BufferOffset2   dw  0
    Buffer1Size     dw  31000
    Buffer2Size     dw  31000

    BaseAddress     dw  220h
    IntNumber       db  7
    Status          dw  0

    QuitDma         dw  0

    MoreB           db  0,0
    IPage           db  0,0,0,0
    ILength         dw  0,0,0,0
    IOffset         dw  0,0,0,0

    OldDmaIntOff    dw  0
    OldDmaIntSeg    dw  0

    RealDmaOff      dw  0
    RealDmaSeg      dw  0

;==================
;==- Interrupts -==
;==================

DMAINT1:
    pusha
    cli
    mov     cs:[status],3
    mov     dx,cs:[BaseAddress]
    add     dx,0eh   
    in      al,dx           ;acknowledge interrupt
    cmp     cs:[QUITDMA],0
    jne     DoneInt7        ;if we are to quit, quit now
    cmp     cs:[MoreB],0
    jne     DoDma2
    jmp     DoDma3

DMAINT2:
    pusha
    cli
    mov     dx,cs:[BaseAddress]
    add     dx,0eh   
    in      al,dx           ;acknowledge interrupt
    cmp     cs:[QUITDMA],0
    jne     DoneInt7        ;if we are to quit, quit now
    jmp     DoDma3

DoneInt7:
    call    haltdma
    ;call    dspreset
    call    undorealint
    jmp     SkipMask

DMAINT3:
    pusha
    cli
    mov     cs:[status],4
    mov     dx,cs:[BaseAddress]
    add     dx,0eh   
    in      al,dx           ;acknowledge interrupt
    cmp     cs:[QUITDMA],0
    jne     DoneInt7        ;if we are to quit, quit now
    cmp     cs:[MoreB],0
    jne     DoDma4
    jmp     DoDma1

DMAINT4:
    pusha
    cli
    mov     dx,cs:[BaseAddress]
    add     dx,0eh   
    in      al,dx           ;acknowledge interrupt
    cmp     cs:[QUITDMA],0
    jne     DoneInt7        ;if we are to quit, quit now
    jmp     DoDma1

DoDma1:
    mov     ah,cs:[IPage]     ;set up for dma 1
    mov     cx,cs:[Ilength]
    mov     dx,cs:[Ioffset]
    mov     bx,offset DMAInt1
    call    TransferDma
    jmp     short skipmask
DoDma2:
    mov     ah,cs:[IPage+1]     ;set up for dma 2
    mov     cx,cs:[ILength+2]
    mov     dx,cs:[IOffset+2]
    mov     bx,offset DMAInt2
    call    TransferDma
    jmp     short skipmask
DoDma3:
    mov     ah,cs:[IPage+2]     ;set up for dma 3
    mov     cx,cs:[ILength+4]
    mov     dx,cs:[IOffset+4]
    mov     bx,offset DMAInt3
    call    TransferDma
    jmp     short skipmask
DoDma4:
    mov     ah,cs:[IPage+3]     ;set up for dma 4
    mov     cx,cs:[ILength+6]
    mov     dx,cs:[IOffset+6]
    mov     bx,offset DMAInt4
    call    TransferDma
    jmp     short skipmask

Skipmask:
    mov     al,20h
    out     20h,al          ;end of hardware interrupt
    popa
    sti
    iret

;======================
;==- Start Transfer -==
;======================

StartTransferDma PROC Near
    pusha

    mov     al,cs:[IntNumber]
    add     al,8
    cbw
    shl     al,2
    mov     bx,ax
    push    es
    sub     ax,ax
    mov     es,ax
    mov     ax,es:[bx]
    mov     cs:[OldDmaIntOff],ax              ; Grab the current interrupt
    mov     ax,es:[bx+2]
    mov     cs:[OldDmaIntSeg],ax              ; so we can restore it later
    pop     es

    mov     ah,cs:[IPage]     ;set up for dma 1
    mov     cx,cs:[Ilength]
    mov     dx,cs:[Ioffset]
    mov     bx,offset DMAInt1
    call    TransferDma               ;GO!
    
    popa
    ret
StartTransferDma ENDP

    ;ah= page number
    ;dx= BASE_ADDRESS
    ;cx= DATA_LENGTH (-1)
    ;bx= address of interrupt
TransferDma proc near
    pusha
    dec     cx
    mov     al,5
    out     0ah,al          ;Mask off channel 1
    xor     al,al
    out     0ch,al          ;Clear byte pointer F/F to lowerbyte
    mov     al,49h  
    out     0bh,al          ;Set transfer mode to DAC (ADC = 45h)_
    mov     al,dl           ;LSB of BASE_ADDRESS
    out     2,al
    mov     al,dh           ;MSB of BASE_ADDRESS
    out     2,al
    mov     al,ah
    out     83h,al          ;Page Number
    mov     al,cl
    out     3,al            ;LSB of DATA_Length
    mov     al,ch
    out     3,al            ;MSB of DATA_Length
    mov     al,1
    out     0ah,al          ;Enable Channel 1
    mov     ax,bx
    call    DoREALint
    mov     al,14h          ;function 14h DMAmode 8-bit DAC
    call    Sendcommand
    mov     al,cl           ;send LSB of DATALENGTH
    call    SendCommand
    mov     al,ch           ;send MSB of DATALENGTH
    call    SendCommand
    popa
    ret
TransferDMA endp

    ;ah= SR (Time Constant = 256 - 1,000,000/HZ)
SetSampleRate proc near
    mov     al,40h
    call    SendCommand
    mov     al,ah
    call    SendCommand
    ret
SetSampleRate endp

TurnOffSpeaker proc near
    mov     al,0d3h         ;turn off speaker
    call    Sendcommand
    ret
endp TurnOffSpeaker

HaltDMA proc near
    mov     al,0d0h         ;Halt DMA
    call    SendCommand
    ret
endp HaltDma

    ;al = command
SendCommand proc near
    push    dx
    push    ax
    mov     dx,cs:[BaseAddress]
    add     dx,0ch
sendcommandloop:
    in      al,dx
    test    al,10000000b
    jnz     sendcommandloop
    pop     ax
    out     dx,al
    pop     dx
    ret
endp SendCommand

TurnOnSpeaker proc near
    mov     al,0d1h
    call    SendCommand
    ret
endp TurnOnSpeaker

    ;input- none 
    ;output al=0 successful
    ;       al=1 unsuccessful
    ;DESTROYED: ax,dx,cx

DspReset proc near
    mov     dx,cs:[baseaddress]
    add     dx,06h
    mov     al,1
    out     dx,al
    mov     cx,100
    push    di
    rep     lodsb   ;wait for at least 3æs
    pop     di
    xor     al,al
    out     dx,al

    add     dx,8         ;check status (22eh)
    mov     cx,120
waitforstat:
    in      al,dx
    dec     cx
    je      errorstat
    test    al,10000000b
    jz      waitforstat

    mov     cx,10000
    sub     dx,4         ;(22ah)      
waitforstat2:
    in      al,dx
    dec     cx
    je      errorstat
    cmp     al,0aah
    jne     waitforstat2
    mov     al,0
    ret
errorstat:
    mov     al,1
    ret
endp DspReset

;========================
;==- Start Interrupts -==
;========================

    ;cs:ax=location of interrupt
DoRealint   proc    near
    pushf                           ; Push flags
    push    bx
    push    cx
    push    dx
    cli                             ; Disable interrupts
    mov     dx,ax
    mov     al,cs:[IntNumber]
    add     al,8
    cbw                             ; Convrt byte to word
    shl     ax,2                    ; Shift w/zeros fill
    mov     bx,ax
    push    es
    sub     ax,ax
    mov     es,ax
    mov     es:[bx],dx
    mov     es:[bx+2],cs
    pop     es
    mov     cl,cs:[IntNumber]
    mov     ah,1
    shl     ah,cl                   ; Shift w/zeros fill
    not     ah
    in      al,21h                  ; port 21h, 8259-1 int IMR
    and     al,ah
    out     21h,al                  ; port 21h, 8259-1 int comands
    pop     dx
    pop     cx
    pop     bx
    popf                            ; Pop flags
    sti
    ret
endp DoRealint
  
UnDoREALint proc    near
    pushf
    pusha
    cli
    mov     al,cs:[IntNumber]
    add     al,8
    cbw
    shl     ax,2
    mov     di,ax
    push    es
    sub     ax,ax
    mov     es,ax
    mov     ax,cs:[OldDmaIntOff]              
    mov     es:[di],ax
    mov     ax,cs:[OldDmaIntSeg]              
    mov     es:[di+2],ax
    pop     es
    mov     cl,cs:[IntNumber]               ; (=7)
    mov     ah,1
    shl     ah,cl                   ; Shift w/zeros fill
    in      al,21h                  ; port 21h, 8259-1 int IMR
    or      al,ah
    out     21h,al                  ; port 21h, 8259-1 int comands
    popa
    popf                            ; Pop flags
    sti
    ret
endp UnDoREALint

    ;takes a Segment:offset pair and a length and returns a page and offset
    ;for and length for both the first half and the second half (if the
    ;   sample crosses a page boundry) MakePage actully does the work
CalcForDMA proc near
    cli
    pusha
    mov     ax,cs
    mov     ds,ax

    mov     [MoreB],0
    mov     ax,[BufferSeg]
    mov     dx,[BufferOffset1]   ;offset for part1
    call    MakePage
    
    cmp     cx,[buffer1size]        ;is max less than needed?
    jb      skiplengthset
    mov     cx,[buffer1size]
    jmp     nomoreb1
skiplengthset:
    mov     bx,[buffer1size]
    sub     bx,cx
    mov     [MoreB],1
    dec     bx
    mov     [ILength+2],bx
    mov     [IOffset+2],0
    mov     [IPage+1],ah
    inc     [IPage+1]
NoMoreb1:
    mov     [IPage],ah
    mov     [IOffset],dx
    dec     cx
    mov     [ILength],cx

    mov     [MoreB+1],0
    mov     ax,[BufferSeg]
    mov     dx,[BufferOffset2]   ;offset for part1
    call    MakePage
    
    cmp     cx,[buffer2size]        ;is max less than needed?
    jb      skiplengthset2
    mov     cx,[buffer2size]
    jmp     nomoreb2
skiplengthset2:
    mov     bx,[buffer2size]
    sub     bx,cx
    mov     [MoreB+1],1
    dec     bx
    mov     [ILength+6],bx
    mov     [IOffset+6],0
    mov     [IPage+3],ah
    inc     [IPage+3]
NoMoreb2:
    mov     [IPage+2],ah
    mov     [IOffset+4],dx
    dec     cx
    mov     [ILength+4],cx
    popa
    sti
    ret
endp CalcForDMA

    ;input: ax=segment
    ;       dx=offset
    ;output: ah=page
    ;       dx=offset
    ;       cx=MAX_LENGTH
MakePage proc near 
    push    ax
    shl     ax,4
    add     dx,ax     
    jnc     Nocarry
    pop     ax
    shr     ah,4
    inc     ah
    mov     cx,dx
    neg     cx
    ret

Nocarry:        
    pop     ax
    shr     ah,4
    mov     cx,dx
    neg     cx
    ret
endp MakePage