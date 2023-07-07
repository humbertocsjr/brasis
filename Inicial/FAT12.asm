; BRASIS Inicial
; Reescrita para FAT12 inspirado no tutorial Brokenthorn
; Copyright (c) 2023, Humberto Costa dos Santos Junior

cpu 386
bits 16
org 0
inicio:
    jmp short carregar
    nop

; Indice FAT
; ==========

times 0xb - $ + inicio db 0

fatBytesPorSetor:      DW 512
fatSetoresPorConj:     DB 1
fatSetoresReservados:  DW 1
fatQtdTabelas:         DB 2
fatItensNaRaiz:        DW 224
fatTotalSetores:       DW 2880
fatMidia:              DB 0xF0
fatSetoresPorTabela:   DW 9
fatSetoresPorTrilha:   DW 18
fatCabecasPorCilindro: DW 2
fatSetoresEscondidos:  dw 0,0
fatTotalSetores32b:    dw 0,0
fatDiscoNumero:        DB 0
fatNaoUsasdo:          DB 0
fatAssinatura:         DB 0x29
fatNumeroDeSeria:      dw 0xc7d6, 0xa9b8
fatRotulo:             DB "BRASIS     "
fatSisArq:             DB "FAT12   "

paramArquivo:          db "ESTAGIO2BIN"
varSetoresDaRaiz:      dw 0
varRaizEndereco:       dw 0
varDadosEndereco:      dw 0
varSetoresDasTabelas:  dw 0
varConj:               dw 0

; Inicializacao
; =============

carregar:
    cli
    xor ax, ax
    mov ss, ax
    mov sp, 0x7bfe
    mov ax, 0x7c0
    mov gs, ax
    mov fs, ax
    mov es, ax
    mov ds, ax
    jmp 0x7c0:.continua
    .continua:
    sti

    ; Calcula Qtd de Setores da Raiz
    mov ax, 32
    mul word [fatItensNaRaiz]
    div word [fatBytesPorSetor]
    mov [varSetoresDaRaiz], ax
    mov [varDadosEndereco], ax

    ; Calcula posicao da raiz
    xor ax, ax
    mov al, [fatQtdTabelas]
    mul word [fatSetoresPorTabela]
    mov [varSetoresDasTabelas], ax
    add ax, [fatSetoresReservados]
    mov [varRaizEndereco], ax
    add [varDadosEndereco], ax

    mov cx, [varSetoresDaRaiz]
    mov bx, 0x200
    call escreva
    db "Montando",0
    call disco_leia
    call ok
    call escreva
    db "Buscando",0

    mov cx, [fatItensNaRaiz]
    mov di, 0x200
    .loop:
        push cx
        mov cx, 11
        mov si, paramArquivo
        mov ax, [di + 0x1a]
        mov [varConj], ax
        mov ax, di
        rep cmpsb
        mov di, ax
        call ponto
        je .encontrado
        pop cx
        add di, 32
        loop .loop
        call erro
        jmp infinito
    .encontrado:
    pop cx
    call ok
    call escreva
    db "FAT",0
    
    ; Carregando FAT
    mov cx, [varSetoresDasTabelas]
    mov ax, [fatSetoresReservados]
    mov bx, 0x200
    call disco_leia

    call ok

    call escreva
    db "Carregando",0

    ; Leia o arquivo
    mov ax, 0x60
    mov es, ax
    xor bx, bx
    push bx

    .carrega:
        mov ax, [varConj]
        sub ax, 2
        xor cx, cx
        mov cl, [fatSetoresPorConj]
        mul cx
        add ax, [varDadosEndereco]
        pop bx
        call disco_leia
        push bx
        mov ax, [varConj]
        mov cx, ax
        mov dx, ax
        shr dx, 1
        add cx, dx
        mov bx, 0x200
        add bx, cx
        mov dx, [bx]
        test ax, 1
        jnz .impar
        .par:
            and dx, 0xfff
            jmp short .proximo
        .impar:
            shr dx, 4
        .proximo:
        mov [varConj], dx
        cmp dx, 0xff0
        jb .carrega
    call ok
    xor ax, ax
    mov ds, ax
    mov fs, ax
    mov gs, ax
    xor dx, dx
    mov dl, [fatDiscoNumero]
    mov cx, [fatSetoresPorTrilha]
    mov bx, [fatCabecasPorCilindro]
    jmp 0:0x600

disco_leia:
    mov si, 6
    .loop:
        push ax
        push bx
        push cx
        xor dx, dx
        div word [fatSetoresPorTrilha]
        inc dl
        mov cl, dl
        xor dx, dx
        div word [fatCabecasPorCilindro]
        mov ch, al
        mov dh, dl
        mov dl, [fatDiscoNumero]
        mov ax, 0x201
        int 0x13
        jnc .ok
        xor ax, ax
        int 0x13
        dec si
        pop cx
        pop bx
        pop ax
        jnz .loop
        call erro
        jmp infinito
    .ok:
        call ponto
        pop cx
        pop bx
        pop ax
        add bx, word [fatBytesPorSetor]
        inc ax
        loop disco_leia
        ret


ponto:
    call escreva
    db ".",0
    ret

ok:
    call escreva
    db "[ OK ]",13,10,0
    ret

erro:
    call escreva
    db "[ Erro ]",0
    ret

infinito:
    hlt
    jmp infinito

escreva:
    push bp
    mov bp, sp
    pushf
    push ax
    push si
    push bx
    mov si, [bp+2]
    .loop:
        lodsb
        cmp al, 0
        je .fim
        xor bx, bx
        mov ah, 0xe
        int 0x10
        jmp short .loop
    .fim:
    mov [bp+2], si
    pop bx
    pop si
    pop ax
    popf
    pop bp
    ret
	
times 510 - ($-$$) db 0
db 0x55, 0xaa
