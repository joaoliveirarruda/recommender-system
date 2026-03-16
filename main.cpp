#include <stdio.h>
#include "listaCompras.hpp"
#include "similaridade.hpp"

static void exibirProdutosDoCliente(const ListaCompras *compras, const char *codigoCliente) {
    std::map<std::string, int>::const_iterator it =
        compras->clienteIndiceInterno.find(std::string(codigoCliente));

    if (it == compras->clienteIndiceInterno.end()) {
        printf("  Cliente '%s' nao encontrado na base.\n", codigoCliente);
        return;
    }

    int idx = it->second;
    int qtd = (int)compras->historicoComprasPorCliente[idx].size();

    printf("  Produtos comprados por '%s' (%d produto(s)):\n", codigoCliente, qtd);
    for (int k = 0; k < qtd; k++) {
        int produto_idx = compras->historicoComprasPorCliente[idx][k];
        printf("    - %s\n", compras->produtosNomeDescritivo[produto_idx].c_str());
    }
}


int main() {
    const char *caminho = "./data/dados_venda_cluster_0.csv";

    ListaCompras compras = carregarCompras(caminho);

    if (compras.clientesCodigoBase.empty()) {
        printf("Nenhuma compra foi carregada ou ocorreu um erro.\n");
        return 1;
    }

    printf("\nTotal de clientes: %d\n", (int)compras.clientesCodigoBase.size());
    printf("Total de produtos: %d\n\n", (int)compras.produtosNomeDescritivo.size());

    char codigo[50];
    int indices[3] = {-1, -1, -1};

    printf("Digite os codigos de 3 clientes (um por linha):\n");
    for (int i = 0; i < 3; i++) {
        printf("  Cliente %d: ", i + 1);
        if (scanf(" %49s", codigo) != 1) {
            printf("Erro na leitura.\n");
            return 1;
        }
        printf("\n");
        exibirProdutosDoCliente(&compras, codigo);
        printf("\n");

        std::map<std::string, int>::const_iterator it =
            compras.clienteIndiceInterno.find(std::string(codigo));
        if (it != compras.clienteIndiceInterno.end())
            indices[i] = it->second;
    }

    int nClientes = 0;
    int    **I = calcularSimilaridade(&compras, &nClientes);
    double **S = calcularJaccard(I, nClientes);

    printf("---------------------------------------\n");
    printf("Similaridade de Jaccard entre os 3 clientes:\n\n");
    printf("        C1     C2     C3\n");
    for (int i = 0; i < 3; i++) {
        printf("C%d  ", i + 1);
        for (int j = 0; j < 3; j++) {
            if (indices[i] < 0 || indices[j] < 0)
                printf("%7s", "-");
            else
                printf("%7.2f", S[indices[i]][indices[j]]);
        }
        printf("\n");
    }

    matrixFree(I, nClientes);
    matrixFreeDouble(S, nClientes);

    return 0;
}
