cpu 386
bits 16
org 0x600


ModoVideo:
    .Texto: equ 0
    .EGA: equ 1
    .VGA: equ 2
    .SVGA: equ 3
    .VBE2: equ 4

jmp near inicial
times 8-($-$$) db 0
param:
db "BRASIS"
db 1,0 ; Versão
dw ModoVideo.VGA
dw 640
dw 480
dw 4
param_nucleo:
dd 0x100000
param_tam_nucleo:
dd 0
param_imagem:
dd 0x200000
param_tam_imagem:
dd 0
param_video:
dd 0xb8000
param_tam_video:
dd 2000
param_mem_baixa:
dd 0
param_mem_media:
dd 0
param_mem_alta:
dd 0
param_arq_cfg:
db "BRASIS.cfg",0
times 32-($-param_arq_cfg) db 0
param_arq_nucleo:
times 32-($-param_arq_nucleo) db 0
param_arq_imagem:
times 32-($-param_arq_imagem) db 0

disco: dd 0
cabecas: dd 0
setores: dd 0
bloco_seg: dw 0
temp_seg: dw 0
temp_itens_seg: dw 0


; =====================================
; Acesso ao disco
; =====================================

disco_leia_1kib:
    cmp [bloco_pos], ax
    jne .executa
    cmp [bloco_pos+2], dx
    jne .executa
    cmp [bloco_qtd], cx
    jne .executa
    jmp .fim
    .executa:
    push ax
    push dx
    push cx
    shl ax, 1
    rcl dx, 1
    shl cx, 1
    call disco_leia
    pop cx
    pop dx
    pop ax
    mov [bloco_pos], ax
    mov [bloco_pos+2], dx
    mov [bloco_qtd], cx
    .fim:
    ret

;es:0 = Destino
;dx:ax = Endereco
;cx = Blocos
disco_leia:
    push es
    cs push word [bloco_seg]
    pop es
    call disco_leia_direto
    pop es
    mov [bloco_pos], ax
    mov [bloco_pos+2], dx
    mov [bloco_qtd], cx
    ret


disco_leia_direto_1kib:
    push ax
    push dx
    push cx
    shl ax, 1
    rcl dx, 1
    shl cx, 1
    call disco_leia_direto
    pop cx
    pop dx
    pop ax
    ret

;es:0 = Destino
;dx:ax = Endereco
;cx = Blocos
disco_leia_direto:
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
        cs div word [setores]
        mov cl, dl
        xor dx, dx
        cs div word [cabecas]
        mov ch, al
        mov dh, dl
        ror ah, 1
        ror ah, 1
        and ah, 0x3f
        inc cl
        or cl, ah
        mov si, 5
        cs mov dl, [disco]
        xor bx, bx
        .tenta:
            call escreva_ponto
            mov ax, 0x201
            int 0x13
            jnc .ok
            xor ax, ax
            int 0x13
            dec si
            cmp si, 0
            jne .tenta
            call escreva
            db "[ Erro ]",0
            .infinito:
            hlt
            jmp .infinito
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


; =====================================
; Escrita na tela
; =====================================

escreva:
    push bp
    mov bp, sp
    pusha
    mov ax, cs
    mov ds, ax
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
    popa
    pop bp
    ret

escreva_si:
    push bp
    mov bp, sp
    push ax
    push si
    push bx
    .loop:
        lodsb
        cmp al, 0
        je .fim
        xor bx, bx
        mov ah, 0xe
        int 0x10
        jmp short .loop
    .fim:
    pop bx
    pop si
    pop ax
    pop bp
    ret

escreva_ok:
    call escreva
    db "[ OK ]",13,10,0
    ret

escreva_ponto:
    call escreva
    db ".",0
    ret

escreva_int:
    pusha
    xor dx, dx
    mov bx, 10
    div bx
    cmp ax, 0
    je .fim
        call escreva_int
    .fim:
    mov ax, dx
    add ax, '0'
    mov ah, 0xe
    xor bx, bx
    int 0x10
    popa
    ret

escreva_hex_byte:
    pusha
    mov cx, 2
    .loop:
        ror al, 4
        push ax
        and ax, 0xf
        cmp al, 10
        jb .numero
            add al, 'A' - 10
            jmp .fim
        .numero:
            add al, '0'
        .fim:
        xor bx, bx
        mov ah, 0xe
        int 0x10
        pop ax
        loop .loop
    popa
    ret

escreva_hex_word:
    pusha
    xchg al, ah
    call escreva_hex_byte
    xchg al, ah
    call escreva_hex_byte
    popa
    ret

