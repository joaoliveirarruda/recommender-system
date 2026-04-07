# Integração C++ ↔ Python com pybind11

## O que foi feito

Foi criada uma camada de bindings que permite chamar todos os módulos do sistema de recomendação escritos em C++ diretamente a partir de Python, sem precisar reescrever nenhuma lógica. O módulo Python resultante se chama `recommender` e expõe as seguintes funções e classes:

| Elemento Python | Origem C++ | Descrição |
|---|---|---|
| `carregar_compras(caminho)` | `carregarCompras()` | Lê o CSV e retorna um objeto `ListaCompras` |
| `calcular_similaridade(compras, otimizada)` | `calcularSimilaridade()` | Calcula a matriz de intersecção entre clientes |
| `calcular_jaccard(intersecao)` | `calcularJaccard()` | Calcula a distância de Jaccard a partir da intersecção |
| `recomendar_produtos(cliente, max, compras, jaccard)` | `recomendarProdutos()` | Retorna uma lista de `ProdutoRecomendado` |
| `ListaCompras` | `struct ListaCompras` | Acesso aos clientes, produtos e histórico |
| `MatrizInt` / `MatrizDouble` | Wrappers novos | Encapsulam as matrizes C (`int**`, `double**`) |
| `ProdutoRecomendado` | `struct ProdutoRecomendado` | Contém `id_produto`, `nome` e `score` |

## O que é um Wrapper

Um **wrapper** (do inglês "to wrap" = envolver) é uma camada de código que **envolve** algo existente para torná-lo mais fácil ou seguro de usar, sem alterar o comportamento original.

Uma analogia simples: imagine um adaptador de tomada. A tomada na parede (o código C++) funciona perfeitamente, mas o plugue do seu aparelho (Python) não encaixa nela. O adaptador (wrapper) não muda como a eletricidade funciona — ele apenas faz a conexão entre as duas interfaces.

### Wrapper de dados (structs `MatrizInt` e `MatrizDouble`)

No código C++ original, as matrizes são representadas como `int**` — um ponteiro para ponteiro, que é essencialmente um endereço de memória bruto. Python não entende endereços de memória, então não há como passar um `int**` diretamente para Python.

O wrapper `MatrizInt` **envolve** esse ponteiro dentro de uma struct que Python consegue manipular:

```
Sem wrapper (impossível em Python):
  Python → int** (endereço de memória) → ???

Com wrapper:
  Python → objeto MatrizInt → .get(i, j) → acessa int** internamente → retorna o valor
```

Além de traduzir a interface, o wrapper também assume a responsabilidade de **liberar a memória**. Em C, quem aloca com `malloc` precisa liberar com `free` manualmente. Em Python, a memória é liberada automaticamente quando o objeto não é mais usado (garbage collector). O destrutor do wrapper conecta esses dois mundos:

```cpp
~MatrizInt() { 
    if (data) matrixFree(data, linhas);  // libera a memória C quando o Python descarta o objeto
}
```

### Wrapper de funções (`wrap_calcularSimilaridade`, etc.)

As funções C++ originais usam padrões comuns em C que não fazem sentido em Python, como parâmetros de saída via ponteiro:

```cpp
// C++: o resultado "nClientes" é retornado via ponteiro (int*)
int** calcularSimilaridade(ListaCompras *compras, int *nClientes_out, bool otimizada);
```

Em Python, não existe o conceito de "passar um ponteiro para receber um valor de volta". O wrapper simplifica essa interface:

```cpp
// Wrapper: esconde o ponteiro, retorna um objeto completo
MatrizInt* wrap_calcularSimilaridade(ListaCompras &compras, bool otimizada) {
    int nClientes = 0;
    int **I = calcularSimilaridade(&compras, &nClientes, otimizada);  // chama o original
    return new MatrizInt(I, nClientes, nClientes);                     // envolve o resultado
}
```

Do ponto de vista do Python, a chamada fica simples e natural:

```python
# Python: sem ponteiros, sem preocupação com memória
intersecao = rec.calcular_similaridade(compras, otimizada=True)
```

### Resumo

| Sem wrapper | Com wrapper |
|---|---|
| Python não consegue acessar `int**` | Python acessa via `MatrizInt.get(i, j)` |
| Memória precisa ser liberada manualmente | Memória é liberada automaticamente |
| Funções exigem ponteiros de saída | Funções retornam objetos completos |
| Interface pensada para C | Interface pensada para Python |

O wrapper não muda nenhuma lógica do sistema de recomendação. Ele é apenas o **adaptador** que permite que o código C++ existente seja utilizado de forma natural em Python.

## Como foi feito

### 1. Arquivo `bindings.cpp`

