#include "similaridade.hpp"
#include <stdio.h>
#include <stdlib.h>

int** matrixAlloc(int linhas, int colunas) {
    int **matriz = (int **)malloc(linhas * sizeof(int *));
    for (int i = 0; i < linhas; i++) {
        matriz[i] = (int *)calloc(colunas, sizeof(int));
    }
    return matriz;
}

void matrixFree(int **matriz, int linhas) {
    for (int i = 0; i < linhas; i++) {
        free(matriz[i]);
    }
    free(matriz);
}

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

int** calcularTransposta(int **A, int nClientes, int nProdutos) {
    int **AT = matrixAlloc(nProdutos, nClientes);

    for (int i = 0; i < nClientes; i++) {
        for (int j = 0; j < nProdutos; j++) {
            AT[j][i] = A[i][j];
        }
    }

    return AT;
}

void matrixMult(int **A, int **AT, int **I, int nClientes, int nProdutos) {
    for (int i = 0; i < nClientes; i++) {
        for (int j = 0; j < nClientes; j++) {
            for (int k = 0; k < nProdutos; k++) {
                I[i][j] += A[i][k] * AT[k][j];
            }
        }
    }
}

//  Multiplica A por ela mesma explorando a simetria 
 
void matrixMultOtimizada(int **A, int **I, int nClientes, int nProdutos) {
    for (int i = 0; i < nClientes; i++) {
        for (int j = i; j < nClientes; j++) { // j comeca em i
            int soma = 0;
            for (int k = 0; k < nProdutos; k++) {
                soma += A[i][k] * A[j][k];
            }
            I[i][j] = soma;
            I[j][i] = soma; // Aproveita a simetria 
        }
    }
}

// Escolhe qual algoritmo usar

int** calcularSimilaridade(ListaCompras *compras, int *nClientes_out, bool otimizada) {
    int nClientes = (int)compras->clientesCodigoBase.size();
    int nProdutos = (int)compras->produtosNomeDescritivo.size();

    *nClientes_out = nClientes;

    int **A            = construirMatrizDensa(compras, nClientes, nProdutos);
    int **intersecao   = matrixAlloc(nClientes, nClientes);

    if (otimizada) {
        matrixMultOtimizada(A, intersecao, nClientes, nProdutos);
    } else {
        int **AT = calcularTransposta(A, nClientes, nProdutos);
        matrixMult(A, AT, intersecao, nClientes, nProdutos);
        matrixFree(AT, nProdutos);
    }

    matrixFree(A, nClientes);

    return intersecao;
}

double** matrixAllocDouble(int linhas, int colunas) {
    double **matriz = (double **)malloc(linhas * sizeof(double *));
    for (int i = 0; i < linhas; i++) {
        matriz[i] = (double *)calloc(colunas, sizeof(double));
    }
    return matriz;
}

void matrixFreeDouble(double **matriz, int linhas) {
    for (int i = 0; i < linhas; i++) {
        free(matriz[i]);
    }
    free(matriz);
}

double** calcularJaccard(int **I, int nClientes) {
    double **S = matrixAllocDouble(nClientes, nClientes);

    for (int i = 0; i < nClientes; i++) {
        for (int j = 0; j < nClientes; j++) {
            if (I[i][i] == 0)
                S[i][j] = 1.0; /* cliente sem compras: maxima distancia */
            else
                S[i][j] = 1.0 - (double)I[i][j] / (double)I[i][i];
        }
    }

    return S;
}