escreva_hex_bloco:
    pusha
    call escreva
    db 13,10,0
    mov dx, 16
    .linha:
        mov cx, 32
        .loop:
            lodsb
            call escreva_hex_byte
            loop .loop
        call escreva
        db 13,10,0
        dec dx
        jnz .linha
    popa
    ret


; =====================================
; Ativacao da Linha A20 da Memoria
; =====================================

teste_a20: dw 0

verifica_a20:
    pushf
    push ds
    push es
    push di
    push si
 
    cli
 
    xor ax, ax ; ax = 0
    mov es, ax
 
    not ax ; ax = 0xFFFF
    mov ds, ax
 
    mov di, 0x0500
    mov si, 0x0510
 
    mov al, byte [es:di]
    push ax
 
    mov al, byte [ds:si]
    push ax
 
    mov byte [es:di], 0x00
    mov byte [ds:si], 0xFF
 
    cmp byte [es:di], 0xFF
 
    pop ax
    mov byte [ds:si], al
 
    pop ax
    mov byte [es:di], al
 
    je .fim
 
    pop si
    pop di
    pop es
    pop ds
    popf
    stc
    ret
 
    .fim:
    pop si
    pop di
    pop es
    pop ds
    popf
    clc
    ret

a20_metodo1:
    in al, 0x92
    test al, 2
    jnz .fim
    or al, 2
    and al, 0xFE
    out 0x92, al
    .fim:
    ret

a20_metodo4:
    in al, 0xee
    ret

a20_metodo3:
    mov al, 0xdd
    out 0x64, al
    ret

a20_metodo2:
    call .espera_entrada
    mov al, 0xad
    out 0x64, al
    call .espera_entrada

    mov al, 0xd0
    out 0x64, al
    call .espera_saida

    in al, 0x60
    push eax
    call .espera_entrada

    mov al, 0xd1
    out 0x64, al
    call .espera_entrada

    pop eax
    or al, 2
    out 0x60, al

    call .espera_entrada
    mov al, 0xae
    out 0x64, al

    call .espera_entrada
    
    .espera_saida:
        in al, 0x64
        test al, 1
        jz .espera_saida
        ret
    .espera_entrada:
        in al, 0x64
        test al, 2
        jnz .espera_entrada
        ret

_gdt_tabela:
    .nulo:
        dw 0,0,0,0
    .codigo:
        dw 0xffff
        dw 0
        db 0
        db 0b10011010
        db 0b11001111
        db 0
    .dados:
        dw 0xffff
        dw 0
        db 0
        db 0b10010011
        db 0b11001111
        db 0
    .fim:

_gdt_indice:
    dw _gdt_tabela.fim - _gdt_tabela - 1
    dd _gdt_tabela


; =====================================
; Sistema de Arquivos Generico
; =====================================

sa_raiz: dw 0
sa_primeiro: dw 0
sa_proximo: dw 0
sa_abrir: dw 0
sa_leia: dw 0
sa_fecha: dw 0
sa_total_blocos: dd 0 
sa_itens_total: dd 0
sa_itens_total_blocos: dd 0
sa_itens_lista_tam: dd 0
sa_itens_pos: dd 0
sa_item_nome: times 32 db 0
sa_item_id: dd 0
sa_item_bloco: dd 0
sa_diretorio_id: dd 0
sa_diretorio_bloco: dd 0
sa_diretorio_subpos: dw 0
bloco_pos: dd 0
bloco_sub: dw 0
bloco_qtd: dw 0


; =====================================
; Implementacao MinixFS
; =====================================

minixfs1:
    call escreva_ok
    call escreva
    db " - Montando MinixFS",0
    mov word [sa_raiz], minixfs1_raiz
    mov word [sa_primeiro], minixfs1_primeiro
    mov word [sa_proximo], minixfs1_proximo
    mov word [sa_abrir], minixfs1_abrir
    mov word [sa_leia], minixfs1_leia
    mov word [sa_fecha], minixfs1_fecha
    mov ax, 1
    xor dx, dx
    mov cx, 1
    call disco_leia_1kib
    mov ax, [bloco+2]
    mov [sa_total_blocos], ax
    mov ax, [bloco+0]
    mov [sa_itens_total], ax
    mov cx, 5
    shr ax, cl
    mov [sa_itens_total_blocos], ax
    mov ax, [sa_itens_total]
    mov cx, 5
    shl ax, cl
    mov [sa_itens_lista_tam], ax
    mov ax, 2
    add ax, [bloco+4]
    add ax, [bloco+6]
    mov [sa_itens_pos], ax
    ; Carrega Lista de Itens
    push es
    xor dx, dx
    mov ax, [temp_itens_seg]
    mov es, ax
    mov cx, [sa_itens_total_blocos]
    mov ax, [sa_itens_pos]
    call disco_leia_direto_1kib
    pop es
    call escreva_ponto
    call escreva_ok
    ret


