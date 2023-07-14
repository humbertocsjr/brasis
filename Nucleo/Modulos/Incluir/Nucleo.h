#ifndef NUCLEO_H
#define NUCLEO_H


#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "../../Incluir/Status.h"
#include "../../Incluir/SisArq.h"

#pragma pack(1)


typedef struct Nucleo_e
{
    /// @brief Versão da Interface do Nucleo
    uint16_t Versao;

    /// @brief Sub Versão da Interface do Nucleo
    uint16_t SubVersao;

    /// @brief Escreve na saida de Debug
    /// @param fmt Texto de saida com formatação 
    void (*Debug_Escreva)(uint8_t * fmt,...);

    /// @brief Escreve UInt8 em uma Porta
    /// @param porta Porta de E/S
    /// @param valor Valor
    void (*ES_EscrevaUInt8)(uint16_t porta, uint8_t valor);

    /// @brief Escreve UInt16 em uma Porta
    /// @param porta Porta de E/S
    /// @param valor Valor
    void (*ES_EscrevaUInt16)(uint16_t porta, uint16_t valor);

    /// @brief Leia UInt8 de uma Porta
    /// @param porta Porta de E/S
    uint8_t (*ES_LeiaUInt8)(uint16_t porta);

    /// @brief Leia UInt16 de uma Porta
    /// @param porta Porta de E/S
    uint16_t (*ES_LeiaUInt16)(uint16_t porta);

    /// @brief Registra um Dispositivo
    /// @param st Status da ação
    /// @param disp Dispositivo
    bool (*Dispositivo_Registra)(Status_t * st, Item_t * disp);

    /// @brief Desregistra um Dispositivo
    /// @param st Status da ação
    /// @param disp Dispositivo
    bool (*Dispositivo_Desregistra)(Status_t * st, Item_t * disp);

    /// @brief Inicializa o objeto Item_t
    /// @param item Objeto a ser inicializado
    void (*Item)(Item_t * item);

    /// @brief Inicializa o objeto SisArq_t
    /// @param sisarq Objeto a ser inicializado
    void (*SisArq)(SisArq_t * sisarq);

    /// @brief Registra um SisArq
    /// @param st Status da acao
    /// @param sisarq Sistema de Arquivos
    /// @return 
    bool (*SisArq_Registra)(Status_t * st, SisArq_t * sisarq);

    /// @brief Remove o registro de um SisArq
    /// @param st Status da acao
    /// @param sisarq Sistema de Arquivos
    /// @return 
    bool (*SisArq_Desregistra)(Status_t * st, SisArq_t * sisarq);

    /// @brief Abre um item(Arquivo/Diretorio)
    /// @param st Status da acao
    /// @param item Item destino
    /// @param endereco Endereço do item (URL)
    /// @return 
    bool (*Abre)(Status_t * st, Item_t * item, uint8_t *endereco);

    /// @brief Fecha um item aberto
    /// @param st Status da acao
    /// @param item Item que deve ser fechado
    /// @return 
    bool (*Fecha)(Status_t * st, Item_t * item);

    /// @brief Va para uma posicao no item
    /// @param st Status da acao
    /// @param item Item de onde sera lido
    /// @param posicao Posicao
    /// @return 
    bool (*VaPara)(Status_t * st, Item_t * item, size_t posicao);

    /// @brief Le um bloco de dados de um item
    /// @param st Status da acao
    /// @param item Item de onde sera lido
    /// @param dados Destino
    /// @param tam Tamanho do Destino
    /// @return Tamanho Lido
    size_t (*Leia)(Status_t * st, Item_t * item, uint8_t * dados, size_t tam);

    /// @brief Escreve um bloco de dados de um item
    /// @param st Status da acao
    /// @param item Item de onde sera lido
    /// @param dados Origem
    /// @param tam Tamanho da Origem
    /// @return Tamanho Escrito
    size_t (*Escreva)(Status_t * st, Item_t * item, uint8_t * dados, size_t tam);

    /// @brief Carrega o primeiro sub item de um diretorio
    /// @param st Status da acao
    /// @param item Item do Diretorio
    /// @param subItem Sub item retorno
    /// @return 
    bool (*Diretorio_Primeiro)(Status_t * st, Item_t * item, Item_t * subItem);

    /// @brief Carrega o proximo sub item de um diretorio
    /// @param st Status da acao
    /// @param item Item do diretorio
    /// @param subItem Sub item retorno
    /// @return 
    bool (*Diretorio_Proximo)(Status_t * st, Item_t * item, Item_t * subItem);

} Nucleo_t;

#endif