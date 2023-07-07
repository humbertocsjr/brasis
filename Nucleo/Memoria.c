#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "Brasis.h"

/*
 Este gerenciador de memória gerencia localmente os primeiros 16 MiB de RAM
*/

// 4096 Páginas de 4096 Bytes totalizando 16 MiB
#define MEMORIA_PAGINASBAIXAS 4096
#define MEMORIA_PAGINASBAIXAS_B 4096 / 32

static uint32_t _Memoria_PaginasBaixas[MEMORIA_PAGINASBAIXAS_B]; // Até 16 MiB


static void _Memoria_PB_EscrevaPagina(size_t pagina, bool valor)
{
    if(pagina >= MEMORIA_PAGINASBAIXAS) return;

    if(valor)
        _Memoria_PaginasBaixas[pagina / 32] |= 1 << (pagina % 32);
    else
        _Memoria_PaginasBaixas[pagina / 32] &= ~(1 << (pagina % 32));
}

static bool _Memoria_PB_LeiaPagina(size_t pagina)
{
    if(pagina >= MEMORIA_PAGINASBAIXAS) return false;

    return
        (_Memoria_PaginasBaixas[pagina / 32] & (1 << (pagina % 32))) ? true : false;
}

static void _Memoria_PB_EscrevaEntre(size_t inicio, size_t fim, bool valor)
{
    inicio /= 4096;
    fim /= 4096 + ((fim % 4096) > 0 ? 1 : 0);
    for (size_t i = inicio; i <= fim; i++)
    {
        _Memoria_PB_EscrevaPagina(i, true);
    }
}


static size_t _Memoria_PB_Soma(bool valor)
{
    size_t ret = 0;
    for (size_t i = 0; i <= MEMORIA_PAGINASBAIXAS_B; i++)
    {
        if(_Memoria_PaginasBaixas[i] == (valor ? -1 : 0))
        {
            ret += 32;
        }
        else
        {
            for (size_t j = 0; j < 32; j++)
            {
                if(_Memoria_PB_LeiaPagina(j + (i * 32)) == (valor ? true : false)) ret++;
            }
        }
    }
    return ret;
}

void Memoria
(
    size_t codigo_inicio, 
    size_t codigo_fim, 
    size_t dados_inicio, 
    size_t dados_fim,
    size_t imagem_inicio,
    size_t imagem_fim,
    size_t mem_baixa,
    size_t mem_media,
    size_t mem_alta,
    size_t mem_max
)
{
    size_t i;
    for ( i = 0; i < MEMORIA_PAGINASBAIXAS_B; i++)
    {
        _Memoria_PaginasBaixas[i] = 0;
    }
    _Memoria_PB_EscrevaEntre(0, 64*1024, true);
    _Memoria_PB_EscrevaEntre(codigo_inicio, codigo_fim, true);
    _Memoria_PB_EscrevaEntre(dados_inicio, dados_fim, true);
    _Memoria_PB_EscrevaEntre(imagem_inicio, imagem_fim, true);
    _Memoria_PB_EscrevaEntre(mem_baixa, 1024*1024 - 1, true);
    _Memoria_PB_EscrevaEntre(mem_media, 16*1024*1024 - 1, true);
    _Memoria_PB_EscrevaEntre(15*1024*1024, 16*1024*1024 - 1, true);
}

size_t Memoria_Livre()
{
    return _Memoria_PB_Soma(false) * 4096;
}
