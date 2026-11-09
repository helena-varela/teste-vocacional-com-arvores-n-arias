// Testes unitários da estrutura N-ária e da leitura de respostas.
// Rodar com: make testes

#include <cmath>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "arvore.hpp"
#include "exemplo.hpp"
#include "terminal.hpp"

namespace {

int falhas = 0;
int total = 0;

void verificar(bool condicao, const std::string& descricao) {
    ++total;
    if (!condicao) {
        ++falhas;
        std::cout << "FALHOU: " << descricao << '\n';
    }
}

bool quase_igual(double a, double b) { return std::fabs(a - b) < 1e-9; }

void teste_arvore_vazia() {
    Arvore a;
    verificar(a.vazia(), "arvore nova e vazia");
    verificar(a.altura() == -1, "altura da arvore vazia e -1");
    verificar(a.contar_nos() == 0, "arvore vazia tem 0 nos");
    verificar(a.contar_folhas() == 0, "arvore vazia tem 0 folhas");
    verificar(quase_igual(a.profundidade_media_folhas(), 0.0), "profundidade media da arvore vazia e 0");
}

void teste_so_raiz() {
    Arvore a(criar_folha("Jogos Digitais"));
    verificar(a.altura() == 0, "arvore so com a raiz tem altura 0");
    verificar(a.contar_nos() == 1, "arvore so com a raiz tem 1 no");
    verificar(a.contar_folhas() == 1, "arvore so com a raiz (folha) tem 1 folha");
}

void teste_arvore_exemplo_pes() {
    const Arvore a = exemplo::arvore_pes();
    verificar(a.altura() == 2, "altura da arvore de PES de exemplo e 2");
    verificar(a.contar_nos() == 8, "arvore de PES de exemplo tem 8 nos");
    verificar(a.contar_folhas() == 6, "arvore de PES de exemplo tem 6 folhas");
    // Folhas: 3 na profundidade 1 e 3 na profundidade 2 -> (3 + 6) / 6 = 1.5
    verificar(quase_igual(a.profundidade_media_folhas(), 1.5), "profundidade media das folhas e 1.5");

    const No* raiz = a.raiz();
    verificar(raiz->filhos.size() == 4, "raiz tem 4 filhos (a, b, c, d)");
    verificar(raiz->filhos[1]->id_pergunta == 2, "alternativa b leva a pergunta 2");
    verificar(raiz->filhos[0]->resultado == "Internet das Coisas", "alternativa a leva a Internet das Coisas");
}

void teste_impressao() {
    const Arvore a = exemplo::arvore_ciclo();

    std::ostringstream pre;
    a.imprimir_preordem(pre);
    const std::string esperado_pre =
        "[P3] Voce prefere estudar teoria e matematica ou construir produtos?\n"
        "    a) Teoria e matematica -> [P4] Voce gosta da ideia de criar modelos que aprendem com dados?\n"
        "        a) Sim => Inteligencia Artificial\n"
        "        b) Nao => Ciencia da Computacao\n"
        "    b) Construir produtos => Engenharia de Software\n";
    verificar(pre.str() == esperado_pre, "pre-ordem imprime na ordem raiz, filhos da esquerda para a direita");

    std::ostringstream bfs;
    a.imprimir_por_nivel(bfs);
    const std::string saida = bfs.str();
    verificar(saida.find("Nivel 0:") < saida.find("Nivel 1:") && saida.find("Nivel 1:") < saida.find("Nivel 2:"),
              "BFS imprime os niveis em ordem");
    verificar(saida.find("Engenharia de Software") < saida.find("Inteligencia Artificial"),
              "BFS mostra o nivel 1 inteiro antes do nivel 2");
}

void teste_folha_nao_recebe_filho() {
    auto folha = criar_folha("Bioinformatica");
    bool lancou = false;
    try {
        adicionar_filho(*folha, criar_folha("Outra"), "a) x");
    } catch (const std::invalid_argument&) {
        lancou = true;
    }
    verificar(lancou, "adicionar filho em folha lanca invalid_argument");
}

void teste_ler_opcao() {
    std::ostringstream descarte;

    std::istringstream entrada1("x\ne\n  B  \n");
    verificar(ler_opcao(3, entrada1, descarte) == 'b', "ler_opcao ignora invalidas e aceita maiuscula com espacos");

    std::istringstream entrada2("ab\n\nc\n");
    verificar(ler_opcao(3, entrada2, descarte) == 'c', "ler_opcao rejeita mais de uma letra e linha vazia");

    std::istringstream entrada3("");
    verificar(ler_opcao(4, entrada3, descarte) == '\0', "ler_opcao devolve '\\0' no fim da entrada");

    std::istringstream entrada4("5\nabc\n-1\n3\n");
    verificar(ler_numero(0, 4, entrada4, descarte) == 3, "ler_numero ignora fora do intervalo e nao numeros");

    std::istringstream entrada5("");
    verificar(ler_numero(0, 4, entrada5, descarte) == -1, "ler_numero devolve minimo-1 no fim da entrada");
}

}  // namespace

int main() {
    teste_arvore_vazia();
    teste_so_raiz();
    teste_arvore_exemplo_pes();
    teste_impressao();
    teste_folha_nao_recebe_filho();
    teste_ler_opcao();

    std::cout << (total - falhas) << "/" << total << " testes passaram\n";
    return falhas == 0 ? 0 : 1;
}
