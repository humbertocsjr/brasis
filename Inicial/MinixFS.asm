; MinixFSv1 Bootloader
; Humberto Costa dos Santos Junior, 2020-2023
; 3-Clause BSD license

ConstCabecas: equ 2
ConstSetoresPorTrilha: equ 18

cpu 8086
jmp short PreInicio
db "HC"
SetoresPorTrilha:
    dw ConstSetoresPorTrilha
Cabecas:
    dw ConstCabecas
Disco:
    dw 0
SegTemp:
    dw 0
SegTempItens:
    dw 0
TotalItens:
    dw 0
TotalBlocos:
    dw 0
PosItens:
    dw 0
TotalBlocosItens:
    dw 0
TamListaItens:
    dw 0
Destino:
    dw 0x50 + (256 / 16)
Chamar:
    dw 0
ChamarIP:
    dw 0x500 + 256

PreInicio:
    ; Realoca para o fim da memoria RAM
    cld
    xor ax, ax
    int 0x12
    mov cl, 6
    shl ax, cl
    sub ax, 5120/16
    mov es, ax
    mov ax, 0x7c0
    mov ds, ax
    xor di, di
    xor si, si
    mov cx, 256
    rep
    movsw
    ; Inicia os registradores e pilha
    mov ax, es
    mov ds, ax
    cli
    mov ss, ax
    mov sp, 4096
    sti
    ; Pula para o inicio
    mov [Disco], dl
    push ax
    mov ax, Inicio
    push ax
    retf

Escreva:
    push bp
    mov bp, sp
    push ax
    push si
    push ds
    push cs
    pop ds
    mov si, [bp + 2]
    _inicial1_loop:
        lodsb
        cmp al, 0
        je _inicial1_fim
        mov ah, 0xe
        int 0x10
        jmp short _inicial1_loop
    _inicial1_fim:
    mov [bp + 2], si
    pop ds
    pop si
    pop ax
    mov sp, bp
    pop bp
    ret

;#func DiscoLeia
;#arg es:0 = Destino
;#arg dx:ax = Endereco
;#arg cx = Blocos
DiscoLeia:
    push es
    push ds
    push ax
    push bx
    push cx
    push dx
    push di
    ; Prepara
    xor di, di
    xor bx, bx
    .leia:
        push ax
        push bx
        push dx
        push cx
        cs div word [SetoresPorTrilha]
        mov cl, dl
        xor dx, dx
        cs div word [Cabecas]
        mov ch, al
        mov dh, dl
        ror ah, 1
        ror ah, 1
        and ah, 0x3f
        inc cl
        or cl, ah
        mov si, 5
        cs mov dl, [Disco]
        xor bx, bx
        .tenta:
            call Ponto
            mov ax, 0x201
            int 0x13
            jnc .ok
            xor ax, ax
            int 0x13
            dec si
            cmp si, 0
            jne .tenta
            call EscrevaCHS
            jmp Erro
        .ok:
        mov ax, es
        add ax, 512/16
        mov es, ax
        pop cx
        pop dx
        pop bx
        pop ax
        add ax, 1
        adc dx, 0
        loop .leia
    stc
    .fim:
    pop di
    pop dx
    pop cx
    pop bx
    pop ax
    pop ds
    pop es
    ret

Inicio:
    ; Calcula posicao temporaria
    mov ax, cs
    add ax, 4096/16
    mov [SegTemp], ax
    mov es, ax
    ; Calcula posicao da extensao 
    mov ax, cs
    add ax, 512/16
    mov es, ax
    ; Exibe mensagem
    call Escreva
    db 10, 13, "Montando", 0
    ; Carrega bloco extensao
    mov ax, 1
    xor dx, dx
    mov cx, 1
    call DiscoLeia
    ; Carrega bloco indice do MinixFS
    mov ax, [SegTemp]
    mov es, ax
    mov ax, 2
    xor dx, dx
    mov cx, 1
    call DiscoLeia
    ; Verifica a versao do MinixFS
    es mov ax, [16]
    ; Versao 1 com 30 bytes de nome
    cmp ax, 5007 
    je .ok
        call Erro
    .ok:
    call Ponto
    ; Copia dados importantes para o carregamento
    es mov ax, [2]
    mov [TotalBlocos], ax
    es mov ax, [0]
    mov [TotalItens], ax
    mov cx, 5
    shr ax, cl
    mov [TotalBlocosItens], ax
    mov ax, [TotalItens]
    mov cx, 5
    shl ax, cl
    mov [TamListaItens], ax
    mov ax, 2
    es add ax, [4]
    es add ax, [6]
    mov [PosItens], ax
    ; Calcula posicao da lista de itens
    mov ax, cs
    mov bx, [TamListaItens]
    mov cx, 3
    shr bx, cl
    sub ax, bx
    mov es, ax
    mov [SegTempItens], ax
    call Ponto
    ; Carrega Lista de Itens
    xor dx, dx
    mov cx, [TotalBlocosItens]
    mov ax, [PosItens]
    call DiscoLeia1KiB
    call Ok
    call BuscaArq


    jmp Infinito

EscrevaUShort:
    pushf
    push ax
    push dx
    xor dx, dx
    cs div word [.div]
    cmp ax, 0
    je .pula
        call EscrevaUShort
    .pula:
    mov ah, 0xe
    mov al, dl
    add al, '0'
    int 0x10
    pop dx
    pop ax
    popf
    ret
    .div:
        dw 10