; ax = Item
; bx = Zona
minixfs1_carrega_bloco:
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
    cs mov ax, [temp_itens_seg]
    mov ds, ax
    mov ax, 32
    mul cx
    mov si, ax
    cmp bx, 7
    jb .direto
    cmp bx, 7 + 512
    jb .indireto
    call escreva
    db "[ Erro ]",0
    .infinito:
    hlt
    jmp .infinito
    .direto:
        add si, 0xe
        shl bx, 1
        mov ax, [bx + si]
        cmp ax, 0
        je .vazio
        xor dx, dx
        mov cx, 1
        call escreva_ponto
        ;call escreva
        ;db "[",0
        ;call escreva_int
        ;call escreva
        ;db "]",0
        call disco_leia_1kib
        ;mov si, bloco
        ;call escreva_hex_bloco
        stc
        jmp .fim
    .indireto:
        add si, 0x1c
        sub bx, 7
        shl bx, 1
        mov ax, [si]
        cmp ax, 0
        je .vazio
        call escreva_ponto
        ;call Escreva
        ;.asciz "[Indireto "
        push es
        cs push word [temp_seg]
        pop es
        xor dx, dx
        mov cx, 1
        ;call EscrevaUShort
        ;call Escreva
        ;.asciz "]"
        call disco_leia_direto_1kib
        es mov ax, [bx]
        pop es
        cmp ax, 0
        je .vazio
        xor dx, dx
        mov cx, 1
        call escreva_ponto
        ;call Escreva
        ;.asciz "["
        ;call EscrevaUShort
        ;call Escreva
        ;.asciz "]"
        call disco_leia_1kib
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


minixfs1_raiz:
    push es
    mov dword [sa_diretorio_id], 1
    mov dword [sa_diretorio_bloco], 0
    mov word [sa_diretorio_subpos], 32
    mov dword [sa_item_id], 1
    mov byte [sa_item_nome], '/'
    mov byte [sa_item_nome + 1], 0
    pop es
    ret

minixfs1_primeiro:
    push es
    mov word [sa_diretorio_bloco], 0
    mov word [sa_diretorio_subpos], 0-32
    call minixfs1_proximo
    pop es
    ret

minixfs1_proximo:
    push es
    mov ax, [sa_diretorio_id]
    mov bx, [sa_diretorio_bloco]
    call minixfs1_carrega_bloco
    jc .ok
        clc
        pop es
        ret
    .ok:
    xor esi, esi
    xor edi, edi
    mov si, [sa_diretorio_subpos]
    add si, 32
    mov [sa_diretorio_subpos], si
    cmp si, 1023
    jb .pos_ok
        inc word [sa_diretorio_bloco]
        pop es
        jmp minixfs1_proximo
    .pos_ok:
    mov [sa_diretorio_subpos], si
    add si, bloco
    mov di, sa_item_id
    mov ax, [si]
    cmp word [si], 0
    je .ok
    movsw
    mov di, sa_item_nome
    mov cx, 30
    rep movsb
    stc
    pop es
    ret

minixfs1_abrir:
    mov word [sa_item_bloco], 0
    cmp word [sa_item_id], 0
    jne .ok
        clc
        ret
    .ok:
    stc
    ret

minixfs1_leia:
    mov ax, [sa_item_id]
    mov bx, [sa_item_bloco]
    inc word [sa_item_bloco]
    call minixfs1_carrega_bloco
    ret

minixfs1_fecha:
    stc
    ret

; =====================================
; Rotina Principal
; =====================================

