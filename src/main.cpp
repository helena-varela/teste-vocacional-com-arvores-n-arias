#include <iostream>

#include "arvore.hpp"
#include "exemplo.hpp"
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
    std::cout << "\nAltura: " << arvore.altura()
              << " | Nos: " << arvore.contar_nos()
              << " | Folhas: " << arvore.contar_folhas()
              << " | Profundidade media das folhas: " << arvore.profundidade_media_folhas() << '\n';
}

}  // namespace

int main() {
    // PROVISORIO: arvores de exemplo montadas a mao. Quando os construtores
    // (Pessoa 4) estiverem prontos, estas linhas passam a usar as arvores reais.
    const Arvore arvore_pes = exemplo::arvore_pes();
    const Arvore arvore_ciclo = exemplo::arvore_ciclo();

    while (true) {
        mostrar_menu();
        const int opcao = ler_numero(0, 4);
        switch (opcao) {
            case 1:
                // TODO (Pessoa 5): chamar o teste interativo com as duas arvores.
                std::cout << "Teste interativo ainda nao implementado (Pessoa 5).\n";
                break;
            case 2:
                mostrar_arvore(arvore_pes, "PES");
                break;
            case 3:
                mostrar_arvore(arvore_ciclo, "2o ciclo");
                break;
            case 4:
                // TODO (Pessoa 5): chamar o script de experimentos.
                std::cout << "Experimentos ainda nao implementados (Pessoa 5).\n";
                break;
            default:  // 0 ou fim da entrada
                std::cout << "Ate mais!\n";
                return 0;
        }
    }
}
