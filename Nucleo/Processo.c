#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "Brasis.h"
#include "Status.h"
#include "SisArq.h"
#include "Memoria.h"
#include "Processo.h"

static Processo_t _Processo[PROCESSO_QTD];
static Processo_t * _Processo_Atual;

void Processo()
{
    for (size_t i = 0; i < PROCESSO_QTD; i++)
    {
        _Processo[i].Id = i;
        if(i == 0 || i == PROCESSO_RESERVADO)
        {
            _Processo[i].Status = PROCESSO_STATUS_INVALIDO;
        }
        else if(i == PROCESSO_NUCLEO)
        {
            _Processo[i].Status = PROCESSO_STATUS_ATIVO;
            _Processo_Atual = &_Processo[i];
        }
        else
        {
            _Processo[i].Status = PROCESSO_STATUS_VAZIO;
        }
    }
    
}