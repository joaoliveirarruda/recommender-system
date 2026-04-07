#ifndef RECOMENDACAO_HPP
#define RECOMENDACAO_HPP

#include "listaCompras.hpp"
#include <vector>
#include <string>

struct ProdutoRecomendado {
    int id_produto;
    std::string nome;
    double score;
};

std::vector<ProdutoRecomendado> recomendarProdutos(
    int clienteAlvo, 
    int maxRecomendacoes, 
    const ListaCompras* compras, 
    double** distanciaJaccard, 
    int nClientes, 
    int nProdutos
);

#endif // RECOMENDACAO_HPP