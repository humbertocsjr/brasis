//Sistema de Arquivos Unix Archive

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "Brasis.h"
#include "AR.h"
#include "SisArq.h"
#include "Semaforo.h"

#pragma pack(1)

SEMAFORO(_Semaforo);

typedef struct AR_Assinatura_s
{
    uint8_t Assinatura[8]; // !<arch>\n
} AR_Assinatura_t;

typedef struct AR_Arquivo_s
{
    uint8_t Nome[16];
    uint8_t UltimaModificacaoDecimal[12];
    uint8_t DonoIdDecimal[6];
    uint8_t GrupoIdDecimal[6];
    uint8_t ModoOctal[8];
    uint8_t TamanhoBytesDecimal[10];
    uint8_t CaracteresFinais[2];
} AR_Arquivo_t;

static bool _Abre(Status_t * st, struct SisArq_s * sisarq, struct Item_s * item)
{
    item->Status = ITEM_STATUS_ABERTO;
    item->Posicao = 0;
    item->SubPosicao = 0;
    STATUS_OK(st);
    STATUS_RET(st);
}

static bool _Desmontar(Status_t * st, struct SisArq_s * sisarq)
{
    STATUS_OK(st);
    STATUS_RET(st);
}

static bool _Fecha(Status_t * st, struct SisArq_s * sisarq, struct Item_s * item)
{
    item->Status = ITEM_STATUS_FECHADO;
    STATUS_OK(st);
    STATUS_RET(st);
}

static bool _Primeiro(Status_t * st, struct SisArq_s * sisarq, struct Item_s * item, struct Item_s * subItem)
{
    item->Posicao = sizeof(AR_Assinatura_t);
    return _Proximo(st, sisarq, item, subItem);
}

static bool _VaPara(Status_t * st, struct SisArq_s * sisarq, struct Item_s * item, size_t posicao)
{
    if(item->Bloco_Total <= posicao)
    {
        STATUS_ERRO(st, STATUS_ESTOURO_DE_CAPACIDADE);
        STATUS_RET(st);
    }
    item->Posicao = posicao;
}

static size_t _Leia(Status_t * st, struct SisArq_s * sisarq, struct Item_s * item, uint8_t * dados, size_t tam)
{
    Item_t *disp = sisarq->Dispositivo;
    Bloqueia(&_Semaforo);
    if(!disp->VaPara(st, sisarq, disp, item->Id + sizeof(AR_Arquivo_t) + item->Posicao))
    {
        Libera(&_Semaforo);
        return 0;
    } 
    if(tam >= (item->Bloco_Total - item->Posicao))
    {
        tam = (item->Bloco_Total - item->Posicao);
    }
    tam = disp->Leia(st, sisarq, disp, dados, tam);
    item->Posicao += tam;
    Libera(&_Semaforo);
    return tam;
}

static bool _Proximo(Status_t * st, struct SisArq_s * sisarq, struct Item_s * item, struct Item_s * subItem)
{
    size_t i;
    AR_Arquivo_t arq;
    Item_t *disp = sisarq->Dispositivo;

    if(!disp->VaPara(st, sisarq, disp, item->Posicao)) STATUS_RET(st);
    if(!disp->Leia(st, sisarq, disp, (uint8_t*)&arq, sizeof(AR_Arquivo_t))) STATUS_RET(st);
    subItem->Id = item->Posicao;
    subItem->Tipo = ITEM_TIPO_ARQUIVO;
    subItem->Status = ITEM_STATUS_FECHADO;
    subItem->Bloco_Tamanho = 1;
    subItem->Bloco_Total = 0;
    subItem->Abre = &_Abre;
    subItem->Leia = &_Leia;
    subItem->VaPara = &_VaPara;

    for (i = 0; i < 10; i++)
    {
        if(arq.TamanhoBytesDecimal[i] < '0' || arq.TamanhoBytesDecimal[i] > '9') break;
        subItem->Bloco_Total *= 10;
        subItem->Bloco_Total += arq.TamanhoBytesDecimal[i] - '0';
    }
    if(subItem->Bloco_Total == 0)
    {
        STATUS_ERRO(st, STATUS_NAO_ENCONTRADO);
        STATUS_RET(st);
    }

    item->Posicao = item->Posicao + sizeof(AR_Arquivo_t) + subItem->Bloco_Total;
    subItem->SisArq = sisarq;
    Memoria_Copia(subItem->Nome, arq.Nome, 16);

    for (i = 15; i > 0; i--)
    {
        if((subItem->Nome[i] == ' ') | (subItem->Nome[i] == 0))
        {
            subItem->Nome[i] = 0;
        }
        else break;
    }
    STATUS_OK(st);
    STATUS_RET(st);
    
}

static bool _Raiz(Status_t * st, struct SisArq_s * sisarq, Item_t * item)
{
    Item(item);
    item->Status = ITEM_STATUS_FECHADO;
    item->Tipo = ITEM_TIPO_DIRETORIO;
    item->Abre = &_Abre;
    item->Primeiro = &_Primeiro;
    item->Proximo = &_Proximo;
    item->Fecha = &_Fecha;
    STATUS_OK(st);
    STATUS_RET(st);
}

void AR(SisArq_t * sisarq, uint8_t *nome, Item_t * dispositivo)
{
    SisArq(sisarq);
    Memoria_Copia(sisarq->Nome, nome, ITEM_NOME_TAM);
    sisarq->Dispositivo = dispositivo;
    sisarq->Visivel = true;
    sisarq->Protocolo = false;
    sisarq->Desmontar = &_Desmontar;
    sisarq->Raiz = &_Raiz;
}