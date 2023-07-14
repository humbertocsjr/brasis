#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "Brasis.h"
#include "Status.h"
#include "SisArq.h"
#include "Memoria.h"
#include "Debug.h"
#include "ES.h"
#include "./Modulos/Incluir/Nucleo.h"

static Nucleo_t _Nucleo =
{
    VERSAO,
    SUBVERSAO,
    &Debug_Escreva,
    &ES_EscrevaUInt8,
    &ES_EscrevaUInt16,
    &ES_LeiaUInt8,
    &ES_LeiaUInt16,
    0,//&Dispositivo_Registra,
    0,//&Dispositivo_Desregistra,
    &Item,
    &SisArq,
    &SisArq_Registra,
    0,//&SisArq_Desregistra,
    &Abre,
    0,//&Fecha,
    &VaPara,
    &Leia,
    0,//&Escreva,
    0,//&Diretorio_Primeiro,
    0,//&Diretorio_Proximo
};

// Executavel AOUT
// 32 Bytes de cabecalho
// Sessões alinhadas em 4KiB conforme descrito no cabecalho

#pragma pack(1)

#define AOUT_ASSINATURA_OMAGIC 0x107

typedef struct AOUT_Cabecalho_s
{
    uint32_t Assinatura;         //07 01 64 00
    uint32_t TextoTamanho;       //00 10 00 00
    uint32_t DadosTamanho;       //00 10 00 00
    uint32_t BssTamanho;         //00 10 00 00
    uint32_t SimbolosTamanho;    //00 00 00 00
    uint32_t Entrada;            //00 00 00 00
    uint32_t TabelaTextoTamanho; //40 00 00 00
    uint32_t TabelaDadosTamanho; //08 00 00 00
} AOUT_Cabecalho_t;

#define AOUT_MASCARA_ID_SIMBOLO        0x00ffffff
#define AOUT_MASCARA_PTR_RELATIVO      0x01000000
#define AOUT_MASCARA_TAMANHO           0x06000000
#define AOUT_MASCARA_TAMANHO_SHR       25
#define AOUT_MASCARA_EXTERNO           0x08000000
#define AOUT_MASCARA_REALOC_DESVIO_GOT 0x10000000
#define AOUT_MASCARA_REALOC_DESVIO_PLT 0x20000000
#define AOUT_MASCARA_REALOC_RELATIVO   0x40000000
#define AOUT_MASCARA_REALOC_COPIA      0x80000000

typedef struct AOUT_Realocacao_s
{
    int32_t Endereco;
    uint32_t Mascara;
}AOUT_Realocacao_t;

void AOUT_ExecutaModulo(size_t ptrExec)
{
    asm("pushad");
    asm("mov eax, [ebp+8]");
    asm("mov esi, __Nucleo");
    asm("call eax");
    asm("popad");
}


