#include <stdio.h>
#include <stdlib.h>
#include <string>


int** matrixalloc(int linhas, int colunas){

    int** matriz = (int**)malloc(linhas * sizeof(int*));//aloca o vetor de ponteiros
    for (int i = 0; i < linhas; i++){

        matriz[i] = (int*)calloc(colunas, sizeof(int));// aloca os vetores coluna
    }
    return matriz;
}


void matrix_mult (int **A, int **AT, int** I, int m, int n){

    for (int i = 0; i < n; i++){ // n == linhas(clientes) , m == colunas(produtos).

        for (int j = 0; j < n; j++){

            I[i][j] = 0; // crio o elemento da matriz interseção.

            for (int k = 0; k < m; k++){ // k == indice dos produtos.

                I[i][j] += A[i][k] * AT[k][j];
                // cada elemento recebe a soma da multiplicação de cada linha por coluna
            }
          
        }

    }

}

