#include "similaridade.hpp"
#include <stdio.h>
#include <stdlib.h>


/* Aloca uma matriz 2D de inteiros (linhas x colunas), inicializada com 0 */
int** matrixAlloc(int linhas, int colunas) {
    int **matriz = (int **)malloc(linhas * sizeof(int *));
    for (int i = 0; i < linhas; i++) {
        matriz[i] = (int *)calloc(colunas, sizeof(int));
    }
    return matriz;
}


/* Libera a memoria de uma matriz 2D previamente alocada */
void matrixFree(int **matriz, int linhas) {
    for (int i = 0; i < linhas; i++) {
        free(matriz[i]);
    }
    free(matriz);
}


/*
 * Constroi a matriz de compras densa A[nClientes][nProdutos].
 * A[i][j] = 1 se o cliente i comprou o produto j, 0 caso contrario.
 */
int** construirMatrizDensa(ListaCompras *compras, int nClientes, int nProdutos) {
    int **A = matrixAlloc(nClientes, nProdutos);

    for (int i = 0; i < nClientes; i++) {
        int qtd = (int)compras->historicoComprasPorCliente[i].size();
        for (int k = 0; k < qtd; k++) {
            int produto_idx = compras->historicoComprasPorCliente[i][k];
            A[i][produto_idx] = 1;
        }
    }

    return A;
}


/*
 * Calcula a transposta AT[nProdutos][nClientes] da matriz A[nClientes][nProdutos].
 */
int** calcularTransposta(int **A, int nClientes, int nProdutos) {
    int **AT = matrixAlloc(nProdutos, nClientes);

    for (int i = 0; i < nClientes; i++) {
        for (int j = 0; j < nProdutos; j++) {
            AT[j][i] = A[i][j];
        }
    }

    return AT;
}


/*
 * Multiplica A[nClientes][nProdutos] por AT[nProdutos][nClientes],
 * armazenando o resultado em I[nClientes][nClientes].
 * I[i][j] = soma dos produtos comprados em comum entre cliente i e cliente j.
 */
void matrixMult(int **A, int **AT, int **I, int nClientes, int nProdutos) {
    for (int i = 0; i < nClientes; i++) {
        for (int j = 0; j < nClientes; j++) {
            I[i][j] = 0;
            for (int k = 0; k < nProdutos; k++) {
                I[i][j] += A[i][k] * AT[k][j];
            }
        }
    }
}


/*
 * Funcao principal do modulo: recebe uma ListaCompras e retorna a matriz
 * de similaridade (nClientes x nClientes) calculada por A x AT.
 * O numero de clientes e escrito em nClientes_out para uso externo.
 * O chamador e responsavel por liberar a memoria com matrixFree().
 */
int** calcularSimilaridade(ListaCompras *compras, int *nClientes_out) {
    int nClientes = (int)compras->clientesCodigoBase.size();
    int nProdutos = (int)compras->produtosNomeDescritivo.size();

    *nClientes_out = nClientes;

    int **A            = construirMatrizDensa(compras, nClientes, nProdutos);
    int **AT           = calcularTransposta(A, nClientes, nProdutos);
    int **similaridade = matrixAlloc(nClientes, nClientes);

    matrixMult(A, AT, similaridade, nClientes, nProdutos);

    matrixFree(A, nClientes);
    matrixFree(AT, nProdutos);

    return similaridade;
}
