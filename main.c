
/*
  Trabalho de AEDSII - Grupo D
*/

#include <stdio.h>
#include <stdlib.h>

#include "ordenacao_busca.h"
#include "lista.h"
#include "particoes.h"
#include "intercalacao.h"
#include "arvore_binaria.h"

int menu() {
    int op;

    printf("\n\n**************MENU**************\n");

    printf("\nBASE DE DADOS\n");
    printf("\nDigite 1 para utilizar a base de dados funcionario de teste.");
    printf("\nDigite 2 para imprimir a base de funcionarios.");
    printf("\nDigite 3 para imprimir o indice primario.");

    printf("\n\nINTERCALACAO\n");
    printf("\nDigite 4 para criar a lista de nomes das particoes.");
    printf("\nDigite 5 para gerar particoes por classificacao interna.");
    printf("\nDigite 6 para gerar particoes por selecao natural.");
    printf("\nDigite 7 para criar pilhas com as particoes.");
    printf("\nDigite 8 para executar a intercalacao basica.");
    printf("\nDigite 9 para executar a Arvore Binaria de Vencedores.");

    printf("\n\nOUTROS ALGORITMOS\n");
    printf("\nDigite 10 para criar arvores binarias.");
    printf("\nDigite 11 para executar insertion sort em disco.");
    printf("\nDigite 12 para criar Arvore B+.");

    printf("\n\nDigite -1 para sair: ");
    scanf("%d", &op);

    return op;
}

