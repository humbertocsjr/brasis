#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "Brasis.h"
#include "Processo.h"

// Este gerenciador de memória gerencia localmente os primeiros 16 MiB de RAM

// 4096 Páginas de 4096 Bytes totalizando 16 MiB
#define MEMORIA_PAGINASBAIXAS 4096

static uint8_t _Memoria_PaginasBaixas[MEMORIA_PAGINASBAIXAS]; // Até 16 MiB


static void _Memoria_PB_EscrevaEntre(size_t inicio, size_t fim, uint8_t valor)
{
    inicio /= 4096;
    fim /= 4096 + ((fim % 4096) > 0 ? 1 : 0);
    for (size_t i = inicio; i <= fim; i++)
    {
        _Memoria_PaginasBaixas[i] = valor;
    }
}

static size_t _Memoria_PB_SomaEmUso(bool em_uso)
{
    size_t ret = 0;
    for (size_t i = 0; i <= MEMORIA_PAGINASBAIXAS; i++)
    {
        if((em_uso ? _Memoria_PaginasBaixas[i] != 0 : _Memoria_PaginasBaixas[i] == 0)) ret++;
    }
    return ret;
}


static size_t _Memoria_PB_Soma(uint8_t processo)
{
    size_t ret = 0;
    for (size_t i = 0; i <= MEMORIA_PAGINASBAIXAS; i++)
    {
        if((_Memoria_PaginasBaixas[i] & 0x7f) == (processo & 0x7f)) ret++;
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
    for ( i = 0; i < MEMORIA_PAGINASBAIXAS; i++)
    {
        _Memoria_PaginasBaixas[i] = PROCESSO_VAZIO;
    }
    _Memoria_PB_EscrevaEntre(0, 64*1024, PROCESSO_RESERVADO);
    _Memoria_PB_EscrevaEntre(codigo_inicio, codigo_fim, PROCESSO_NUCLEO);
    _Memoria_PB_EscrevaEntre(dados_inicio, dados_fim, PROCESSO_NUCLEO);
    _Memoria_PB_EscrevaEntre(imagem_inicio, imagem_fim, PROCESSO_NUCLEO);
    _Memoria_PB_EscrevaEntre(mem_baixa, 1024*1024 - 1, PROCESSO_RESERVADO);
    _Memoria_PB_EscrevaEntre(mem_media, 16*1024*1024 - 1, PROCESSO_RESERVADO);
    _Memoria_PB_EscrevaEntre(15*1024*1024, 16*1024*1024 - 1, PROCESSO_RESERVADO);
}

size_t Memoria_Livre()
{
    return _Memoria_PB_SomaEmUso(false) * 4096;
}

size_t Memoria_Ocupado()
{
    return _Memoria_PB_SomaEmUso(true) * 4096;
}

size_t Memoria_Processo(uint8_t processo)
{
    return _Memoria_PB_Soma(processo) * 4096;
}

size_t AlocaPaginaFisica(uint8_t processo, bool especialEquipamento, size_t qtd)
{
    size_t primeiro = 0;
    size_t ultimo = 0;
    size_t encontrado = 0;
    bool validando = false;
    size_t min = 256;
    size_t max = MEMORIA_PAGINASBAIXAS;
    if(especialEquipamento)
    {
        min = 0;
        max = 255;
    }
    for (size_t i = min; i <= max; i++)
    {
        if(_Memoria_PaginasBaixas[i] == PROCESSO_VAZIO)
        {
            if(validando)
            {
                ultimo = i;
                encontrado++;
            }
            else
            {
                encontrado = 1;
                primeiro = i;
                ultimo = primeiro;
                validando = true;
            }
            if(validando && (qtd <= encontrado))
            {
                for (size_t j = primeiro; j <= ultimo; j++)
                {
                    _Memoria_PaginasBaixas[j] = (j == primeiro) ? (processo | 0x80) : (processo & 0x7f);
                    Memoria_Zera(j * 4096, 4096);
                }
                return primeiro * 4096;
            }
        }
        else
        {
            validando = false;
        }
    }
    return 0;
}

void LiberaPaginaFisica(size_t endereco)
{
    size_t pagina = endereco / 4096;
    uint8_t processo = 0;
    for (size_t i = pagina; i < MEMORIA_PAGINASBAIXAS; i++)
    {
        if(i == pagina)
        {
            processo = _Memoria_PaginasBaixas[i] & 0x7f;
            _Memoria_PaginasBaixas[i] = 0;
            if((_Memoria_PaginasBaixas[i] & 0x80) == PROCESSO_VAZIO) return;
        }
        else
        {
            if
            (
                ((_Memoria_PaginasBaixas[i] & 0x80) == 0x80) ||
                ((_Memoria_PaginasBaixas[i] & 0x7f) != processo)
            ) return;
            _Memoria_PaginasBaixas[i] = 0;
        }
    }
    
}

void Memoria_Copia(uint8_t * destino, uint8_t * origem, size_t tam)
{
    asm("mov edi, [ebp+8]");
    asm("mov esi, [ebp+12]");
    asm("mov ecx, [ebp+16]");
    asm("cld");
    asm("rep movsb");
}

void Memoria_Zera(uint8_t * destino, size_t tam)
{
    asm("mov edi, [ebp+8]");
    asm("mov ecx, [ebp+12]");
    asm("xor eax, eax");
    asm("cld");
    asm("rep stosb");
}