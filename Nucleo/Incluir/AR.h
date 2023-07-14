#ifndef AR_H
#define AR_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "SisArq.h"

/// @brief Monta um sistema de arquivos AR
/// @param sisarq Sistema de Arquivos
/// @param nome Nome do ponto de montagem
/// @param dispositivo Dispositivo de dados
void AR(SisArq_t * sisarq, uint8_t *nome, Item_t * dispositivo);



#endif