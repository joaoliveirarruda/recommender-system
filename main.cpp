#include <iostream>
#include <string>
#include "listaCompras.hpp"

using namespace std;

int main() {
    string caminho = "./data/dados_venda_cluster_0.csv";
    
    ListaCompras compras = carregarCompras(caminho);
    
    if (compras.produtosNomeDescritivo.empty()) {
        cout << "Nenhuma compra foi carregada ou ocorreu um erro." << endl;
        return 1;
    }
    
    cout << "Total de clientes: " << compras.clientesCodigoBase.size() << endl;
    cout << "Total de produtos identificados: " << compras.produtosNomeDescritivo.size() << endl;
    
    return 0;
}
