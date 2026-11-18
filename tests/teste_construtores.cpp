// Testes dos construtores de árvores (Pessoa 4).
// Rodar com: make testes

#include <cmath>
#include <iomanip>
#include <iostream>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "classificador.hpp"
#include "construtores.hpp"

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

// ----- dados pequenos montados à mão -----

Pergunta fazer_pergunta(int id, std::size_t n_alternativas) {
  Pergunta p;
  p.id = id;
  p.texto = "Pergunta " + std::to_string(id);
  for (std::size_t i = 0; i < n_alternativas; ++i) {
    p.alternativas.push_back({static_cast<char>('a' + i), "alt", {}});
  }
  return p;
}

Perfil fazer_perfil(int id, char q1, char q2, const std::string& pes) {
  Perfil p;
  p.id = id;
  p.treino = true;
  p.pes = pes;
  p.ciclo = "CICLO_ES";
  p.respostas = {{1, q1}, {2, q2}};
  return p;
}

// Q1 decide o PES; Q2 é independente do PES (ganho 0).
std::vector<Perfil> perfis_exemplo() {
  return {fazer_perfil(1, 'a', 'a', "PES_IOT"), fazer_perfil(2, 'a', 'b', "PES_IOT"),
          fazer_perfil(3, 'b', 'a', "PES_BIO"), fazer_perfil(4, 'b', 'b', "PES_BIO"),
          fazer_perfil(5, 'c', 'a', "PES_EDU"), fazer_perfil(6, 'c', 'b', "PES_EDU")};
}

std::vector<Pergunta> banco_exemplo() { return {fazer_pergunta(1, 3), fazer_pergunta(2, 2)}; }

std::string imprimir(const Arvore& a) {
  std::ostringstream out;
  a.imprimir_preordem(out);
  return out.str();
}

// ----- invariantes das árvores -----

// Nenhuma pergunta repete no caminho; todo nó interno tem um filho por alternativa.
bool estrutura_valida(const No& no, const std::vector<Pergunta>& perguntas, std::set<int>& caminho) {
  if (no.eh_folha()) return no.filhos.empty() && !no.resultado.empty();
  if (caminho.count(no.id_pergunta)) return false;

  std::size_t n_alt = 0;
  for (const Pergunta& p : perguntas) {
    if (p.id == no.id_pergunta) n_alt = p.alternativas.size();
  }
  if (no.filhos.size() != n_alt) return false;

  caminho.insert(no.id_pergunta);
  bool ok = true;
  for (const auto& filho : no.filhos) ok = ok && estrutura_valida(*filho, perguntas, caminho);
  caminho.erase(no.id_pergunta);
  return ok;
}

// Seção 3.1 do artigo: nó interno tem k filhos, com 3 <= k <= N = 4.
bool aridade_ok(const No& no) {
  if (no.eh_folha()) return true;
  if (no.filhos.size() < 3 || no.filhos.size() > 4) return false;
  for (const auto& filho : no.filhos) {
    if (!aridade_ok(*filho)) return false;
  }
  return true;
}

double acuracia(const Arvore& arvore, const std::vector<Perfil>& perfis, Alvo alvo) {
  int acertos = 0;
  for (const Perfil& p : perfis) {
    if (classificar(arvore, p.respostas).resultado == rotulo(p, alvo)) ++acertos;
  }
  return perfis.empty() ? 0.0 : static_cast<double>(acertos) / static_cast<double>(perfis.size());
}

// ----- entropia e ganho -----

void teste_entropia_e_ganho() {
  const std::vector<Perfil> ps = perfis_exemplo();
  const std::vector<Pergunta> bp = banco_exemplo();

  verificar(quase_igual(entropia({}, Alvo::PES), 0.0), "entropia de conjunto vazio e 0");
  verificar(quase_igual(entropia({ps[0], ps[1]}, Alvo::PES), 0.0), "entropia de conjunto puro e 0");
  verificar(quase_igual(entropia({ps[0], ps[2]}, Alvo::PES), 1.0), "entropia de 2 rotulos iguais e 1");
  verificar(quase_igual(entropia(ps, Alvo::PES), std::log2(3.0)), "entropia de 3 rotulos iguais e log2(3)");

  verificar(quase_igual(ganho_informacao(ps, bp[0], Alvo::PES), std::log2(3.0)),
            "ganho da pergunta decisiva = entropia total");
  verificar(quase_igual(ganho_informacao(ps, bp[1], Alvo::PES), 0.0),
            "ganho da pergunta independente = 0");
}

