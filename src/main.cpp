#include <exception>
#include <iostream>
#include <vector>

#include "arvore.hpp"
#include "exemplo.hpp"
#include "experimento.hpp"
#include "perguntas.hpp"
#include "terminal.hpp"

namespace {
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
  // isso eh provisorio, qnd os construtores estiverem pronto aqui sera as arvores reais
  const Arvore arvore_pes = exemplo::arvore_pes();
  const Arvore arvore_ciclo = exemplo::arvore_ciclo();

  // O caminho é relativo: o programa precisa rodar a partir da raiz do
  // projeto (é o que o `make run` faz).
  std::vector<Pergunta> perguntas;
  try {
    perguntas = carregar_perguntas("dados/perguntas.txt");
  } catch (const std::exception& erro) {
    std::cerr << "Erro ao carregar as perguntas: " << erro.what() << "\n"
              << "Rode o programa a partir da pasta do projeto (ex.: make run).\n";
    return 1;
  }

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
        // chamar o script de experimentos.
        std::cout << "Experimentos ainda nao implementados (Pessoa 5).\n";
        break;
      default:
        std::cout << "Ate mais!\n";
        return 0;
    }
  }
}
