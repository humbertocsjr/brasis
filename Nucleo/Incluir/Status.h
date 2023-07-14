#ifndef STATUS_T
#define STATUS_T

#include <stdbool.h>

#define STATUS_SEM_ERROS 0
#define STATUS_NAO_ENCONTRADO 1
#define STATUS_ESTOURO_DE_CAPACIDADE 2
#define STATUS_NAO_IMPLEMENTADO 3
#define STATUS_ENDERECO_INVALIDO 4

typedef struct Status_s
{
    bool Ok;
    size_t Tipo;
} Status_t;

#define STATUS_OK(st) st->Ok = true; st->Tipo = STATUS_SEM_ERROS;
#define STATUS_ERRO(st, tipo) st->Ok = false; st->Tipo = tipo;
#define STATUS_RET(st) return st->Ok;


#endif