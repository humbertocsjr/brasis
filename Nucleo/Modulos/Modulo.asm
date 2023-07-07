cpu 386
bits 32

section .text

global __start
__start:
    push esi
    extern _Modulo
    call _Modulo
    jmp $