int main() {

    FILE *arq = fopen("base_funcionarios.dat", "wb+");

    if (arq == NULL) {
        printf("Erro ao abrir base_funcionarios.dat.\n");
        return 1;
    }

    FILE *arqIndicePrimario =
        fopen("IndicePrimarioFunc.dat", "wb+");

    if (arqIndicePrimario == NULL) {
        printf("Erro ao abrir IndicePrimarioFunc.dat.\n");
        fclose(arq);
        return 1;
    }

    FILE *arqIndicePrimarioNaoOrdenado =
        fopen("IndicePrimarioFuncNaoOrdenado.dat", "wb+");

    if (arqIndicePrimarioNaoOrdenado == NULL) {
        printf("Erro ao abrir indice nao ordenado.\n");
        fclose(arq);
        fclose(arqIndicePrimario);
        return 1;
    }

    FILE *arqArvoreBinariaExterna =
        fopen("ArvoreBinariaExternaFunc.dat", "wb+");

    if (arqArvoreBinariaExterna == NULL) {
        printf("Erro ao abrir arquivo da arvore externa.\n");
        fclose(arq);
        fclose(arqIndicePrimario);
        fclose(arqIndicePrimarioNaoOrdenado);
        return 1;
    }

    int nFunc = 0;
    int nParticoes = 0;
    int nElementos = 0;

    Lista *nomes = NULL;
    Lista *prox = NULL;

    TPilha **vetPilhas = NULL;
    int *vetTopo = NULL;
    int qtdPilhas = 0;

    int crioubase = 0;
    int crioulistadenomes = 0;
    int criouparticoes = 0;
    int crioupilhas = 0;
    int usouSelecaoNatural = 0;

    int op = menu();

    while (op != -1) {

        switch (op) {

            case 1:
                nFunc = 20;

                cria_base_dados_funcionarios_teste(
                    arq,
                    arqIndicePrimario,
                    arqIndicePrimarioNaoOrdenado,
                    nFunc
                );

                crioubase = 1;
                criouparticoes = 0;
                crioupilhas = 0;

                printf("\nBase de funcionarios criada!\n");
                break;

            case 2:
                if (crioubase) {
                    imprime_base_dados_funcionarios(arq, nFunc);
                } else {
                    printf("\nCrie a base usando a opcao 1.\n");
                }
                break;

            case 3:
                if (crioubase) {

                    printf("\nIndice primario ordenado:\n");

                    imprime_indice_primario_funcionarios(
                        arqIndicePrimario, nFunc
                    );

                    printf("\nIndice primario nao ordenado:\n");

                    imprime_indice_primario_funcionarios(
                        arqIndicePrimarioNaoOrdenado, nFunc
                    );

                } else {
                    printf("\nCrie a base usando a opcao 1.\n");
                }
                break;

            case 4:
                nParticoes = 10;
                nElementos = 2;

                if (nomes != NULL) {
                    libera(nomes);
                    nomes = NULL;
                }

                nomes = cria("p1.dat",
                        cria("p2.dat",
                        cria("p3.dat",
                        cria("p4.dat",
                        cria("p5.dat",
                        cria("p6.dat",
                        cria("p7.dat",
                        cria("p8.dat",
                        cria("p9.dat",
                        cria("p10.dat", NULL))))))))));

                crioulistadenomes = 1;
                criouparticoes = 0;
                crioupilhas = 0;

                printf("\nLista de nomes das particoes:\n");
                imprime(nomes);
                break;

            case 5:
                if (crioubase && crioulistadenomes) {

                    printf("\nGerando particoes por classificacao interna.\n");

                    classificacao_interna(
                        arq, nomes, nElementos, nFunc
                    );

                    nParticoes = 10;
                    criouparticoes = 1;
                    crioupilhas = 0;
                    usouSelecaoNatural = 0;

                    prox = nomes;

                } else {
                    printf("\nExecute primeiro as opcoes 1 e 4.\n");
                }
                break;

            case 6: {
                if (crioubase && crioulistadenomes) {

                    printf("\nGerando particoes por selecao natural.\n");

                    /* Remove particoes antigas */
                    for (Lista *it = nomes; it != NULL; it = it->prox) {
                        remove(it->nome);
                    }

                    /* Executa a Selecao Natural */
                    selecao_natural(
                        arq, nomes, nElementos, nFunc, 1
                    );

                    /* Conta apenas as particoes geradas */
                    nParticoes = 0;

                    for (Lista *it = nomes; it != NULL; it = it->prox) {

                        FILE *teste = fopen(it->nome, "rb");

                        if (teste == NULL)
                            break;

                        fseek(teste, 0, SEEK_END);
                        long bytes = ftell(teste);
                        fclose(teste);

                        if (bytes <= 0)
                            break;

                        nParticoes++;
                    }

                    printf("\nParticoes validas: %d\n", nParticoes);

                    prox = nomes;
                    criouparticoes = (nParticoes > 0);
                    crioupilhas = 0;
                    usouSelecaoNatural = 1;

                } else {
                    printf("\nExecute primeiro as opcoes 1 e 4.\n");
                }
                break;
            }

            case 7: {
                if (!criouparticoes) {
                    printf("\nGere as particoes primeiro.\n");
                    break;
                }

                if (usouSelecaoNatural) {
                    printf("\nA Selecao Natural gera particoes de tamanhos variaveis.\n");
                    printf("Para intercalar, utilize a opcao 9.\n");
                    break;
                }

                if (vetPilhas != NULL) {
                    for (int p = 0; p < qtdPilhas; p++)
                        free(vetPilhas[p]);
                    free(vetPilhas);
                    vetPilhas = NULL;
                }

                free(vetTopo);
                vetTopo = NULL;

                vetPilhas = malloc(sizeof(TPilha *) * nParticoes);
                vetTopo = malloc(sizeof(int) * nParticoes);

                if (vetPilhas == NULL || vetTopo == NULL) {
                    printf("\nErro ao alocar pilhas.\n");
                    break;
                }

                qtdPilhas = 0;
                Lista *it = nomes;

                for (int p = 0; p < nParticoes && it != NULL; p++) {

                    vetPilhas[p] = malloc(sizeof(TPilha) * nElementos);

                    FILE *part = fopen(it->nome, "rb");

                    if (vetPilhas[p] == NULL || part == NULL) {
                        printf("\nErro ao criar pilha da particao.\n");
                        free(vetPilhas[p]);
                        if (part != NULL)
                            fclose(part);
                        break;
                    }

                    printf("\nParticao %s\n", it->nome);

                    imprime_base_dados_funcionarios(
                        part, nElementos
                    );

                    vetTopo[p] = -1;

                    cria_pilha_particao(
                        vetPilhas[p],
                        part,
                        nElementos,
                        &vetTopo[p],
                        p
                    );

                    fclose(part);

                    qtdPilhas++;
                    it = it->prox;
                }

                crioupilhas = (qtdPilhas == nParticoes);
                break;
            }

            case 8: {
                if (!criouparticoes) {
                    printf("\nGere as particoes primeiro.\n");
                    break;
                }

                printf("\nIntercalacao basica.\n");

                intercalacao_basico(
                    "arquivo_intercalado.dat",
                    nParticoes,
                    nomes
                );

                FILE *fi = fopen("arquivo_intercalado.dat", "rb");

                if (fi != NULL) {
                    imprime_base_dados_funcionarios(fi, nFunc);
                    fclose(fi);
                }
                break;
            }

            case 9: {
                if (!criouparticoes || nParticoes <= 0) {
                    printf("\nGere as particoes primeiro.\n");
                    printf("Execute as opcoes 1, 4 e 6.\n");
                    break;
                }

                printf("\nIntercalando com Arvore Binaria de Vencedores.\n");

                intercalacao_arvore_de_vencedores(
                    "arquivo_intercalado.dat",
                    nParticoes,
                    nomes
                );

                FILE *fi = fopen("arquivo_intercalado.dat", "rb");

                if (fi != NULL) {

                    printf("\nFuncionarios intercalados:\n");

                    imprime_base_dados_funcionarios(
                        fi, nFunc
                    );

                    fclose(fi);

                } else {
                    printf("\nArquivo intercalado nao foi gerado.\n");
                }

                break;
            }

            case 10:
                if (crioubase) {

                    printf("\nArvore binaria pelo indice ordenado:\n");

                    cria_arvore_binaria_indice_primario(
                        arqIndicePrimario, nFunc
                    );

                    printf("\nArvore binaria pelo indice nao ordenado:\n");

                    cria_arvore_binaria_indice_primario(
                        arqIndicePrimarioNaoOrdenado, nFunc
                    );

                    printf("\nArvore binaria externa:\n");

                    arqArvoreBinariaExterna = freopen(
                        "ArvoreBinariaExternaFunc.dat",
                        "wb+",
                        arqArvoreBinariaExterna
                    );

                    if (arqArvoreBinariaExterna != NULL) {
                        criar_arvore_binaria_externa(
                            arq, arqArvoreBinariaExterna
                        );
                    } else {
                        printf("\nErro ao abrir arvore externa.\n");
                    }

                } else {
                    printf("\nCrie a base usando a opcao 1.\n");
                }
                break;

            case 11: {
                if (crioubase) {

                    insertion_sort_disco(arq, nFunc);
                    imprime_base_dados_funcionarios(arq, nFunc);

                    int busca_codigo;

                    printf("\nInforme o codigo do funcionario:");
                    printf("\nDigite -1 para nao procurar: ");
                    scanf("%d", &busca_codigo);

                    if (busca_codigo != -1) {

                        TFunc *func = busca_binaria(
                            busca_codigo, arq, nFunc
                        );

                        if (func != NULL) {
                            imprime_funcionario(func);
                            free(func);
                        } else {
                            printf("\nFuncionario nao encontrado.\n");
                        }
                    }

                } else {
                    printf("\nCrie a base usando a opcao 1.\n");
                }
                break;
            }

            case 12:
                printf("\nParte II.\n");
                break;

            default:
                printf("\nOpcao invalida!\n");
                break;
        }

        op = menu();
    }

    /* Libera a lista de nomes */
    if (nomes != NULL)
        libera(nomes);

    /* Libera as pilhas */
    if (vetPilhas != NULL) {
        for (int p = 0; p < qtdPilhas; p++)
            free(vetPilhas[p]);

        free(vetPilhas);
    }

    free(vetTopo);

    /* Fecha os arquivos */
    fclose(arqIndicePrimario);
    fclose(arqIndicePrimarioNaoOrdenado);

    if (arqArvoreBinariaExterna != NULL)
        fclose(arqArvoreBinariaExterna);

    fclose(arq);

    return 0;
}
