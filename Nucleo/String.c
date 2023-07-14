#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "Brasis.h"
#include "String.h"
#include "Status.h"

bool String_Igual(uint8_t * str1, uint8_t * str2, size_t tam)
{
    for (size_t i = 0; i < tam; i++)
    {
        if(str1[i] != str2[i])
        {
            return false;
        }
        else if(str1[i] == 0) return true;
    }
    return true;
}

bool StrBuffer(Status_t * st, StrBuffer_t * str, Item_t * item)
{
    str->Item = item;
    Memoria_Zera(&str->Buffer[0], STRBUFFER_TAM);
    str->Posicao = 0;
    str->Tamanho = Leia(st, item, &str->Buffer[0], STRBUFFER_TAM);
    STATUS_RET(st);
}

bool StrBuffer_LeiaLinha(Status_t * st, StrBuffer_t * str, uint8_t * linha, size_t tam)
{
    uint8_t * buffer = &str->Buffer[0];
    size_t lido = 0;
    linha[lido] = 0;
    if(str->Tamanho == 0)
    {
        STATUS_ERRO(st, STATUS_ESTOURO_DE_CAPACIDADE);
        STATUS_RET(st);
    }
    while((buffer[str->Posicao] != 10) && (buffer[str->Posicao] != 0))
    {
        if((lido + 1) >= tam) break;
        if(str->Posicao >= str->Tamanho)
        {
            str->Tamanho = Leia(st, str->Item, buffer, STRBUFFER_TAM);
            str->Posicao = 0;
            if(str->Tamanho == 0) break;
        }
        linha[lido++] = buffer[str->Posicao++];
        linha[lido] = 0;
        if(buffer[str->Posicao] == 13)
        {
            str->Posicao++;
        }
        STATUS_OK(st);
    }
    if((buffer[str->Posicao] == 10))
    {
        str->Posicao++;
    }
    if((buffer[str->Posicao] == 0) && (lido == 0)) 
    {
        STATUS_ERRO(st, STATUS_ESTOURO_DE_CAPACIDADE);
    }
    STATUS_RET(st);
}