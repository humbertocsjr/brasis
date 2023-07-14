#ifndef PROCESSO_H
#define PROCESSO_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define PROCESSO_QTD 128
#define PROCESSO_VAZIO 0
#define PROCESSO_NUCLEO 1
#define PROCESSO_RESERVADO 127

#define PROCESSO_STATUS_VAZIO 0
#define PROCESSO_STATUS_ATIVO 1
#define PROCESSO_STATUS_SUSPENSO 2
#define PROCESSO_STATUS_INVALIDO 255

#pragma pack(1)

typedef struct Processo_s
{
    uint8_t Id;
    uint8_t Status;
    size_t PtrExecucaoInicial;
    size_t PtrTopoPilha;
} Processo_t;

void Processo();

#endif