// ----- estratégias no exemplo pequeno -----

void teste_ganho_escolhe_a_pergunta_decisiva() {
  const Arvore a = construir_por_ganho(banco_exemplo(), perfis_exemplo(), Alvo::PES);
  verificar(a.raiz()->id_pergunta == 1, "ganho: raiz e a pergunta de maior ganho (P1)");
  verificar(a.altura() == 1, "ganho: altura 1 (P1 separa tudo)");
  verificar(a.contar_folhas() == 3, "ganho: 3 folhas");
  verificar(quase_igual(acuracia(a, perfis_exemplo(), Alvo::PES), 1.0), "ganho: acerta todo o treino");
}

void teste_manual_respeita_a_ordem() {
  const Arvore a = construir_ordem_manual(banco_exemplo(), perfis_exemplo(), {2, 1}, Alvo::PES);
  verificar(a.raiz()->id_pergunta == 2, "manual: raiz e a primeira da ordem (P2)");
  verificar(a.altura() == 2, "manual: ordem ruim gera arvore mais alta");
  verificar(quase_igual(acuracia(a, perfis_exemplo(), Alvo::PES), 1.0), "manual: acerta todo o treino");

  bool lancou = false;
  try {
    construir_ordem_manual(banco_exemplo(), perfis_exemplo(), {1, 99}, Alvo::PES);
  } catch (const std::invalid_argument&) {
    lancou = true;
  }
  verificar(lancou, "manual: id inexistente na ordem lanca invalid_argument");
}

void teste_aleatoria_reprodutivel() {
  const Arvore a1 = construir_ordem_aleatoria(banco_exemplo(), perfis_exemplo(), 42, Alvo::PES);
  const Arvore a2 = construir_ordem_aleatoria(banco_exemplo(), perfis_exemplo(), 42, Alvo::PES);
  verificar(imprimir(a1) == imprimir(a2), "aleatoria: mesma semente, mesma arvore");
  verificar(quase_igual(acuracia(a1, perfis_exemplo(), Alvo::PES), 1.0), "aleatoria: acerta todo o treino");
}

void teste_regra_de_parada_e_ramo_vazio() {
  // Ninguém responde 'c' em Q1: o ramo vazio vira folha com o rótulo do pai.
  const std::vector<Perfil> ps = {fazer_perfil(1, 'a', 'a', "PES_IOT"),
                                  fazer_perfil(2, 'a', 'b', "PES_IOT"),
                                  fazer_perfil(3, 'b', 'a', "PES_BIO")};
  const Arvore a = construir_por_ganho(banco_exemplo(), ps, Alvo::PES);
  const No* raiz = a.raiz();
  verificar(raiz->filhos.size() == 3, "ramo vazio: continua com 1 filho por alternativa");
  verificar(raiz->filhos[2]->eh_folha() && raiz->filhos[2]->resultado == "PES_IOT",
            "ramo vazio: folha com o rotulo mais comum do pai");

  // min_perfis alto: a raiz vira folha.
  const Arvore b = construir_por_ganho(banco_exemplo(), perfis_exemplo(), Alvo::PES, 100);
  verificar(b.raiz()->eh_folha(), "min_perfis alto: raiz vira folha");

  bool lancou = false;
  try {
    construir_por_ganho(banco_exemplo(), {}, Alvo::PES);
  } catch (const std::invalid_argument&) {
    lancou = true;
  }
  verificar(lancou, "treino vazio lanca invalid_argument");
}

// ----- camada do experimento -----

