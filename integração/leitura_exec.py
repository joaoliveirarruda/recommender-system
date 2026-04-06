import meu_modulo

def executar_testes_do_print():
    print("--- Testes Baseados no Print ---")
    
    # 1. Testando list -> std::vector<int>
    v = [1, 2, 3, 4, 5]
    soma = meu_modulo.test_vector(v)
    print(f"Soma do vector (C++): {soma}")

    # 2. Testando retorno de std::vector<int> -> list
    lista_gerada = meu_modulo.test_vector_return(10)
    print(f"Lista gerada no C++: {lista_gerada}")

    # 3. Testando dict -> std::map<string, int>
    m = {"a": 1, "b": 2}
    mapa_modificado = meu_modulo.test_map(m)
    print(f"Dicionário modificado no C++: {mapa_modificado}")

    # 4. Testando list de tuple -> vector de tuple
    v_tuple = [(1, 1.1), (2, 2.2)]
    resultado_tuplas = meu_modulo.test_vector_tuple(v_tuple)
    print(f"Lista de tuplas processada: {resultado_tuplas}\n")


def processar_sistema_recomendacao(caminho_arquivo):
    print("--- Processando Sistema de Recomendação ---")
    try:
        with open(caminho_arquivo, 'r') as arquivo:
            for linha in arquivo:
                # Exemplo: Supondo que o arquivo tenha "id_cliente, id_produto"
                dados = linha.strip().split(',')
                if len(dados) == 2:
                    id_cli = int(dados[0])
                    id_prod = int(dados[1])
                    
                    # Chama a função real do seu solver (baseada no código anterior)
                    # resultado = meu_modulo.calcular_recomendacao(id_cli, id_prod)
                    # print(f"Cliente {id_cli} -> Score: {resultado}")
                    print(f"Lendo dados: Cliente {id_cli}, Produto {id_prod}")
                    
    except FileNotFoundError:
        print(f"Erro: O arquivo {caminho_arquivo} não foi encontrado.")

if __name__ == "__main__":
    # Roda os testes de conversão de tipos do print
    executar_testes_do_print()
    
    # Roda a lógica de leitura de arquivos da atividade
    processar_sistema_recomendacao("dados_clientes.txt")