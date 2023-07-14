#ifndef STRING_H
#define STRING_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "Brasis.h"
#include "SisArq.h"

#define STRBUFFER_TAM 128

typedef struct StrBuffer_s
{
    Item_t * Item;
    uint8_t Buffer[STRBUFFER_TAM];
    size_t Posicao;
    size_t Tamanho;
} StrBuffer_t;

bool StrBuffer(Status_t * st, StrBuffer_t * str, Item_t * item);
bool StrBuffer_LeiaLinha(Status_t * st, StrBuffer_t * str, uint8_t * linha, size_t tam);
bool String_Igual(uint8_t * str1, uint8_t * str2, size_t tam);

#endif