void teste_despacho_e_par() {
  const std::vector<Pergunta> bp = banco_exemplo();
  const std::vector<Perfil> ps = perfis_exemplo();

  verificar(std::string(nome_estrategia(Estrategia::MANUAL)) == "manual", "nome_estrategia: manual");
  verificar(std::string(nome_estrategia(Estrategia::ALEATORIA)) == "aleatoria", "nome_estrategia: aleatoria");
  verificar(std::string(nome_estrategia(Estrategia::GANHO)) == "ganho", "nome_estrategia: ganho");

  verificar(imprimir(construir(Estrategia::GANHO, bp, ps, Alvo::PES)) ==
                imprimir(construir_por_ganho(bp, ps, Alvo::PES)),
            "construir(GANHO) == construir_por_ganho");
  verificar(imprimir(construir(Estrategia::ALEATORIA, bp, ps, Alvo::PES, 7)) ==
                imprimir(construir_ordem_aleatoria(bp, ps, 7, Alvo::PES)),
            "construir(ALEATORIA, semente) == construir_ordem_aleatoria");

  const ParDeArvores par = construir_par(Estrategia::GANHO, bp, ps);
  verificar(!par.pes.vazia() && !par.ciclo.vazia(), "construir_par: as duas arvores existem");
  // No exemplo todos os perfis têm CICLO_ES: a árvore de ciclo é uma folha só.
  verificar(par.ciclo.raiz()->eh_folha() && par.ciclo.raiz()->resultado == "CICLO_ES",
            "construir_par: T_CICLO usa o rotulo de ciclo");
}

// ----- ordem manual do artigo -----

// A Seção 3.2 do artigo fixa esta ordem ANTES dos experimentos.
void teste_ordem_manual_do_artigo() {
  const std::vector<int> do_artigo = {1, 2, 3, 7, 8, 4, 12, 9, 10, 11, 6, 13, 14, 5};
  verificar(ordem_manual() == do_artigo, "ordem_manual() igual a ordem descrita no artigo (Secao 3.2)");
}

// ----- dados reais -----

void teste_dados_reais() {
  const std::vector<Pergunta> perguntas = carregar_perguntas("dados/perguntas.txt");
  const Estrategia estrategias[] = {Estrategia::MANUAL, Estrategia::ALEATORIA, Estrategia::GANHO};

  for (int pct : {10, 25, 40}) {
    const std::string base = "dados/sinteticos/ruido_" + std::to_string(pct) + "pct_";
    const std::vector<Perfil> treino = carregar_perfis(base + "treino.csv");
    const std::vector<Perfil> teste = carregar_perfis(base + "teste.csv");

    std::cout << "\n[ruido " << pct << "%]  alvo/estrategia   altura   nos  folhas  prof.media  acuracia\n";

    for (Alvo alvo : {Alvo::PES, Alvo::CICLO}) {
      const std::string nome_alvo = alvo == Alvo::PES ? "PES  " : "CICLO";
      for (Estrategia e : estrategias) {
        const Arvore a = construir(e, perguntas, treino, alvo, 42);
        const double acc = acuracia(a, teste, alvo);

        std::cout << "  " << nome_alvo << " " << std::left << std::setw(10) << nome_estrategia(e)
                  << std::right << std::setw(9) << a.altura() << std::setw(7) << a.contar_nos()
                  << std::setw(8) << a.contar_folhas() << std::fixed << std::setprecision(2)
                  << std::setw(12) << a.profundidade_media_folhas() << std::setw(10) << acc << '\n';

        const std::string contexto = "ruido " + std::to_string(pct) + "%, " + nome_alvo + ", " +
                                     nome_estrategia(e);
        std::set<int> caminho;
        verificar(estrutura_valida(*a.raiz(), perguntas, caminho),
                  contexto + ": estrutura valida, sem pergunta repetida");
        verificar(aridade_ok(*a.raiz()), contexto + ": nos internos com 3 a 4 filhos");
        verificar(a.altura() <= static_cast<int>(perguntas.size()),
                  contexto + ": altura <= numero de perguntas");
        if (pct == 10) verificar(acc > 0.5, contexto + ": acuracia no teste acima de 50%");
      }
    }
  }
  std::cout << '\n';
}

}  // namespace

int main() {
  teste_entropia_e_ganho();
  teste_ganho_escolhe_a_pergunta_decisiva();
  teste_manual_respeita_a_ordem();
  teste_aleatoria_reprodutivel();
  teste_regra_de_parada_e_ramo_vazio();
  teste_despacho_e_par();
  teste_ordem_manual_do_artigo();
  teste_dados_reais();

  std::cout << (total - falhas) << "/" << total << " testes passaram\n";
  return falhas == 0 ? 0 : 1;
}
