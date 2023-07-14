#ifndef SISARQ_H
#define SISARQ_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "Status.h"

// Formato de endereço: SISARQ://DIR OU ENDERECO/DIR/ARQUIVO
// SISARQ:
//  - Pode ser um disco, neste caso pode usar o nome do dispositivo (Ex.: ide0) ou o nome do volume
//  - Pode ser um protocolo, neste caso usa o próprio nome do protocolo (Ex.: http)
//  - Nos casos de protocolo, o primeiro item do endereço deverá ser um endereço remoto

#define ITEM_NOME_TAM 32

#define ITEM_TIPO_NENHUM 0
#define ITEM_TIPO_ARQUIVO 1
#define ITEM_TIPO_DIRETORIO 2
#define ITEM_TIPO_DISP_CARACTERE 4
#define ITEM_TIPO_DISP_BLOCO 8

#define ITEM_STATUS_NENHUM 0
#define ITEM_STATUS_FECHADO 1
#define ITEM_STATUS_ABERTO 2

#define ITEM_CONFIG_QTD 16

typedef struct Item_s
{
    uint8_t Status;
    uint8_t Nome[ITEM_NOME_TAM+1];
    struct SisArq_s * SisArq;
    uint8_t Tipo;
    size_t Id;
    size_t Posicao;
    size_t SubPosicao;
    uint8_t * Dados;
    size_t Bloco_Tamanho;
    size_t Bloco_Total;
    bool FimDoArquivo;
    size_t Config[ITEM_CONFIG_QTD];
    bool (*Abre)(Status_t * st, struct SisArq_s * sisarq, struct Item_s * item);
    bool (*VaPara)(Status_t * st, struct SisArq_s * sisarq, struct Item_s * item, size_t posicao);
    size_t (*Leia)(Status_t * st, struct SisArq_s * sisarq, struct Item_s * item, uint8_t * dados, size_t tam);
    size_t (*Escreva)(Status_t * st, struct SisArq_s * sisarq, struct Item_s * item, uint8_t * dados, size_t tam);
    bool (*Fecha)(Status_t * st, struct SisArq_s * sisarq, struct Item_s * item);
    bool (*Primeiro)(Status_t * st, struct SisArq_s * sisarq, struct Item_s * item, struct Item_s * subItem);
    bool (*Proximo)(Status_t * st, struct SisArq_s * sisarq, struct Item_s * item, struct Item_s * subItem);
}Item_t;

typedef struct SisArq_s
{
    uint8_t Nome[ITEM_NOME_TAM+1];
    bool Visivel; // Visivel pelo usuario diretamente
    bool Protocolo; // Implementa um protocolo de acesso remoto (Acessos podem ser mais lentos)
    Item_t * Dispositivo;
    bool (*Desmontar)(Status_t * st, struct SisArq_s * sisarq);
    /// @brief Usado quando o Protocolo for falso
    bool (*Raiz)(Status_t * st, struct SisArq_s * sisarq, Item_t * item);
    /// @brief Usado quando o Protocolo for verdadeiro
    bool (*AbreEndereco)(Status_t * st, struct SisArq_s * sisarq, Item_t * item, uint8_t * endereco);
} SisArq_t;

#ifndef MODULO_H

/// @brief Inicializa o objeto Item_t
/// @param item Objeto a ser inicializado
void Item(Item_t * item);

/// @brief Inicializa o objeto SisArq_t
/// @param sisarq Objeto a ser inicializado
void SisArq(SisArq_t * sisarq);

/// @brief Inicializa o modulo SisArq
void SisArq_Inicializa();

/// @brief Registra um SisArq
/// @param st Status da acao
/// @param sisarq Sistema de Arquivos
/// @return 
bool SisArq_Registra(Status_t * st, SisArq_t * sisarq);

/// @brief Remove o registro de um SisArq
/// @param st Status da acao
/// @param sisarq Sistema de Arquivos
/// @return 
bool SisArq_Desregistra(Status_t * st, SisArq_t * sisarq);

/// @brief Abre um item(Arquivo/Diretorio)
/// @param st Status da acao
/// @param item Item destino
/// @param endereco Endereço do item (URL)
/// @return 
bool Abre(Status_t * st, Item_t * item, uint8_t *endereco);

/// @brief Fecha um item aberto
/// @param st Status da acao
/// @param item Item que deve ser fechado
/// @return 
bool Fecha(Status_t * st, Item_t * item);

/// @brief Va para uma posicao no item
/// @param st Status da acao
/// @param item Item de onde sera lido
/// @param posicao Posicao
/// @return 
bool VaPara(Status_t * st, Item_t * item, size_t posicao);

/// @brief Le um bloco de dados de um item
/// @param st Status da acao
/// @param item Item de onde sera lido
/// @param dados Destino
/// @param tam Tamanho do Destino
/// @return Tamanho Lido
size_t Leia(Status_t * st, Item_t * item, uint8_t * dados, size_t tam);

/// @brief Escreve um bloco de dados de um item
/// @param st Status da acao
/// @param item Item de onde sera lido
/// @param dados Origem
/// @param tam Tamanho da Origem
/// @return Tamanho Escrito
size_t Escreva(Status_t * st, Item_t * item, uint8_t * dados, size_t tam);

/// @brief Carrega o primeiro sub item de um diretorio
/// @param st Status da acao
/// @param item Item do Diretorio
/// @param subItem Sub item retorno
/// @return 
bool Diretorio_Primeiro(Status_t * st, Item_t * item, Item_t * subItem);

/// @brief Carrega o proximo sub item de um diretorio
/// @param st Status da acao
/// @param item Item do diretorio
/// @param subItem Sub item retorno
/// @return 
bool Diretorio_Proximo(Status_t * st, Item_t * item, Item_t * subItem);
#endif


#endif