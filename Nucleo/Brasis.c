#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "Brasis.h"
#include "Debug.h"
#include "Memoria.h"

void Inicial
(
    BrasisParams_t * params, 
    size_t codigo_inicio, 
    size_t codigo_fim, 
    size_t dados_inicio, 
    size_t dados_fim
)
{
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

    // Inicia Gerenciador de Memória RAM
    Memoria(codigo_inicio, codigo_fim, dados_inicio, dados_fim, 
        params->ImagemInicio, params->ImagemInicio + params->ImagemTam, params->MemoriaBaixa,
        params->MemoriaMedia, params->MemoriaAlta, mem);

    Debug_Escreva("Memoria Livre: %u KiB\n", Memoria_Livre() / 1024);

    // Loop infinito
    while(true) { asm("hlt"); }
}