
#include "funcionarios.h"
#include <stdlib.h>
#include <limits.h>

#include "particoes.h"

void classificacao_interna(FILE *arq, Lista *nome_arquivos_saida, int M, int nFunc) {
    rewind(arq); //posiciona cursor no inicio do arquivo


    int reg = 0;

    while (reg != nFunc) {
        //le o arquivo e coloca no vetor
        TFunc *v[M];
        int i = 0;
        while (!feof(arq)) {
            fseek(arq, (reg) * tamanho_registro(), SEEK_SET);
            v[i] = le_funcionario(arq);
       //     imprime_funcionario(v[i]);
            i++;
            reg++;
            if(i>=M) break;
        }

        //ajusta tamanho M caso arquivo de entrada tenha terminado antes do vetor
        if (i != M) {
            M = i;
        }

        //faz ordenacao
        for (int j = 1; j < M; j++) {
            TFunc *f = v[j];
            i = j - 1;
            while ((i >= 0) && (v[i]->cod > f->cod)) {
                v[i + 1] = v[i];
                i = i - 1;
            }
            v[i + 1] = f;
        }

        //cria arquivo de particao e faz gravacao
        char *nome_particao = nome_arquivos_saida->nome;
        nome_arquivos_saida = nome_arquivos_saida->prox;
        printf("\n%s\n", nome_particao);
        FILE *p;
        if ((p = fopen(nome_particao, "wb+")) == NULL) {
            printf("Erro criar arquivo de saida\n");
        } else {
            for (int i = 0; i < M; i++) {
                fseek(p, (i) * tamanho_registro(), SEEK_SET);
                salva_funcionario(v[i], p);
                imprime_funcionario(v[i]);
            }
            fclose(p);
        }
        for(int jj = 0; jj<M; jj++)
            free(v[jj]);
    }
}



void selecao_natural(FILE *arq, Lista *nome_arquivos_saida, int M, int nFunc, int n) {
    if (!arq || !nome_arquivos_saida || M <= 0 || n <= 0 || nFunc <= 0) {
        printf("Parametros invalidos.\n");
        return;
    }

    TFunc **memoria = malloc(M * sizeof(TFunc *));
    TFunc **reservatorio = malloc(n * sizeof(TFunc *));

    if (!memoria || !reservatorio) {
        printf("Erro de memoria.\n");
        free(memoria);
        free(reservatorio);
        return;
    }

    int qtdMemoria = 0, qtdReserva = 0;
    int lidos = 0, gravados = 0, particoes = 0;

    // Carrega os primeiros funcionarios
    while (qtdMemoria < M && lidos < nFunc) {
        fseek(arq, lidos * tamanho_registro(), SEEK_SET);

        TFunc *f = le_funcionario(arq);
        if (!f) break;

        memoria[qtdMemoria++] = f;
        lidos++;
    }

    while (qtdMemoria > 0 || qtdReserva > 0) {
        if (!nome_arquivos_saida) {
            printf("Faltam nomes de arquivos para as particoes.\n");
            break;
        }

        FILE *saida = fopen(nome_arquivos_saida->nome, "wb+");

        if (!saida) {
            printf("Erro ao criar particao %s.\n", nome_arquivos_saida->nome);
            break;
        }

        printf("\nParticao %d: %s\n",
               ++particoes, nome_arquivos_saida->nome);

        int ultimoCodigo = INT_MIN;
        int gravadosParticao = 0;

        // Gera a particao em ordem crescente
        while (qtdMemoria > 0) {
            int menor = 0;

            for (int i = 1; i < qtdMemoria; i++) {
                if (memoria[i]->cod < memoria[menor]->cod)
                    menor = i;
            }

            TFunc *atual = memoria[menor];

            memoria[menor] = memoria[--qtdMemoria];

            fseek(saida, gravadosParticao * tamanho_registro(), SEEK_SET);
            salva_funcionario(atual, saida);
            imprime_funcionario(atual);

            gravadosParticao++;
            gravados++;
            ultimoCodigo = atual->cod;

            free(atual);

            // Le o proximo funcionario
            if (lidos < nFunc && qtdReserva < n) {
                fseek(arq, lidos * tamanho_registro(), SEEK_SET);

                TFunc *novo = le_funcionario(arq);

                if (novo) {
                    lidos++;

                    if (novo->cod >= ultimoCodigo) {
                        memoria[qtdMemoria++] = novo;
                    } else {
                        reservatorio[qtdReserva++] = novo;
                    }
                }
            }
        }

        fclose(saida);
        nome_arquivos_saida = nome_arquivos_saida->prox;

        // Transfere o reservatorio para a memoria
        for (int i = 0; i < qtdReserva; i++) {
            memoria[qtdMemoria++] = reservatorio[i];
        }

        qtdReserva = 0;

        // Completa a memoria para a proxima particao
        while (qtdMemoria < M && lidos < nFunc) {
            fseek(arq, lidos * tamanho_registro(), SEEK_SET);

            TFunc *f = le_funcionario(arq);
            if (!f) break;

            memoria[qtdMemoria++] = f;
            lidos++;
        }
    }

    for (int i = 0; i < qtdMemoria; i++)
        free(memoria[i]);

    for (int i = 0; i < qtdReserva; i++)
        free(reservatorio[i]);

    free(memoria);
    free(reservatorio);

    printf("\nTotal de particoes: %d\n", particoes);
    printf("Total de registros gravados: %d\n", gravados);
}
