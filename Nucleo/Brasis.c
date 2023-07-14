#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "Brasis.h"
#include "Debug.h"
#include "Memoria.h"
#include "Processador.h"
#include "SisArq.h"
#include "AR.h"
#include "Processo.h"
#include "String.h"

static Item_t _Inicial_Disp;
static SisArq_t _Inicial_SisArq;


static StrBuffer_t _Inicial_Buffer;

/// @brief Inicia o sistema operaciona
/// @param params Parametros do Sistema de Inicializacao
/// @param codigo_inicio Endereco do inicio do codigo do nucleo
/// @param codigo_fim  Endereco do fim do codigo do nucleo
/// @param dados_inicio Endereco do inicio dos dados do nucleo
/// @param dados_fim Endereco do fim dos dados do nucleo
void Inicial
(
    BrasisParams_t * params, 
    size_t codigo_inicio, 
    size_t codigo_fim, 
    size_t dados_inicio, 
    size_t dados_fim
)
{
    Status_t st;
    Item_t cfg;
    Item_t modulo;
    uint8_t moduloNome[256];
    // Calcula o tamanho da memória RAM
    size_t mem = params->MemoriaAlta;
    if(mem < params->MemoriaMedia) mem = params->MemoriaMedia;
    if(mem < params->MemoriaBaixa) mem = params->MemoriaBaixa;

    // Inicia o sistema de Depuração baseado em VideoTexto
    Debug_ConfigVideoTexto();

    // Inicia o BRASIS
    Debug_Escreva("BRASIS v%u.%u R%u [ RAM %u KiB ]\n", VERSAO, SUBVERSAO, REVISAO, mem / 1024);

    Debug_Escreva("Copyright (c) 2023, Humberto Costa dos Santos Junior\n\n");
    Debug_Escreva("Nucleo: Codigo: %u KiB Dados: %u KiB \n", 
        (codigo_fim - codigo_inicio) / 1024, 
        (dados_fim - dados_inicio) / 1024);

    // Inicia o Processador
    Processador();

    // Inicia Gerenciador de Memória RAM
    Memoria(codigo_inicio, codigo_fim, dados_inicio, dados_fim, 
        params->ImagemInicio, params->ImagemInicio + params->ImagemTam, params->MemoriaBaixa,
        params->MemoriaMedia, params->MemoriaAlta, mem);
    
    // Inicia Gerenciador de Processos
    Processo();

    Debug_Escreva("Memoria Livre: %u KiB\n", Memoria_Livre() / 1024);

    // Inicializa Gerenciador de Sistema de Arquivos
    SisArq_Inicializa();

    // Inicializa Sistema de Arquivos Inicial
    DispMemoria(&_Inicial_Disp, params->ImagemInicio, params->ImagemTam);
    AR(&_Inicial_SisArq, "inicial", &_Inicial_Disp);
    SisArq_Registra(&st, &_Inicial_SisArq);


    if(Abre(&st, &cfg, "inicial://Modulos.cfg"))
    {
        Debug_Escreva("Carregando Modulos [%s]\n\n", cfg.Nome);
        if(StrBuffer(&st, &_Inicial_Buffer, &cfg))
        {
            while(StrBuffer_LeiaLinha(&st, &_Inicial_Buffer, &moduloNome, 256))
            {
                Debug_Escreva(" - %s ", &moduloNome);
                if(Abre(&st, &modulo, &moduloNome))
                {
                    if(Executavel_CarregarModulo(&st, &modulo, PROCESSO_NUCLEO))
                    {
                        Debug_Escreva("[ OK ]\n");
                    }
                    else
                    {
                        Debug_Escreva("[ Erro %u ]\n", st.Tipo);
                    }
                }
                else
                {
                    Debug_Escreva("[ Arquivo nao encontrado (%u) ]\n", st.Tipo);
                }
            }
        }
    }
    else
    {
        Debug_Escreva("Arquivo de configuracoes de Modulos nao encontrado [%u]", st.Tipo);
    }

    // Loop infinito
    while(true) { asm("hlt"); }
}