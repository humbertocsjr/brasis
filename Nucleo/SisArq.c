#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "Brasis.h"
#include "Status.h"
#include "SisArq.h"
#include "String.h"
#include "Memoria.h"

#define SISARQ_QTD 64

static SisArq_t * _SisArq[SISARQ_QTD];

void Item(Item_t * item)
{
    item->Status = ITEM_STATUS_NENHUM;
    item->Nome[0] = 0;
    item->SisArq = 0;
    item->Tipo = ITEM_TIPO_NENHUM;
    item->Id = 0;
    item->Posicao = 0;
    item->SubPosicao = 0;
    item->Dados = 0;
    item->Bloco_Tamanho = 0;
    item->Bloco_Total = 0;
    item->FimDoArquivo = false;
    item->Abre = 0;
    item->VaPara = 0;
    item->Leia = 0;
    item->Fecha = 0;
    item->Primeiro = 0;
    item->Proximo = 0;
    for (size_t i = 0; i < ITEM_CONFIG_QTD; i++)
    {
        item->Config[i] = 0;
    }
}

void SisArq(SisArq_t * sisarq)
{
    sisarq->Nome[0] = 0;
    sisarq->Visivel = false;
    sisarq->Protocolo = false;
    sisarq->Dispositivo = 0;
    sisarq->Desmontar = 0;
    sisarq->Raiz = 0;
    sisarq->AbreEndereco = 0;
}

void SisArq_Inicializa()
{
    for (size_t i = 0; i < SISARQ_QTD; i++)
    {
        _SisArq[i] = 0;
    }
}

bool SisArq_Registra(Status_t * st, SisArq_t * sisarq)
{
    for (size_t i = 0; i < SISARQ_QTD; i++)
    {
        if(_SisArq[i] == 0)
        {
            STATUS_OK(st);
            _SisArq[i] = sisarq;
            STATUS_RET(st);
        }
    }
    STATUS_ERRO(st, STATUS_ESTOURO_DE_CAPACIDADE);
    STATUS_RET(st);
}

bool Abre(Status_t * st, Item_t * item, uint8_t *endereco)
{
    Item_t acima;
    Item_t atual;
    size_t i, j, k;
    bool ok;
    SisArq_t * sisarq = 0;
    uint8_t trecho[ITEM_NOME_TAM + 1];
    j = 0;
    i = 0;
    while(endereco[i] != 0 && endereco[i] != ':' && j < ITEM_NOME_TAM)
    {
        trecho[j] = endereco[i];
        j++;
        i++;
    }
    trecho[j] = 0;
    if(endereco[i] == 0 || endereco[i] != ':' || endereco[i+1] != '/' || endereco[i+2] != '/')
    {
        Debug_Escreva("[Abre: %s: Disco nao informado, falta separador '://' apos o '%s'[%u]]", endereco, trecho, j);
        STATUS_ERRO(st, STATUS_ENDERECO_INVALIDO);
        STATUS_RET(st);
    }
    else i += 2; // Deixa a barra para o loop
    for ( k = 0; k < SISARQ_QTD; k++)
    {
        if(_SisArq[k] != 0)
        {
            if(String_Igual(_SisArq[k]->Nome, trecho, ITEM_NOME_TAM))
            {
                sisarq = _SisArq[k];
                break;
            }
        }
    }
    if(sisarq == 0)
    {
        Debug_Escreva("[Abre: %s: Disco nao encontrado]", endereco);
        STATUS_ERRO(st, STATUS_NAO_ENCONTRADO);
        STATUS_RET(st);
    }
    if(endereco[i] == 0)
    {
        Debug_Escreva("[Abre: %s: Disco nao informado]", endereco);
        STATUS_ERRO(st, STATUS_ENDERECO_INVALIDO);
        STATUS_RET(st);
    }
    if(sisarq->Protocolo)
    {
        return sisarq->AbreEndereco(st, sisarq, item, endereco);
    }
    else
    {
        if(!sisarq->Raiz(st, sisarq, &acima))
        {
            Debug_Escreva("[Abre: %s: Erro ao carregar a raiz do disco]", endereco);
            STATUS_RET(st);
        }
    }
    while(endereco[i] == '/')
    {
        if((acima.Tipo & ITEM_TIPO_DIRETORIO) == 0)
        {
            Debug_Escreva("[Abre: %s: Esperado diretorio]", endereco);
            STATUS_ERRO(st, STATUS_NAO_ENCONTRADO);
            STATUS_RET(st);
        }
        i++;
        j = 0;
        while(endereco[i] != 0 && endereco[i] != '/' && j < ITEM_NOME_TAM)
        {
            trecho[j] = endereco[i];
            j++;
            i++;
        }
        trecho[j] = 0;
        if(j > 0)
        {
            ok = acima.Primeiro(st, sisarq, &acima, &atual);
            while(ok)
            {
                if(String_Igual(atual.Nome, trecho, ITEM_NOME_TAM))
                {
                    Memoria_Copia(&acima, &atual, sizeof(Item_t));
                    Memoria_Copia(item, &atual, sizeof(Item_t));
                    break;
                }
                ok = acima.Proximo(st, sisarq, &acima, &atual);
            }
            if(!ok)
            {
                STATUS_ERRO(st, STATUS_NAO_ENCONTRADO);
                STATUS_RET(st);
            }
        }
    }
    if(atual.Abre != 0)
        atual.Abre(st, sisarq, &item);
    else
        STATUS_ERRO(st, STATUS_NAO_IMPLEMENTADO);
    STATUS_RET(st);
}

bool VaPara(Status_t * st, Item_t * item, size_t posicao)
{
    if(item->VaPara == 0)
    {
        STATUS_ERRO(st, STATUS_NAO_IMPLEMENTADO);
        STATUS_RET(st);
    }
    return item->VaPara(st, item->SisArq, item, posicao);
}

size_t Leia(Status_t * st, Item_t * item, uint8_t * dados, size_t tam)
{
    if(item->Leia == 0)
    {
        STATUS_ERRO(st, STATUS_NAO_IMPLEMENTADO);
        return 0;
    }
    return item->Leia(st, item->SisArq, item, dados, tam);
}