#ifndef SIMILARIDADE_HPP
#define SIMILARIDADE_HPP

#include "listaCompras.hpp"

int** matrixAlloc(int linhas, int colunas);
void     matrixFree(int **matriz, int linhas);
double** matrixAllocDouble(int linhas, int colunas);
void     matrixFreeDouble(double **matriz, int linhas);
int** construirMatrizDensa(ListaCompras *compras, int nClientes, int nProdutos);
int** calcularTransposta(int **A, int nClientes, int nProdutos);
void     matrixMult(int **A, int **AT, int **I, int nClientes, int nProdutos);


void     matrixMult(int **A, int **I, int nClientes, int nProdutos);


int** calcularSimilaridade(ListaCompras *compras, int *nClientes_out, bool otimizada);
double** calcularJaccard(int **I, int nClientes);

#endif // SIMILARIDADE_HPP