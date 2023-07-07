#ifndef DEBUG_H
#define DEBUG_H

void Debug_ConfigVideoTexto();
void Debug_EscrevaTexto(uint8_t * txt);
void Debug_EscrevaUInt(uint32_t valor);
void Debug_EscrevaInt(int32_t valor);
void Debug_Escreva(uint8_t * fmt, ...);

#endif 