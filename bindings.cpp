#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "listaCompras.hpp"
#include "similaridade.hpp"
#include "recomendacao.hpp"

namespace py = pybind11;

// Wrapper que encapsula as matrizes C (int** e double**) e gerencia a memória
struct MatrizInt {
    int **data;
    int linhas, colunas;

    MatrizInt(int **d, int l, int c) : data(d), linhas(l), colunas(c) {}
    ~MatrizInt() { if (data) matrixFree(data, linhas); }

    // Desabilita cópia para evitar double-free
    MatrizInt(const MatrizInt&) = delete;
    MatrizInt& operator=(const MatrizInt&) = delete;

    int get(int i, int j) const { return data[i][j]; }

    std::vector<std::vector<int>> to_list() const {
        std::vector<std::vector<int>> resultado(linhas, std::vector<int>(colunas));
        for (int i = 0; i < linhas; i++)
            for (int j = 0; j < colunas; j++)
                resultado[i][j] = data[i][j];
        return resultado;
    }
};

struct MatrizDouble {
    double **data;
    int linhas, colunas;

    MatrizDouble(double **d, int l, int c) : data(d), linhas(l), colunas(c) {}
    ~MatrizDouble() { if (data) matrixFreeDouble(data, linhas); }

    MatrizDouble(const MatrizDouble&) = delete;
    MatrizDouble& operator=(const MatrizDouble&) = delete;

    double get(int i, int j) const { return data[i][j]; }

    std::vector<std::vector<double>> to_list() const {
        std::vector<std::vector<double>> resultado(linhas, std::vector<double>(colunas));
        for (int i = 0; i < linhas; i++)
            for (int j = 0; j < colunas; j++)
                resultado[i][j] = data[i][j];
        return resultado;
    }
};

// Wrapper para calcularSimilaridade que retorna MatrizInt gerenciada
MatrizInt* wrap_calcularSimilaridade(ListaCompras &compras, bool otimizada) {
    int nClientes = 0;
    int **I = calcularSimilaridade(&compras, &nClientes, otimizada);
    return new MatrizInt(I, nClientes, nClientes);
}

// Wrapper para calcularJaccard que retorna MatrizDouble gerenciada
MatrizDouble* wrap_calcularJaccard(MatrizInt &intersecao) {
    double **S = calcularJaccard(intersecao.data, intersecao.linhas);
    return new MatrizDouble(S, intersecao.linhas, intersecao.linhas);
}

// Wrapper para recomendarProdutos
std::vector<ProdutoRecomendado> wrap_recomendarProdutos(
    int clienteAlvo, int maxRecomendacoes,
    ListaCompras &compras, MatrizDouble &jaccard) {

    int nProdutos = (int)compras.produtosNomeDescritivo.size();
    return recomendarProdutos(clienteAlvo, maxRecomendacoes, &compras,
                              jaccard.data, jaccard.linhas, nProdutos);
}

PYBIND11_MODULE(recommender, m) {
    m.doc() = "Sistema de recomendação de produtos";

    // ListaCompras
    py::class_<ListaCompras>(m, "ListaCompras")
        .def(py::init<>())
        .def_readonly("clientes", &ListaCompras::clientesCodigoBase)
        .def_readonly("cliente_indice", &ListaCompras::clienteIndiceInterno)
        .def_readonly("produtos", &ListaCompras::produtosNomeDescritivo)
        .def_readonly("produto_indice", &ListaCompras::produtoIndiceInterno)
        .def_readonly("historico", &ListaCompras::historicoComprasPorCliente);

    m.def("carregar_compras", &carregarCompras, py::arg("caminho_arquivo"),
          "Carrega dados de compras a partir de um arquivo CSV");

    // MatrizInt
    py::class_<MatrizInt>(m, "MatrizInt")
        .def_readonly("linhas", &MatrizInt::linhas)
        .def_readonly("colunas", &MatrizInt::colunas)
        .def("get", &MatrizInt::get, py::arg("i"), py::arg("j"))
        .def("to_list", &MatrizInt::to_list);

    // MatrizDouble
    py::class_<MatrizDouble>(m, "MatrizDouble")
        .def_readonly("linhas", &MatrizDouble::linhas)
        .def_readonly("colunas", &MatrizDouble::colunas)
        .def("get", &MatrizDouble::get, py::arg("i"), py::arg("j"))
        .def("to_list", &MatrizDouble::to_list);

    // Similaridade
    m.def("calcular_similaridade", &wrap_calcularSimilaridade,
          py::arg("compras"), py::arg("otimizada") = true,
          py::return_value_policy::take_ownership,
          "Calcula a matriz de intersecção (similaridade) entre clientes");

    m.def("calcular_jaccard", &wrap_calcularJaccard,
          py::arg("intersecao"),
          py::return_value_policy::take_ownership,
          "Calcula a distância de Jaccard a partir da matriz de intersecção");

    // ProdutoRecomendado
    py::class_<ProdutoRecomendado>(m, "ProdutoRecomendado")
        .def_readonly("id_produto", &ProdutoRecomendado::id_produto)
        .def_readonly("nome", &ProdutoRecomendado::nome)
        .def_readonly("score", &ProdutoRecomendado::score)
        .def("__repr__", [](const ProdutoRecomendado &p) {
            return "<ProdutoRecomendado '" + p.nome + "' score=" + std::to_string(p.score) + ">";
        });

    // Recomendação
    m.def("recomendar_produtos", &wrap_recomendarProdutos,
          py::arg("cliente_alvo"), py::arg("max_recomendacoes"),
          py::arg("compras"), py::arg("jaccard"),
          "Recomenda produtos para um cliente com base na similaridade de Jaccard");
}
