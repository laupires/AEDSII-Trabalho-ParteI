#include "intercalacao.h"
#include "funcionarios.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

void intercalacao_basico(char *nome_arquivo_saida, int num_p, Lista *nome_particoes) {

    int fim = 0;
    FILE *out;

    if ((out = fopen(nome_arquivo_saida, "wb")) == NULL) {
        printf("Erro ao abrir arquivo de saida\n");
    } else {
        TVet v[num_p];

        for (int i = 0; i < num_p; i++) {
            v[i].f = fopen(nome_particoes->nome, "rb");
            v[i].aux_p = 0;

            if (v[i].f != NULL) {
                fseek(v[i].f, v[i].aux_p * tamanho_registro(), SEEK_SET);
                TFunc *f = le_funcionario(v[i].f);

                if (f == NULL) {
                    v[i].func = funcionario(INT_MAX, "", "", "", 0);
                } else {
                    v[i].func = f;
                }
            } else {
                fim = 1;
            }

            nome_particoes = nome_particoes->prox;
        }

        int aux = 0;

        while (!fim) {
            int menor = INT_MAX;
            int pos_menor = -1;

            for (int i = 0; i < num_p; i++) {
                if (v[i].func->cod < menor) {
                    menor = v[i].func->cod;
                    pos_menor = i;
                }
            }

            if (menor == INT_MAX) {
                fim = 1;
            } else {
                fseek(out, aux * tamanho_registro(), SEEK_SET);
                salva_funcionario(v[pos_menor].func, out);

                v[pos_menor].aux_p++;

                fseek(
                    v[pos_menor].f,
                    v[pos_menor].aux_p * tamanho_registro(),
                    SEEK_SET
                );

                TFunc *f = le_funcionario(v[pos_menor].f);
                aux++;

                if (f == NULL) {
                    v[pos_menor].func =
                        funcionario(INT_MAX, "", "", "", 0.0);
                } else {
                    v[pos_menor].func = f;
                }
            }
        }

        for (int i = 0; i < num_p; i++) {
            if (v[i].f != NULL)
                fclose(v[i].f);
        }

        fclose(out);
    }
}

/* Compara os dois participantes e retorna o vencedor */
static int escolher_vencedor(int a, int b, TVet *v) {

    if (a < 0 || v[a].func == NULL)
        return b;

    if (b < 0 || v[b].func == NULL)
        return a;

    if (v[a].func->cod <= v[b].func->cod)
        return a;

    return b;
}

/* Intercalacao utilizando Arvore Binaria de Vencedores */
void intercalacao_arvore_de_vencedores(
    char *nome_arquivo_saida,
    int num_p,
    Lista *nome_particoes
) {

    if (num_p <= 0 || nome_particoes == NULL) {
        printf("Nao existem particoes para intercalar.\n");
        return;
    }

    TVet *v = calloc(num_p, sizeof(TVet));

    int base = 1;

    while (base < num_p)
        base *= 2;

    int *arvore = malloc(2 * base * sizeof(int));

    FILE *saida = NULL;
    int abertos = 0;
    int gravados = 0;
    int sucesso = 0;

    if (v == NULL || arvore == NULL) {
        printf("Erro ao alocar memoria.\n");
        goto limpeza;
    }

    Lista *atual = nome_particoes;

    /* Abre todas as particoes */
    for (int i = 0; i < num_p; i++) {

        if (atual == NULL) {
            printf("Lista de particoes incompleta.\n");
            goto limpeza;
        }

        v[i].f = fopen(atual->nome, "rb");

        if (v[i].f == NULL) {
            printf("Erro ao abrir a particao: %s\n", atual->nome);
            goto limpeza;
        }

        abertos++;
        v[i].aux_p = 0;

        fseek(v[i].f, 0, SEEK_SET);
        v[i].func = le_funcionario(v[i].f);

        atual = atual->prox;
    }

    /* Inicializa as folhas da arvore */
    for (int i = 0; i < base; i++) {

        if (i < num_p && v[i].func != NULL)
            arvore[base + i] = i;
        else
            arvore[base + i] = -1;
    }

    /* Constroi a Arvore Binaria de Vencedores */
    for (int i = base - 1; i >= 1; i--) {

        arvore[i] = escolher_vencedor(
            arvore[i * 2],
            arvore[i * 2 + 1],
            v
        );
    }

    /* Cria o arquivo final */
    saida = fopen(nome_arquivo_saida, "wb+");

    if (saida == NULL) {
        printf("Erro ao criar arquivo de saida.\n");
        goto limpeza;
    }

    /* Realiza a intercalacao */
    while (arvore[1] != -1) {

        /* A raiz indica a particao vencedora */
        int vencedor = arvore[1];

        /* Grava o menor funcionario no arquivo final */
        if (fseek(
            saida,
            gravados * tamanho_registro(),
            SEEK_SET
        ) != 0) {
            printf("Erro ao gravar arquivo.\n");
            goto limpeza;
        }

        salva_funcionario(v[vencedor].func, saida);

        gravados++;

        free(v[vencedor].func);
        v[vencedor].func = NULL;

        /* Le o proximo funcionario da particao vencedora */
        v[vencedor].aux_p++;

        fseek(
            v[vencedor].f,
            v[vencedor].aux_p * tamanho_registro(),
            SEEK_SET
        );

        v[vencedor].func = le_funcionario(v[vencedor].f);

        /* Atualiza a folha vencedora */
        int pos = base + vencedor;

        if (v[vencedor].func != NULL)
            arvore[pos] = vencedor;
        else
            arvore[pos] = -1;

        /* Atualiza a arvore ate a raiz */
        pos /= 2;

        while (pos >= 1) {

            arvore[pos] = escolher_vencedor(
                arvore[pos * 2],
                arvore[pos * 2 + 1],
                v
            );

            pos /= 2;
        }
    }

    printf("\nIntercalacao concluida!\n");
    printf("Total de funcionarios gravados: %d\n", gravados);

    sucesso = 1;

limpeza:

    if (saida != NULL)
        fclose(saida);

    if (v != NULL) {

        for (int i = 0; i < abertos; i++) {
            free(v[i].func);
            fclose(v[i].f);
        }
    }

    free(v);
    free(arvore);

    if (!sucesso)
        remove(nome_arquivo_saida);
}
