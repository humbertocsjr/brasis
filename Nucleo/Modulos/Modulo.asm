cpu 386
bits 32

section .text

global __start
__start:
    pushf
    pushad
    mov [_Nucleo], esi
    extern _Modulo
    call _Modulo
    popad
    popf
    ret

global _Debug_Escreva
_Debug_Escreva:
    mov ebx, [_Nucleo]
    jmp dword [ebx+4]

global _ES_EscrevaUInt8
_ES_EscrevaUInt8:
    mov ebx, [_Nucleo]
    jmp dword [ebx+8]

global _ES_EscrevaUInt16
_ES_EscrevaUInt16:
    mov ebx, [_Nucleo]
    jmp dword [ebx+12]

global _ES_LeiaInt8
_ES_LeiaInt8:
    mov ebx, [_Nucleo]
    jmp dword [ebx+16]

global _ES_LeiaInt16
_ES_LeiaInt16:
    mov ebx, [_Nucleo]
    jmp dword [ebx+20]

section .bss
global _Nucleo
_Nucleo: resd 1