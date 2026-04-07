#include "recomendacao.hpp"
#include <algorithm>

bool compararScore(const ProdutoRecomendado &produtoA, const ProdutoRecomendado &produtoB) {
    if (produtoA.score != produtoB.score) {
        return produtoA.score < produtoB.score; // Ordem por score
    }
    return produtoA.nome < produtoB.nome; // Se o score for igual, ordena por ordem alfabética
}

std::vector<ProdutoRecomendado> recomendarProdutos(
    int clienteAlvo, int maxRecomendacoes, const ListaCompras* compras,
    double** distanciaJaccard, int nClientes, int nProdutos) {

    // Bloco 1: Encontrar clientes similares ao cliente alvo
    std::vector<int> clientesSimilares;
    for (int idCliente = 0; idCliente < nClientes; idCliente++) {
        if (idCliente != clienteAlvo && distanciaJaccard[clienteAlvo][idCliente] < 1.0) {
            clientesSimilares.push_back(idCliente);
        }
    }

    // Bloco 2: Marcar produtos já comprados e inicializar scores
    std::vector<double> scoresProdutos(nProdutos, 1.0);
    std::vector<bool> compradoPeloAlvo(nProdutos, false);
    for (int idProduto : compras->historicoComprasPorCliente[clienteAlvo]) {
        compradoPeloAlvo[idProduto] = true;
    }

    // Bloco 3: Calcular score de cada produto baseado nos clientes similares
    for (int idSimilar : clientesSimilares) {
        for (int idProduto : compras->historicoComprasPorCliente[idSimilar]) {
            if (!compradoPeloAlvo[idProduto]) {
                scoresProdutos[idProduto] = scoresProdutos[idProduto] * distanciaJaccard[clienteAlvo][idSimilar];
            }
        }
    }

    // Bloco 4: Montar lista de recomendações
    std::vector<ProdutoRecomendado> recomendacoes;
    for (int idProduto = 0; idProduto < nProdutos; idProduto++) {
        if (!compradoPeloAlvo[idProduto] && scoresProdutos[idProduto] < 1.0) {
            ProdutoRecomendado produtoRecomendado;
            produtoRecomendado.id_produto = idProduto;
            produtoRecomendado.nome = compras->produtosNomeDescritivo[idProduto];
            produtoRecomendado.score = scoresProdutos[idProduto];
            recomendacoes.push_back(produtoRecomendado);
        }
    }

    // Bloco 5: Ordenar e limitar ao máximo pedido
    std::sort(recomendacoes.begin(), recomendacoes.end(), compararScore);

    if (recomendacoes.size() > (size_t)maxRecomendacoes) {
        recomendacoes.resize(maxRecomendacoes);
    }

    return recomendacoes;
}