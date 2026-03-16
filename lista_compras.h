#ifndef LISTA_COMPRAS_H
#define LISTA_COMPRAS_H

#include <string>
#include <vector>
#include <map>

using namespace std;

// Estrutura responsável por organizar e armazenar o histórico [cite: 456]
typedef struct {
    
    vector<string> codigosClientes;// Vetor contendo os códigos de todos os clientes [cite: 458]
    
    map<string, int> posicaoCodigoCliente;// Mapa que associa o código original ao índice interno único [cite: 459]
    
    vector<string> nomesProdutos;// Vetor contendo os nomes de todos os produtos [cite: 460]
    
    map<string, int> posicaoCodigoProduto;// Mapa que associa o código original do produto ao índice interno [cite: 461]
    
    vector<vector<int>> comprasPorCliente;// Vetor onde cada célula contém o vetor de produtos comprados [cite: 462]

} ListaCompras;

//talvez seja o caso de remover a definição do struct do listaCompras.cpp e deixar só aqui


#endif