#ifndef MEMORIA_H
#define MEMORIA_H


void Memoria
(
    size_t codigo_inicio, 
    size_t codigo_fim, 
    size_t dados_inicio, 
    size_t dados_fim,
    size_t imagem_inicio,
    size_t imagem_fim,
    size_t mem_baixa,
    size_t mem_media,
    size_t mem_alta,
    size_t mem_max
);

size_t Memoria_Livre();

#endif