DiscoLeia1KiB:
    push ax
    push dx
    push cx
    shl ax, 1
    rcl dx, 1
    shl cx, 1
    call DiscoLeia
    pop cx
    pop dx
    pop ax
    ret

Ponto:
    call Escreva
    db ".", 0
    ret

Ok:
    call Escreva
    db "[ OK ]", 13, 10, 0
    ret

Erro:
    call Escreva
    db "[ Erro ]",0

Infinito:
    hlt
    jmp Infinito


Fim:
times 510-($-$$) db 0
db 0x55, 0xaa
; ==========================================================================
; Bloco 0:0:2
; ==========================================================================
; Bloco extensao
Nome:
    db "BRASIS.Inicial", 0
FimNome:
times 542-($-$$) db 0

BuscaArq:
    call Escreva
    db "Buscando", 0
    mov ax, [SegTemp]
    mov es, ax
    mov bx, 0
    .loop:
        mov ax, bx
        mov ax, 1
        call CarregaBloco
        jc .ok
        .erro:
            call Erro
        .ok:
        mov di, 2
        mov cx, 32
        .busca:
            call Ponto
            push bx
            push cx
            push di
            cmp di, 1024
            ja .erro
            mov si, Nome
            mov cx, 30
            rep
            cmpsb
            pop di
            pop cx
            pop bx
            je .encontrado
            add di, 32
            mov ax, di
            loop .busca
        inc bx
        jmp .busca
    .encontrado:
    call Ok
    call Escreva
    db "Carregando ", 0
    es mov ax, [di-2]
    call EscrevaUShort
    push ax
    mov ax, [Destino]
    mov es, ax
    pop ax
    xor bx, bx
    .arq:
        call CarregaBloco
        jnc .fim
        push ax
        mov ax, es
        add ax, 1024/16
        mov es, ax
        pop ax
        inc bx
        jmp .arq
    .fim:
    call Ok
    cs mov ax, [Chamar]
    mov es, ax
    cs mov dx, [Disco]
    cmp dx, 0x80
    jb _inicial10_disquete
        and dx, 0x7
        cs mov byte [ArgumentoDisco], '1'
        cs mov byte [ArgumentoDisco+1], '2'
    _inicial10_disquete:
    add dx, '0'
    cs mov [ArgumentoDisco+2], dl
    mov ax, cs
    mov ds, ax
    mov si, Argumento
    mov di, 0x81
    mov cx, Tam_Argumento
    es mov [0x80], cl
    rep
    movsb
    cs mov ax, [Chamar]
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0xfffe
    mov word [0x10], 1989
    cs mov dx, [Disco]
    cs mov cx, [SetoresPorTrilha]
    cs mov bx, [Cabecas]
    xor di, di
    xor si, si
    push ds
    cs push word [ChamarIP]
    retf

; ax = Item
; bx = Zona
CarregaBloco:
    push ax
    push bx
    push cx
    push dx
    push es
    push ds
    push si
    ; Define o ponteiro ds:si para o item
    dec ax
    mov cx, ax
    mov ax, [SegTempItens]
    mov ds, ax
    mov ax, 32
    mul cx
    mov si, ax
    cmp bx, 7
    jb .direto
    cmp bx, 7 + 512
    jb .indireto
    call Erro
    .direto:
        add si, 0xe
        shl bx, 1
        mov ax, [bx + si]
        cmp ax, 0
        je .vazio
        xor dx, dx
        mov cx, 1
        call Ponto
        ;call Escreva
        ;.asciz "["
        ;call EscrevaUShort
        ;call Escreva
        ;.asciz "]"
        call DiscoLeia1KiB
        stc
        jmp .fim
    .indireto:
        add si, 0x1c
        sub bx, 7
        shl bx, 1
        mov ax, [si]
        cmp ax, 0
        je .vazio
        call Ponto
        ;call Escreva
        ;.asciz "[Indireto "
        push es
        cs push word [SegTemp]
        pop es
        xor dx, dx
        mov cx, 1
        ;call EscrevaUShort
        ;call Escreva
        ;.asciz "]"
        call DiscoLeia1KiB
        es mov ax, [bx]
        pop es
        cmp ax, 0
        je .vazio
        xor dx, dx
        mov cx, 1
        call Ponto
        ;call Escreva
        ;.asciz "["
        ;call EscrevaUShort
        ;call Escreva
        ;.asciz "]"
        call DiscoLeia1KiB
        stc
        jmp .fim
    .vazio:
        clc
    .fim:
    pop si
    pop ds
    pop es
    pop dx
    pop cx
    pop bx
    pop ax
    ret

EscrevaCHS:
    push ax
    xor ax, ax
    call Escreva
    db "[", 0
    mov al, ch
    call EscrevaUShort
    call Escreva
    db ":", 0
    mov al, dh
    call EscrevaUShort
    call Escreva
    db ":", 0
    mov al, cl
    call EscrevaUShort
    call Escreva
    db "]", 0
    pop ax
    ret


times 952-($-$$) db 0
Argumento:
db " Disco="
ArgumentoDisco:
db "000"
db "", 13, 0

FimArgumento:
Tam_Argumento: equ $-Argumento-2
times 1024-($-$$) db 0