inicial:
    cli

    xor ax, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ax, 0x9000
    mov ss, ax
    mov sp, 0xfffe
    and edx, 0xf
    mov [disco], edx
    mov [cabecas], ebx
    mov [setores], ecx

    call escreva
    db "BRASIS Sistema Inicial - Etapa 2", 13, 10, 13, 10, " - Calculando tamanho da memoria",0

    clc
    xor ax, ax
    int 0x12
    jnc .mem_baixa_ok
        mov eax, 640 * 1024
        mov [param_mem_baixa], eax
        jmp .mem_baixa_fim
    .mem_baixa_ok:
    and eax, 0xffff
    mov ebx, 1024
    mul ebx
    mov [param_mem_baixa], eax
    .mem_baixa_fim:
    call escreva_ponto

    xor eax, eax
    xor ebx, ebx
    xor ecx, ecx
    xor edx, edx
    mov [param_mem_media], eax
    mov [param_mem_alta], eax
    a32 nop

    stc
    mov ax, 0xe801
    int 0x15
    jc .mem_e801_erro

        mov ebx, edx
        and ebx, 0xffff
        and ecx, 0xffff
        mov eax, 1024
        mul ecx

        mov [param_mem_media], eax
        a32 nop
        mov eax, 64*1024
        mul ebx
        mov [param_mem_alta], eax
        a32 nop
        jmp .mem_alta_ok

    .mem_e801_erro:
    
        stc
        mov ax, 0x8800
        int 0x15
        mov ebx, 1024
        and eax, 0xffff
        mul ebx
        add eax, 1024*1024
        .mem_88_ok:
        mov [param_mem_media], eax
        a32 nop

    .mem_alta_ok:
    call escreva_ponto

    call escreva_ok

    call escreva
    db " - Ativando A20" ,0
    
    call verifica_a20
    jc .a20_ok
    call a20_metodo1
    call escreva_ponto

    call verifica_a20
    jc .a20_ok
    call a20_metodo2
    call escreva_ponto

    call verifica_a20
    jc .a20_ok
    call a20_metodo3
    call escreva_ponto
    
    call verifica_a20
    jc .a20_ok
    call a20_metodo4
    call escreva_ponto
    
    call verifica_a20
    jc .a20_ok
    
    call escreva
    db "[ Erro ]",0
    .a20_erro:
        hlt
        jmp .a20_erro
    .a20_ok:
    call escreva_ok

    call escreva
    db " - Entrando em modo Unreal",0

    lgdt [_gdt_indice]

    call escreva_ponto

    push ds
    push es

    mov eax, cr0
	or eax, 1
	mov cr0, eax

bits 32
    mov bx, 0x10
    mov ds, bx
    mov es, bx

    and al, 0xfe
    mov cr0, eax

