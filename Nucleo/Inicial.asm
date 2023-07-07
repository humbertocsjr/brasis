cpu 386
bits 32

section .text

global __start
__start:
    ; Guardando ponteiro de parametros
    mov ebx, esi

    ; Limpando area BSS
    xor eax, eax
    extern __start__bss
    mov esi, __start__bss
    extern __stop__bss
    mov ecx, __stop__bss
    sub ecx, __start__bss
    rep stosb

    ; Definindo pilha
    mov esp, __pilha_topo
    
    ; Chamando Inicial
    extern __stop_alldata__
    push dword __stop_alldata__
    extern __start_alldata__
    push dword __start_alldata__
    extern __stop_allcode__
    push dword __stop_allcode__
    extern __start_allcode__
    push dword __start_allcode__
    push ebx
    extern _Inicial
    call _Inicial
    jmp $


section .bss
; Pilha do núcleo
__pilha: 
resb 8192
__pilha_topo:
