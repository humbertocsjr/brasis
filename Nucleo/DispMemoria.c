// Dispositivo Memoria Mapeada
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "Brasis.h"
#include "SisArq.h"
#include "Status.h"
#include "Memoria.h"

#define DISPMEM_CONFIG_POS 0
#define DISPMEM_CONFIG_TAM 1

static bool _VaPara(Status_t * st, struct SisArq_s * sisarq, struct Item_s * item, size_t posicao)
{
    size_t inicio = item->Config[DISPMEM_CONFIG_POS] + posicao;
    size_t fim = item->Config[DISPMEM_CONFIG_POS] + item->Config[DISPMEM_CONFIG_TAM];
    if(inicio >= fim)
    {
        STATUS_ERRO(st, STATUS_ESTOURO_DE_CAPACIDADE);
        item->FimDoArquivo = true;
        STATUS_RET(st);
    }
    item->Posicao = posicao;
    STATUS_OK(st);
    STATUS_RET(st);
}

static size_t _Leia(Status_t * st, struct SisArq_s * sisarq, Item_t * item, uint8_t * dados, size_t tam)
{
    size_t inicio = item->Config[DISPMEM_CONFIG_POS] + item->Posicao;
    size_t fim = item->Config[DISPMEM_CONFIG_POS] + item->Config[DISPMEM_CONFIG_TAM];
    if(inicio >= fim)
    {
        STATUS_ERRO(st, STATUS_ESTOURO_DE_CAPACIDADE);
        item->FimDoArquivo = true;
        return 0;
    }
    if((fim - inicio) < tam)
    {
        tam = fim-inicio;
    }
    if(tam > 0)
    {
        Memoria_Copia(dados, inicio, tam);
    }
    else
    {
        STATUS_ERRO(st, STATUS_ESTOURO_DE_CAPACIDADE);
        item->FimDoArquivo = true;
        return 0;
    }
    STATUS_OK(st);
    return tam;
}

void DispMemoria(Item_t * item, size_t endereco, size_t tamanho)
{
    Item(item);
    item->Tipo = ITEM_TIPO_ARQUIVO | ITEM_TIPO_DISP_CARACTERE;
    item->Status = ITEM_STATUS_ABERTO;
    item->Config[DISPMEM_CONFIG_POS] = endereco;
    item->Config[DISPMEM_CONFIG_TAM] = tamanho;
    item->Leia = &_Leia;
    item->VaPara = &_VaPara;


}