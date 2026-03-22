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
    int cliente_c, 
    int k, 
    const ListaCompras* compras, 
    double** S, 
    int nClientes, 
    int nProdutos
);

#endif // RECOMENDACAO_HPP