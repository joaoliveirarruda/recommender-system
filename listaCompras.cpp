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
                    string cliente = codClient;
                    string produto = codProduto;
                    string nome = nomeProduto;
                    
                    int cliente_idx;
                    if (compras.posicaoCodigoCliente.find(cliente) !=
                    compras.posicaoCodigoCliente.end()) {
                        cliente_idx = compras.posicaoCodigoCliente[cliente];
                    } else{
                        cliente_idx = compras.posicaoCodigoCliente[cliente];
                        compras.codigosClientes.push_back(cliente);
                        compras.posicaoCodigoCliente[cliente] = cliente_idx;
                        compras.comprasPorCliente.push_back(vector<int>());
                    }

                    int produto_idx;
                    if (compras.posicaoCodigoProduto.find(produto) != compras.posicaoCodigoProduto.end()) {
                    produto_idx = compras.posicaoCodigoProduto[produto];
                    }  else {
                    produto_idx = compras.nomesProdutos.size();
                    compras.nomesProdutos.push_back(nome);
                    compras.posicaoCodigoProduto[produto] = produto_idx;
                    }
                    
                    compras.comprasPorCliente[cliente_idx].push_back(produto_idx);
    }
    fclose(arquivo);

    return 0;
}