#ifndef ES_H
#define ES_H

#include <stdint.h>

void ES_EscrevaUInt8(uint16_t porta, uint8_t valor);
void ES_EscrevaUInt16(uint16_t porta, uint16_t valor);
uint8_t ES_LeiaUInt8(uint16_t porta);
uint16_t ES_LeiaUInt16(uint16_t porta);

#endif