Este é o arquivo central da integração. Ele usa a biblioteca [pybind11](https://github.com/pybind/pybind11) para declarar ao Python quais funções e structs do C++ devem ser acessíveis.

A macro `PYBIND11_MODULE(recommender, m)` define o ponto de entrada do módulo. Dentro dela, cada classe é registrada com `py::class_<>` e cada função com `m.def()`.

#### Problema: matrizes C com ponteiro duplo (`int**`)

O código original usa `int**` e `double**` para representar matrizes — ponteiros alocados manualmente com `malloc`/`calloc`. Esse tipo de dado não tem equivalente direto em Python, então foi necessário criar wrappers:

```cpp
struct MatrizInt {
    int **data;
    int linhas, colunas;

    ~MatrizInt() { if (data) matrixFree(data, linhas); }

    int get(int i, int j) const { return data[i][j]; }
    std::vector<std::vector<int>> to_list() const { /* ... */ }
};
```

Esses wrappers resolvem dois problemas:

- **Acesso aos dados**: o método `get(i, j)` permite consultar elementos individuais e `to_list()` converte a matriz inteira para uma lista de listas Python.
- **Gerenciamento de memória**: o destrutor (`~MatrizInt`) chama `matrixFree` automaticamente quando o objeto Python sai de escopo, evitando vazamento de memória (memory leak).

A cópia dessas structs foi desabilitada para evitar double-free (dois objetos tentando liberar a mesma memória):

```cpp
MatrizInt(const MatrizInt&) = delete;
MatrizInt& operator=(const MatrizInt&) = delete;
```

#### Funções wrapper

Algumas funções do C++ têm assinaturas inconvenientes para Python (ponteiros de saída, ponteiros duplos como parâmetro). Foram criadas funções wrapper que simplificam a interface:

```cpp
// C++ original: recebe ponteiro int* para saída
int** calcularSimilaridade(ListaCompras *compras, int *nClientes_out, bool otimizada);

// Wrapper: retorna objeto gerenciado, sem ponteiros expostos
MatrizInt* wrap_calcularSimilaridade(ListaCompras &compras, bool otimizada);
```

### 2. Arquivo `setup.py`

Define como compilar o módulo usando `setuptools` e o helper `Pybind11Extension`:

```python
ext_modules = [
    Pybind11Extension(
        "recommender",
        ["bindings.cpp", "listaCompras.cpp", "similaridade.cpp", "recomendacao.cpp"],
        cxx_std=11,
    ),
]
```

Ele lista todos os `.cpp` que precisam ser compilados juntos e define C++11 como padrão (necessário por causa de `std::vector`, `std::map`, etc.).

Para compilar:

```bash
python3 setup.py build_ext --inplace
```

Isso gera um arquivo `.so` (shared object) na raiz do projeto que o Python importa como módulo nativo.

### 3. Conversão automática de tipos (pybind11 + STL)

O header `pybind11/stl.h` faz a conversão automática entre tipos C++ da STL e tipos Python:

| C++ | Python |
|---|---|
| `std::string` | `str` |
| `std::vector<T>` | `list` |
| `std::map<K, V>` | `dict` |
| `bool` | `bool` |
| `int`, `double` | `int`, `float` |

Isso significa que `compras.clientes` retorna uma `list[str]` em Python sem nenhuma conversão manual.

## Por que foi feito

### Performance do C++ com ergonomia do Python

O sistema de recomendação envolve multiplicação de matrizes e cálculos de similaridade que se beneficiam da performance do C compilado. Porém, C++ não é ideal para prototipagem rápida, visualização de dados ou integração com bibliotecas como pandas, matplotlib ou scikit-learn.

Com o pybind11, o núcleo computacional continua rodando em C++ nativo (sem overhead de interpretador), mas o código de orquestração pode ser escrito em Python.

### Sem duplicação de código

A alternativa seria reescrever a lógica em Python, o que traria:

- Duplicação de código (dois sistemas para manter sincronizados)
- Perda de performance (Python puro é ordens de magnitude mais lento para loops numéricos)
- Risco de divergência (bugs corrigidos em um lado e não no outro)

Com bindings, existe uma única implementação (C++) e uma interface Python gerada automaticamente.

### Por que pybind11 e não outras alternativas

| Alternativa | Desvantagem |
|---|---|
| ctypes / cffi | Exigem interface C pura (sem classes, sem STL) e conversão manual de tipos |
| SWIG | Gera código intermediário complexo, configuração verbosa |
| Cython | Requer aprender uma linguagem híbrida (nem C++ nem Python) |
| **pybind11** | Header-only, sintaxe declarativa em C++, conversão automática da STL |

O pybind11 foi a escolha mais direta porque o projeto já usa C++ com STL (`std::vector`, `std::map`, `std::string`), e o pybind11 converte esses tipos automaticamente.

## Exemplo de uso

```python
import recommender as rec

compras = rec.carregar_compras('./data/dados_venda_cluster_0.csv')

print(f'Clientes: {len(compras.clientes)}')
print(f'Produtos: {len(compras.produtos)}')

intersecao = rec.calcular_similaridade(compras, otimizada=True)
jaccard = rec.calcular_jaccard(intersecao)

idx = compras.cliente_indice["99CL9Y01"]
recomendacoes = rec.recomendar_produtos(idx, 5, compras, jaccard)

for r in recomendacoes:
    print(f'[Score: {r.score:.6f}] {r.nome}')
```

Saída:

```
Clientes: 855
Produtos: 160
[Score: 0.000000] PEN DRIVE 16GB PD588 TWIST PRETO
[Score: 0.000000] PEN DRIVE 8GB PD587 TWIST PRETO
[Score: 0.000001] SUPORTE TV UNIVERSAL LCD/LED 10 A 71 SBRUB750
[Score: 0.000026] PEN DRIVE 8GB PD687 TWIST ROSA
[Score: 0.000200] PEN DRIVE SONY R 8GB USMRGR
```
