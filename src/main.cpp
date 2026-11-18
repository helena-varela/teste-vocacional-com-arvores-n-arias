#include <exception>
#include <iostream>
#include <vector>

#include "arvore.hpp"
#include "construtores.hpp"
#include "experimento.hpp"
#include "perfis.hpp"
#include "perguntas.hpp"
#include "terminal.hpp"

namespace {
  // Perfis usados para construir as árvores do teste interativo. O cenário de
  // 10% de ruído é o que tem o sinal mais limpo de cada combinação PES+ciclo.
  const char* const ARQUIVO_TREINO = "dados/sinteticos/ruido_10pct_treino.csv";

  void mostrar_menu() {
    std::cout << "\n========================================\n"
                "   TESTE VOCACIONAL - PES e 2o CICLO\n"
                "========================================\n"
                "1) Fazer o teste vocacional\n"
                "2) Mostrar a arvore de PES\n"
                "3) Mostrar a arvore de 2o ciclo\n"
                "4) Rodar os experimentos (gera os CSVs)\n"
                "0) Sair\n";
  }

  void mostrar_arvore(const Arvore& arvore, const char* nome) {
    std::cout << "\n--- Arvore de " << nome << " (pre-ordem) ---\n";
    arvore.imprimir_preordem(std::cout);
    std::cout << "\nAltura: " << arvore.altura() << " | Nos: " << arvore.contar_nos() << " | Folhas: " << arvore.contar_folhas() << " | Profundidade media das folhas: " << arvore.profundidade_media_folhas() << '\n';
  }
}

int main() {
  // Os caminhos são relativos: o programa precisa rodar a partir da raiz do
  // projeto (é o que o `make run` faz).
  std::vector<Pergunta> perguntas;
  std::vector<Perfil> treino;
  try {
    perguntas = carregar_perguntas("dados/perguntas.txt");
    treino = carregar_perfis(ARQUIVO_TREINO);
  } catch (const std::exception& erro) {
    std::cerr << "Erro ao carregar os dados: " << erro.what() << "\n"
              << "Rode o programa a partir da pasta do projeto (ex.: make run).\n";
    return 1;
  }

  // Árvores do teste: estratégia de ganho de informação (ID3).
  const ParDeArvores arvores = construir_par(Estrategia::GANHO, perguntas, treino);
  const Arvore& arvore_pes = arvores.pes;
  const Arvore& arvore_ciclo = arvores.ciclo;

  while (true) {
    mostrar_menu();
    const int opcao = ler_numero(0, 4);
    switch (opcao) {
      case 1:
        executar_teste_interativo(arvore_pes, arvore_ciclo, perguntas);
        break;
      case 2:
        mostrar_arvore(arvore_pes, "PES");
        break;
      case 3:
        mostrar_arvore(arvore_ciclo, "2o ciclo");
        break;
      case 4:
        // Falta executar_experimentos() (Pessoa 5). Quando existir:
        // executar_experimentos("dados/sinteticos", "resultados");
        std::cout << "Experimentos ainda nao implementados (Pessoa 5).\n";
        break;
      default:
        std::cout << "Ate mais!\n";
        return 0;
    }
  }
}
