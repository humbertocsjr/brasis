bits 32
cpu 386

global _ES_EscrevaUInt8
_ES_EscrevaUInt8:
    push ebp
    mov ebp, esp
    mov edx, [ebp+8]
    mov eax, [ebp+12]
    out dx, al
    pop ebp
    ret

global _ES_EscrevaUInt16
_ES_EscrevaUInt16:
    push ebp
    mov ebp, esp
    mov edx, [ebp+8]
    mov eax, [ebp+12]
    out dx, ax
    pop ebp
    ret

global _ES_LeiaUInt8
_ES_LeiaUInt8:
    push ebp
    mov ebp, esp
    mov edx, [ebp+8]
    xor eax, eax
    in al, dx
    pop ebp
    ret

global _ES_LeiaUInt16
_ES_LeiaUInt16:
    push ebp
    mov ebp, esp
    mov edx, [ebp+8]
    xor eax, eax
    in ax, dx
    pop ebp
    ret