#ifndef MODULO_H
#define MODULO_H

#include "Nucleo.h"

extern Nucleo_t * Nucleo;

void Debug_Escreva(uint8_t * fmt,...);
void ES_EscrevaUInt8(uint16_t porta, uint8_t valor);
void ES_EscrevaUInt16(uint16_t porta, uint8_t valor);
uint8_t ES_LeiaUInt8(uint16_t porta);
uint8_t ES_LeiaUInt16(uint16_t porta);

#endif