#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "Brasis.h"
#include "Status.h"
#include "SisArq.h"
#include "String.h"
#include "Memoria.h"
#include "Semaforo.h"

void Bloqueia(Semaforo_t * semaforo)
{
    asm("cpu 386");
    asm(".tenta:");
    asm("lock bts dword [ebp+8], 0");
    asm("jc .tenta");
}

void Libera(Semaforo_t * semaforo)
{
    asm("cpu 386");
    asm(".tenta:");
    asm("lock btr dword [ebp+8], 0");
    asm("jc .tenta");
}