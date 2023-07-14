#ifndef PROCESSADOR_H
#define PROCESSADOR_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

/// @brief Inicializa o processador
void Processador();

/// @brief Chama uma interrupcao (NAO REENTRANTE)
/// @param numero Numero da interrupcao
void Processador_ChamaInterrupcao(size_t numero);

/// @brief Registra um manipulador de interrupcao de baixo nivel
/// @param numero Numero da Interrupcao
/// @param acao Rotina de baixo nivel terminada em IRET
void Processador_RegistraInterrupcao(size_t numero, void (*acao)());

/// @brief Registra um manipulador de interrupcao de hardware de alto nivel
/// @param numero Numero da IRQ
/// @param acao Rotina de alto nivel comum
void Processador_RegistraIRQ(size_t numero, void (*acao)());

/// @brief Habilita as interrupcoes do processador
void Processador_HabilitarInterrupcoes();

/// @brief Desabilita as interrupcoes
void Processador_DesabilitarInterrupcoes();

#endif