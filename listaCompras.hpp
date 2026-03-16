#ifndef LISTACOMPRAS_HPP
#define LISTACOMPRAS_HPP

#include <string>
#include <vector>
#include <map>

struct ListaCompras {
    std::vector<std::string> clientesCodigoBase;
    std::map<std::string, int> clienteIndiceInterno;
    std::vector<std::string> produtosNomeDescritivo;
    std::map<std::string, int> produtoIndiceInterno;
    std::vector<std::vector<int>> historicoComprasPorCliente;
};

ListaCompras carregarCompras(const char *caminhoArquivo);

#endif // LISTACOMPRAS_HPP
