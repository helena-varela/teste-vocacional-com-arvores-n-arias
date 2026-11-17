// Testes de carregar_perfis (perfis.cpp) e classificar (classificador.cpp).
// Rodar com: make testes

#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

#include "classificador.hpp"
#include "exemplo.hpp"
#include "perfis.hpp"

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

// ----- classificar -----

void teste_classificar_caminho_normal() {
    const Arvore a = exemplo::arvore_pes();
    const Classificacao c = classificar(a, {{1, 'b'}, {2, 'c'}});
    verificar(c.resultado == "Ciencia de Dados", "classificar: resultado da resposta b -> c");
    verificar(c.caminho == std::vector<int>{1, 2}, "classificar: caminho [1, 2]");
    verificar(c.perguntas_feitas() == 2, "classificar: 2 perguntas feitas");
}

void teste_classificar_folha_direta() {
    const Arvore a = exemplo::arvore_pes();
    const Classificacao c = classificar(a, {{1, 'a'}});
    verificar(c.resultado == "Internet das Coisas", "classificar: resposta a vai direto para folha");
    verificar(c.perguntas_feitas() == 1, "classificar: 1 pergunta feita");
}

void teste_classificar_ignora_respostas_extras() {
    const Arvore a = exemplo::arvore_pes();
    const Classificacao c = classificar(a, {{1, 'c'}, {2, 'a'}, {99, 'd'}});
    verificar(c.resultado == "Jogos Digitais", "classificar: respostas fora do caminho sao ignoradas");
    verificar(c.perguntas_feitas() == 1, "classificar: so conta perguntas do caminho");
}

void teste_classificar_erros() {
    const Arvore a = exemplo::arvore_pes();

    bool lancou = false;
    try { classificar(a, {{1, 'b'}}); } catch (const std::invalid_argument&) { lancou = true; }
    verificar(lancou, "classificar: lanca se falta resposta no caminho");

    lancou = false;
    try { classificar(a, {{1, 'z'}}); } catch (const std::invalid_argument&) { lancou = true; }
    verificar(lancou, "classificar: lanca com letra sem filho");

    lancou = false;
    const Arvore vazia;
    try { classificar(vazia, {}); } catch (const std::invalid_argument&) { lancou = true; }
    verificar(lancou, "classificar: lanca com arvore vazia");
}

// ----- carregar_perfis -----

void teste_carregar_perfis_csv_real() {
    const std::vector<Perfil> perfis = carregar_perfis("dados/sinteticos/ruido_10pct_teste.csv");
    verificar(!perfis.empty(), "carregar_perfis: le pelo menos um perfil");
    if (perfis.empty()) return;

    const Perfil& p = perfis[0];
    verificar(p.id == 1, "carregar_perfis: id do primeiro perfil");
    verificar(!p.treino, "carregar_perfis: arquivo de teste tem divisao = teste");
    verificar(p.pes == "PES_JOGOS", "carregar_perfis: pes do primeiro perfil");
    verificar(p.ciclo == "CICLO_IA", "carregar_perfis: ciclo do primeiro perfil");
    verificar(p.respostas.size() == 14, "carregar_perfis: 14 respostas");
    verificar(p.respostas.count(1) && p.respostas.at(1) == 'c', "carregar_perfis: Q1 = c");
    verificar(rotulo(p, Alvo::PES) == "PES_JOGOS", "rotulo: alvo PES");
    verificar(rotulo(p, Alvo::CICLO) == "CICLO_IA", "rotulo: alvo CICLO");
}

void teste_carregar_perfis_treino() {
    const std::vector<Perfil> perfis = carregar_perfis("dados/sinteticos/ruido_10pct_treino.csv");
    bool todos_treino = !perfis.empty();
    for (const Perfil& p : perfis) {
        if (!p.treino) todos_treino = false;
    }
    verificar(todos_treino, "carregar_perfis: arquivo de treino tem divisao = treino");
}

void teste_carregar_perfis_arquivo_inexistente() {
    bool lancou = false;
    try { carregar_perfis("dados/nao_existe.csv"); } catch (const std::runtime_error&) { lancou = true; }
    verificar(lancou, "carregar_perfis: lanca se o arquivo nao existe");
}

}  // namespace

int main() {
    teste_classificar_caminho_normal();
    teste_classificar_folha_direta();
    teste_classificar_ignora_respostas_extras();
    teste_classificar_erros();
    teste_carregar_perfis_csv_real();
    teste_carregar_perfis_treino();
    teste_carregar_perfis_arquivo_inexistente();

    std::cout << (total - falhas) << "/" << total << " testes passaram\n";
    return falhas == 0 ? 0 : 1;
}