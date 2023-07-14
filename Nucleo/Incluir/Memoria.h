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
size_t Memoria_Ocupado();
size_t Memoria_Processo(uint8_t processo);

// Gestão de Memória Fisica
size_t AlocaPaginaFisica(uint8_t processo, bool especialEquipamento, size_t qtd);
void LiberaPaginaFisica(size_t endereco);

void Memoria_Copia(uint8_t * destino, uint8_t * origem, size_t tam);
void Memoria_Zera(uint8_t * destino, size_t tam);

#endif