bits 16
    pop es
    pop ds
    sti

    call escreva_ok

    call escreva
    db " - Abrindo disco ",0

    ; Calcula ponteiro do bloco para uso do disco
    mov ax, bloco
    shr ax, 4
    mov [bloco_seg], ax
    mov ax, temp
    shr ax, 4
    mov [temp_seg], ax
    mov ax, temp_itens
    shr ax, 4
    mov [temp_itens_seg], ax
    
    mov ax, [disco]
    call escreva_int

    call escreva
    db " [H: ",0
    
    mov ax, [cabecas]
    call escreva_int

    call escreva
    db " S: ",0
    
    mov ax, [setores]
    call escreva_int

    call escreva
    db "]",0

    mov ax, 1
    xor dx, dx
    mov cx, 1
    call disco_leia_1kib

    mov esi, bloco
    a32 nop ; Usado para evitar algum bug do 386 quando mistura modo de enderecamentos

    call escreva_ponto

    a32 nop
    add esi, 16
    a32 es lodsw
    a32 nop

    call escreva_ponto
    ; Versao 1 com 30 bytes de nome
    cmp ax, 5007 
    jne .naoMinixFS
        call minixfs1
        jmp .continua
    .naoMinixFS:
    call escreva
    db "[ Sistema de Arquivos nao suportado ]",0
    jmp .infinito

    .continua:


    call escreva
    db " - Busca arquivo de configuracao",0

    call [sa_raiz]
    call [sa_primeiro]
    jc .busca
    jmp .nao_encontrado
    .busca:
        call escreva_ponto
        mov cx, 32
        mov si, sa_item_nome
        mov di, param_arq_cfg
        rep cmpsb
        je .encontrado

        call [sa_proximo]
        jc .busca
    .nao_encontrado:
    call escreva
    db "[ Arquivo nao encontrado ]",0
    jmp .infinito
    .encontrado:
    call escreva_ok

    call escreva
    db " - Carregando configuracao [Max 1KiB]",0

    call escreva_ponto
    call [sa_abrir]
    jc .cfg_aberto
        call escreva
        db "[ Arquivo nao encontrado ]",0
        jmp .infinito
    .cfg_aberto:
    call escreva_ponto
    call [sa_leia]
    jc .cfg_leia
        call escreva
        db "[ Arquivo ilegivel ]",0
        jmp .infinito
    .cfg_leia:
    call escreva_ok

    mov si, bloco
    mov di, param_arq_nucleo
    mov cx, 32
    call .leiastr
    mov di, param_arq_imagem
    mov cx, 32
    call .leiastr

    call escreva
    db " - Carregando ",0
    mov si, param_arq_nucleo
    call escreva_si

    ; Carrega o nucleo
    
    call [sa_raiz]
    call [sa_primeiro]
    jc .busca_nucleo
    jmp .nao_encontrado_nucleo
    .busca_nucleo:
        call escreva_ponto
        mov cx, 32
        mov si, sa_item_nome
        mov di, param_arq_nucleo
        rep cmpsb
        je .encontrado_nucleo

        call [sa_proximo]
        jc .busca_nucleo
    .nao_encontrado_nucleo:
    call escreva
    db "[ Arquivo nao encontrado ]",0
    jmp .infinito
    .encontrado_nucleo:

    xor eax, eax
    mov [param_tam_nucleo], eax
    mov eax, [param_nucleo]
    mov [.var_nucleo_ptr], eax
    xor eax, eax

    call escreva_ponto
    call [sa_abrir]
    jc .nucleo_aberto
        call escreva
        db "[ Nucleo nao encontrado ]",0
        jmp .infinito
    .nucleo_aberto:
    .nucleo_loop:
        call escreva_ponto
        call [sa_leia]
        jc .nucleo_leia
            jmp .nucleo_fim
        .nucleo_leia:
        mov esi, bloco
        mov edi, [.var_nucleo_ptr]
        mov ecx, 1024
        nop
        a32 nop
        a32 rep movsb
        a32 nop
        nop

        mov eax, [param_tam_nucleo]
        add eax, 1024
        mov [param_tam_nucleo], eax
        mov eax, [.var_nucleo_ptr]
        add eax, 1024
        mov [.var_nucleo_ptr], eax
        jmp .nucleo_loop
        .var_nucleo_ptr: dd 0

    .nucleo_fim:

    call escreva_ok

    call escreva
    db " - Carregando ",0
    mov si, param_arq_imagem
    call escreva_si


    ; Carrega a imagem
    
    call [sa_raiz]
    call [sa_primeiro]
    jc .busca_img
    jmp .nao_encontrado_img
    .busca_img:
        call escreva_ponto
        mov cx, 32
        mov si, sa_item_nome
        mov di, param_arq_imagem
        rep cmpsb
        je .encontrado_img

        call [sa_proximo]
        jc .busca_img
    .nao_encontrado_img:
    call escreva
    db "[ Arquivo nao encontrado ]",0
    jmp .infinito
    .encontrado_img:

    xor eax, eax
    mov [param_tam_imagem], eax
    mov eax, [param_imagem]
    mov [.var_img_ptr], eax
    xor eax, eax

    call escreva_ponto
    call [sa_abrir]
    jc .img_aberto
        call escreva
        db "[ Imagem nao encontrada ]",0
        jmp .infinito
    .img_aberto:
    .img_loop:
        call escreva_ponto
        call [sa_leia]
        jc .img_leia
            jmp .img_fim
        .img_leia:
        mov esi, bloco
        mov edi, [.var_img_ptr]
        mov ecx, 1024
        nop
        a32 nop
        a32 rep movsb
        a32 nop
        nop

        mov eax, [param_tam_imagem]
        add eax, 1024
        mov [param_tam_imagem], eax
        mov eax, [.var_img_ptr]
        add eax, 1024
        mov [.var_img_ptr], eax
        jmp .img_loop
        .var_img_ptr: dd 0

    .img_fim:



    call escreva_ok

    
    call escreva
    db 13,10,"Executando",13,10,0

    cli

    mov eax, cr0
	or eax, 1
	mov cr0, eax
    jmp 0x8:protegido



    .infinito:
    hlt
    jmp short .infinito

    .leiastr:
        lodsb
        cmp al, 13
        je .leiastr
        cmp al, 10
        je .leiastr
        jmp .leiastr_processa
        .leiastr_loop:
            lodsb
            .leiastr_processa:
            cmp al, 0
            je .leiastr_fim
            cmp al, 13
            je .leiastr_fim
            cmp al, 10
            je .leiastr_fim
            stosb
            loop .leiastr_loop
        .leiastr_fim:
        xor al, al
        stosb
        ret
    


bits 32
protegido:
    mov bx, 0x10
    mov ds, bx
    mov es, bx
    mov fs, bx
    mov gs, bx
    mov ss, bx
    mov esp, [param_mem_baixa]

    mov esi, param
    push dword [param_nucleo]
    ret


align 16
fim:
bloco: equ fim + 16
temp: equ bloco + 1024
temp_itens: equ bloco + 1024 + 1024

