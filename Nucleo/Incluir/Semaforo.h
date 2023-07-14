#ifndef SEMAFORO_H
#define SEMAFORO_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

typedef unsigned int Semaforo_t;
void Bloqueia(Semaforo_t * semaforo);
void Libera(Semaforo_t * semaforo);

#define SEMAFORO(nome) static Semaforo_t nome = 0 //Cria um semaforo


#endif