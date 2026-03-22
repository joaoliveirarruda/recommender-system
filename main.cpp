#include <stdio.h>
#include <ctime>
#include "listaCompras.hpp"
#include "similaridade.hpp"
#include "recomendacao.hpp"

int main() {
    const char *caminho = "./data/dados_venda_cluster_0.csv";
    ListaCompras compras = carregarCompras(caminho);

    if (compras.clientesCodigoBase.empty()) {
        printf("Nenhuma compra foi carregada ou ocorreu um erro.\n");
        return 1;
    }

    int nProdutos = (int)compras.produtosNomeDescritivo.size();
    int nClientes = 0;
    
    printf("\nTotal de clientes: %d\n", (int)compras.clientesCodigoBase.size());
    printf("Total de produtos: %d\n\n", nProdutos);

    printf("--- COMPARATIVO DE PERFORMANCE ---\n");
    
    // Algoritmo Padrao
    clock_t inicio_padrao = clock();
    int **I_padrao = calcularSimilaridade(&compras, &nClientes, false);
    clock_t fim_padrao = clock();
    double tempo_padrao = double(fim_padrao - inicio_padrao) / CLOCKS_PER_SEC;
    matrixFree(I_padrao, nClientes);
    
    // Algoritmo Otimizado
    clock_t inicio_otimizado = clock();
    int **I_otimizado = calcularSimilaridade(&compras, &nClientes, true);
    clock_t fim_otimizado = clock();
    double tempo_otimizado = double(fim_otimizado - inicio_otimizado) / CLOCKS_PER_SEC;
    
    printf("Tempo Algoritmo Padrao    (O(N^2 * M) + O(N*M)): %.4f s\n", tempo_padrao);
    printf("Tempo Algoritmo Otimizado (Triangulo Superior) : %.4f s\n\n", tempo_otimizado);

    double **S = calcularJaccard(I_otimizado, nClientes);

    printf("--- SISTEMA DE RECOMENDACAO ---\n");
    char codigo[50];
    int k = 5; // Quantidade padrao de produtos pra recomendar

    printf("Digite os codigos de 3 clientes para gerar recomendacoes:\n");
    for (int i = 0; i < 3; i++) {
        printf("\nCliente %d: ", i + 1);
        if (scanf(" %49s", codigo) != 1) return 1;

        std::map<std::string, int>::const_iterator it = compras.clienteIndiceInterno.find(std::string(codigo));
        
        if (it == compras.clienteIndiceInterno.end()) {
            printf("Cliente '%s' nao encontrado.\n", codigo);
            continue;
        }

        int idx_cliente = it->second;
        std::vector<ProdutoRecomendado> recs = recomendarProdutos(idx_cliente, k, &compras, S, nClientes, nProdutos);

        printf("Top %d recomendacoes para o cliente '%s':\n", k, codigo);
        if (recs.empty()) {
            printf("  Nao ha dados suficientes para recomendar produtos a este cliente.\n");
        } 
        else {
            for (const auto &r : recs) {
                printf("  [Score: %f] - %s\n", r.score, r.nome.c_str());
            }
        }
    }

    matrixFree(I_otimizado, nClientes);
    matrixFreeDouble(S, nClientes);

    return 0;
}