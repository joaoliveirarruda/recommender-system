#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <vector>
#include <map>
#include <algorithm>

namespace py = pybind11;

// Função baseada no print da Atividade
std::vector<int> recomendar(std::vector<int> itens_consumidos, std::map<int, std::vector<int>> base_itens, int n) {
    std::vector<int> recomendacoes;
    
    // Aqui entra a sua lógica das Atividades 2, 3 ou 4.
    // Exemplo simples: recomendar itens da base que o usuário ainda não viu
    for (auto const& [id, relacionados] : base_itens) {
        // Verifica se o item já foi consumido
        if (std::find(itens_consumidos.begin(), itens_consumidos.end(), id) == itens_consumidos.end()) {
            recomendacoes.push_back(id);
            if (recomendacoes.size() >= n) break; // Para quando atingir N recomendações
        }
    }
    
    return recomendacoes;
}

PYBIND11_MODULE(meu_modulo, m) {
    m.doc() = "Módulo de recomendação real";
    
    // Fazendo o Bind da função real
    m.def("recomendar", &recomendar, "Retorna n recomendações baseadas no histórico",
          py::arg("itens_consumidos"), 
          py::arg("base_itens"), 
          py::arg("n"));
}