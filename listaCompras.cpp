#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <map>
using namespace std;


typedef struct {
    vector<string> codigosClientes;
    map<string, int> posicaoCodigoCliente;
    vector<string> nomesProdutos;
    map<string, int> posicaoCodigoProduto;
    vector<vector<int>> comprasPorCliente;
} ListaCompras;


int main() {
    FILE *arquivo;
    ListaCompras compras;

    arquivo = fopen("./data/dados_venda_cluster_0.csv", "r");

    if (arquivo == NULL) {
        printf("Erro ao abrir arquivo");
        return 1;
    }

    printf("Leitura dos dados do arquivo:\n");
    printf("---------------------------------------\n");

    char codClient[50];
    char codProduto[50];
    char nomeProduto[50];

    char cabecalho[200];
    if (fgets(cabecalho, sizeof(cabecalho), arquivo) == NULL) { 
        printf("Arquivo vazio\n");
        fclose(arquivo);
        return 1;
    }

    while (fscanf(arquivo, " %*[^,],%49[^,],%49[^,],%49[^\n]",
                codClient,
                codProduto,
                nomeProduto) == 3) {
                    printf("COD_CLIENTE: %-10s | COD_PRODUTO: %s | NOME_PRODUTO: %s\n",
                        codClient, codProduto, nomeProduto);

    }
    fclose(arquivo);

    return 0;
}