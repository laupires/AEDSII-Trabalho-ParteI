#ifndef INTERCALACAO_H
#define INTERCALACAO_H

#include "lista.h"
#include "pilha.h"

/* Estrutura para armazenar o vetor de particoes */
typedef struct vetor {
    TFunc *func;
    int aux_p;
    FILE *f;
} TVet;

/* Algoritmo de intercalacao basica */
void intercalacao_basico(
    char *nome_arquivo_saida,
    int num_p,
    Lista *nome_particoes
);

/* Algoritmo de intercalacao com Arvore Binaria de Vencedores */
void intercalacao_arvore_de_vencedores(
    char *nome_arquivo_saida,
    int num_p,
    Lista *nome_particoes
);

#endif