bool Executavel_CarregarModulo(Status_t * st, Item_t * executavel, uint8_t processo)
{
    AOUT_Cabecalho_t aout;
    size_t ptrTexto;
    size_t ptrDados = 0;
    size_t ptrBss = 0;
    size_t ptrSimbolo = 0;
    size_t ptrTabelaTexto = 0;
    size_t ptrTabelaDados = 0;
    size_t tamTotal = 0;
    size_t tamTotalPaginas = 0;
    STATUS_OK(st);
    // Le o cabecalho
    VaPara(st, executavel, 0);
    Leia(st, executavel, &aout, sizeof(AOUT_Cabecalho_t));
    // Aloca memoria para o aplicativo
    tamTotal = aout.TextoTamanho + aout.DadosTamanho + aout.BssTamanho;
    tamTotalPaginas = tamTotal / 4096 + (tamTotal % 4096 ? 1 : 0);
    ptrTexto = AlocaPaginaFisica(processo, false, tamTotalPaginas);
    ptrDados = ptrTexto + aout.TextoTamanho;
    ptrBss = ptrDados + aout.DadosTamanho;
    if(aout.SimbolosTamanho) ptrSimbolo = AlocaPaginaFisica(processo, false, aout.SimbolosTamanho / 4096 + (aout.SimbolosTamanho % 4096 ? 1 : 0));
    if(aout.TabelaTextoTamanho) ptrTabelaTexto = AlocaPaginaFisica(processo, false, aout.TabelaTextoTamanho / 4096 + (aout.TabelaTextoTamanho % 4096 ? 1 : 0));
    if(aout.TabelaDadosTamanho) ptrTabelaDados = AlocaPaginaFisica(processo, false, aout.TabelaDadosTamanho / 4096 + (aout.TabelaTextoTamanho % 4096 ? 1 : 0));
    // Le arquivo
    Leia(st, executavel, ptrTexto, aout.TextoTamanho + aout.DadosTamanho);
    if(aout.SimbolosTamanho) Leia(st, executavel, ptrSimbolo, aout.SimbolosTamanho);
    if(aout.TabelaTextoTamanho) Leia(st, executavel, ptrTabelaTexto, aout.TabelaTextoTamanho);
    if(aout.TabelaDadosTamanho) Leia(st, executavel, ptrTabelaDados, aout.TabelaDadosTamanho);
    
    // Processa arquivo
    AOUT_Realocacao_t * tbl;
    uint8_t * ptr;

    if(aout.TabelaTextoTamanho)
    {
        ptr = (uint8_t *)ptrTexto;
        tbl = ptrTabelaTexto;
        for (size_t i = 0; i < (aout.TabelaTextoTamanho / sizeof(AOUT_Realocacao_t)); i++)
        {
            uint32_t tmp = 0;
            tmp = ptr[tbl->Endereco]
                | (ptr[tbl->Endereco+1] << 8)
                | (ptr[tbl->Endereco+2] << 16)
                | (ptr[tbl->Endereco+3] << 24);
            if(((tbl->Mascara & AOUT_MASCARA_TAMANHO) >> AOUT_MASCARA_TAMANHO_SHR) == 1)
            {
                tmp &= 0xffff;
            }
            if(tbl->Mascara & AOUT_MASCARA_PTR_RELATIVO)
            {
                tmp += tbl->Endereco;
            }

            tmp += ptrTexto;

            ptr[tbl->Endereco] = tmp & 0xff;
            ptr[tbl->Endereco+1] = (tmp >> 8) & 0xff;
            if(((tbl->Mascara & AOUT_MASCARA_TAMANHO)>>AOUT_MASCARA_TAMANHO_SHR) == 2)
            {
                ptr[tbl->Endereco+2] = (tmp >> 16) & 0xff;
                ptr[tbl->Endereco+3] = (tmp >> 24) & 0xff;
            }
            tbl++;
        }
        
    }
    if(aout.TabelaDadosTamanho)
    {
        ptr = (uint8_t *)ptrDados;
        tbl = ptrTabelaDados;
        for (size_t i = 0; i < (aout.TabelaTextoTamanho / sizeof(AOUT_Realocacao_t)); i++)
        {
    
            uint32_t tmp = 0;
            tmp = ptr[tbl->Endereco]
                | (ptr[tbl->Endereco+1] << 8)
                | (ptr[tbl->Endereco+2] << 16)
                | (ptr[tbl->Endereco+3] << 24);
            if(((tbl->Mascara & AOUT_MASCARA_TAMANHO) >> AOUT_MASCARA_TAMANHO_SHR) == 1)
            {
                tmp &= 0xffff;
            }
            if(tbl->Mascara & AOUT_MASCARA_PTR_RELATIVO)
            {
                tmp += tbl->Endereco;
            }

            tmp += ptrTexto;

            ptr[tbl->Endereco] = tmp & 0xff;
            ptr[tbl->Endereco+1] = (tmp >> 8) & 0xff;
            if(((tbl->Mascara & AOUT_MASCARA_TAMANHO)>>AOUT_MASCARA_TAMANHO_SHR) == 2)
            {
                ptr[tbl->Endereco+2] = (tmp >> 16) & 0xff;
                ptr[tbl->Endereco+3] = (tmp >> 24) & 0xff;
            }
            tbl++;
        }
        
    }
    // Liberando dados extras
    if(aout.SimbolosTamanho) LiberaPaginaFisica(ptrSimbolo);
    if(aout.TabelaTextoTamanho) LiberaPaginaFisica(ptrTabelaTexto);
    if(aout.TabelaDadosTamanho) LiberaPaginaFisica(ptrTabelaDados);
    
    // Executa modulo
    STATUS_OK(st);
    AOUT_ExecutaModulo(aout.Entrada + ptrTexto);
    STATUS_RET(st);
}