drawobject proc
ARG objoff:word, xp:word, yp:word, col:word
        push bp
        mov bp, sp
        pushad

        mov di, objoff
        mov si, [di]
        and si, 00ffh
        inc si
        shl si, 1
        add si, objoff
        mov bx, si
        mov ax, [di+1]
        xor ah, ah
        mov dx, ax
        shl ax, 1
        add ax, dx
        add bx, ax
        mov cx, dx
        add di, 4
@@j1:
;        call drawface c, xp, yp, word ptr [bx]
        call drawface c, xp, yp, col
        add si, 3
        add bx, 1
        dec cx
        jnz @@j1

        popad
        pop bp
        ret
drawobject endp

decide macro
        mov ax, x2
        sub ax, x3
        mov bx, y1
        sub bx, y3
        xor dx, dx
        imul bx
        mov cx, dx
        shl ecx, 16
        mov cx, ax

        mov ax, x1
        sub ax, x3
        mov bx, y2
        sub bx, y3
        xor dx, dx
        imul bx
        shl edx, 16
        mov dx, ax

        stc
        sub cx, dx
        js @@jd1
        clc
@@jd1:
endm

getpt macro p, x, y
        mov bx, p
        xor bh, bh
        dec bx
        shl bx, 1
        xor ax, ax
        mov al, [di+bx]
        add ax, xp
        xor dx, dx
        mov dl, [di+bx+1]
        add dx, yp
        mov x, ax
        mov y, dx
endm

drawface proc
ARG xp:word, yp:word, c:word
LOCAL x1:word, y1:word, x2:word, y2:word, x3:word, y3:word=loc
        push bp
        mov bp, sp
        sub sp, loc
        push ebx ecx edx di si

        getpt [si], x1, y1
        getpt [si+1], x2, y2
        getpt [si+2], x3, y3

        call sort
        decide
        jnc @@t1

        mov di, offset xbl
        call line c, x1, y1, x3, y3
        mov di, offset xbl+400
        call line c, x1, y1, x2, y2
        call line c, x2, y2, x3, y3
        jmp @@t2
@@t1:
        mov di, offset xbl+400
        call line c, x1, y1, x3, y3
        mov di, offset xbl
        call line c, x1, y1, x2, y2
        call line c, x2, y2, x3, y3
@@t2:

        cmp y3, 200
        jl @@f1
        mov y3, 199
@@f1:
        cmp y1, 0
        jge @@f2
        mov y1, 0
@@f2:
        xor si, si
        mov cx, y1
@@j1:
        mov ax, [xbl+si]
        mov bx, [xbl+400+si]
        call shline c, ax, bx, cx, c
        add si, 2
        inc cx
        cmp cx, y3
        jle @@j1

        pop si di edx ecx ebx
        mov sp, bp
        pop bp
        ret
drawface endp

exchange macro a1, a2, b1, b2
        mov bx, a1
        mov cx, b1
        mov b1, bx
        mov a1, cx

        mov bx, a2
        mov cx, b2
        mov b2, bx
        mov a2, cx
endm

sort proc
        mov ax, y1
        cmp ax, y2
        jle @@j1
        exchange x1, y1, x2, y2
@@j1:

        mov ax, y1
        cmp ax, y3
        jle @@j2
        exchange x1, y1, x3, y3
@@j2:
        mov ax, y2
        cmp ax, y3
        jl @@j3
        jg @@j31
        mov ax, x2
        cmp ax, x3
        jle @@j3
@@j31:
        exchange x2, y2, x3, y3
@@j3:

        ret
sort endp

shline proc
ARG x1:word, x2:word, y:word, c:word
        push bp
        mov bp, sp

        push bx cx di

        mov cx, x2
        mov bx, x1
        cmp cx, bx
        jge @@t1
        xchg cx, bx
@@t1:

        sub cx, bx
        inc cx

@@z1:
        mov di, y
        shl di, 2
        add di, y
        shl di, 6
        add di, bx

        mov ax, c
        mov ah, al
        shl eax, 16
        mov ax, c
        mov ah, al

        shr cx, 1
        jnc @@j1
        stosb
@@j1:
        shr cx, 1
        jnc @@j2
        stosw
@@j2:
        rep stosd

        pop di cx bx bp
        ret
shline endp

line proc
ARG x1:word, y1:word, x2:word, y2:word
        push bp
        mov bp, sp

        xor ebx, ebx

        mov bx, y2
        sub bx, y1
        jz @@end

;        inc bx
        mov ax, x2
        sub ax, x1
        js @@o1
;        inc ax
        jmp @@o2
@@o1:
;        dec ax
@@o2:

        sal eax, 16
        mov edx, eax
        sar edx, 16
        sar edx, 16
        idiv ebx
        mov edx, eax

        mov si, x1
        shl esi, 16

        cmp y2, 200
        jl @@f0
        mov y2, 199
@@f0:
        mov cx, y1
@@f1:
        cmp cx, 0
        jge @@j1
        add esi, edx
        inc cx

        jmp @@f1

@@j1:
        cmp esi, 0
        jnl @@t1
        mov esi, 0
@@t1:
        cmp esi, 319*65536
        jng @@l1
        mov esi, 319*65536

        cmp di, offset xbl+400
        jb @@of1
        cmp edx, 65536
        jg @@l2
        jmp @@l1
@@of1:
        cmp edx, -65536
        jl @@l2

@@l1:
        ror esi, 16
        mov [di], si
        rol esi, 16
        add esi, edx
        add di, 2
        inc cx
        cmp cx, y2
        jle @@l1
        jmp @@l3

@@l2:
        add esi, edx
        ror esi, 16
        mov [di], si
        rol esi, 16
        add di, 2
        inc cx
        cmp cx, y2
        jle @@l2

@@l3:
        sub di, 2
@@end:

        pop bp
        ret
line endp

xbl	dw 800 DUP (0)