#include "listaCompras.hpp"
#include <stdio.h>
#include <stdlib.h>

ListaCompras carregarCompras(const char *caminhoArquivo) {
  FILE *arquivo;
  ListaCompras compras{}; //garante que se o struct tiver algum tipo primitivo de C, que eles sejam zerados também.

  arquivo = fopen(caminhoArquivo, "r");

  if (arquivo == NULL) {
    printf("Erro ao abrir arquivo\n");
    return compras;
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
    return compras;
  }

  while (fscanf(arquivo, " %*[^,],%49[^,],%49[^,],%49[^\n]", codClient,
                codProduto, nomeProduto) == 3) {
    std::string cliente = codClient;
    std::string produto = codProduto;
    std::string nome = nomeProduto;

    int cliente_idx;
    if (compras.clienteIndiceInterno.find(cliente) !=
        compras.clienteIndiceInterno.end()) {
      cliente_idx = compras.clienteIndiceInterno[cliente];
    } else {
      cliente_idx = compras.clientesCodigoBase.size();
      compras.clientesCodigoBase.push_back(cliente);
      compras.clienteIndiceInterno[cliente] = cliente_idx;
      compras.historicoComprasPorCliente.push_back(std::vector<int>());
    }

    int produto_idx;
    if (compras.produtoIndiceInterno.find(produto) !=
        compras.produtoIndiceInterno.end()) {
      produto_idx = compras.produtoIndiceInterno[produto];
    } else {
      produto_idx = compras.produtosNomeDescritivo.size();
      compras.produtosNomeDescritivo.push_back(nome);
      compras.produtoIndiceInterno[produto] = produto_idx;
    }

    compras.historicoComprasPorCliente[cliente_idx].push_back(produto_idx);
  }
  fclose(arquivo);

  return compras;
}