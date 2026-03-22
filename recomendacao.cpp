#include "recomendacao.hpp"
#include <algorithm>

bool compararScore(const ProdutoRecomendado &a, const ProdutoRecomendado &b) {
    if (a.score != b.score) {
        return a.score < b.score; // Ordem por score
    }
    return a.nome < b.nome; // Se o score for igual, ordena por ordem alfabética
}
std::vector<ProdutoRecomendado> recomendarProdutos(
    int cliente_c, int k, const ListaCompras* compras, double** S, int nClientes, int nProdutos) {

    std::vector<int> L;
    for (int s = 0; s < nClientes; s++) {
        if (s != cliente_c && S[cliente_c][s] < 1.0) {
            L.push_back(s);
        }
    }

    std::vector<double> R(nProdutos, 1.0);
    std::vector<bool> comprado_por_c(nProdutos, false);
    for (int p : compras->historicoComprasPorCliente[cliente_c]) {
        comprado_por_c[p] = true;
    }

    for (int s : L) {
        for (int p : compras->historicoComprasPorCliente[s]) {
            if (!comprado_por_c[p]) {
                R[p] = R[p] * S[cliente_c][s];
            }
        }
    }

    std::vector<ProdutoRecomendado> recomendacoes;
    for (int p = 0; p < nProdutos; p++) {
        if (!comprado_por_c[p] && R[p] < 1.0) {
            ProdutoRecomendado rec;
            rec.id_produto = p;
            rec.nome = compras->produtosNomeDescritivo[p];
            rec.score = R[p];
            recomendacoes.push_back(rec);
        }
    }

    std::sort(recomendacoes.begin(), recomendacoes.end(), compararScore);

    if (recomendacoes.size() > (size_t)k) {
        recomendacoes.resize(k);
    }

    return recomendacoes;
}