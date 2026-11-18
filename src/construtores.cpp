#include "construtores.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <functional>
#include <map>
#include <numeric>
#include <random>
#include <stdexcept>
#include <string>

// Complexidade (n = perfis de treino, m = perguntas, L = rótulos, constante):
//  - dividir os perfis de um nó por uma pergunta: O(p log m), pois cada perfil
//    guarda as respostas num std::map; p = perfis que chegaram ao nó.
//  - manual/aleatória: uma divisão por nó. Cada nível particiona no máximo n
//    perfis e há no máximo m níveis, logo O(n m log m).
//  - ganho (ID3): em cada nó avalia as r <= m perguntas restantes, logo
//    O(n m^2 log m) no pior caso.
// (Cota superior.)

namespace {

using Ponteiros = std::vector<const Perfil*>;

// Devolve a POSIÇÃO (dentro de `restantes`) da pergunta que o nó vai usar.
using Escolha = std::function<std::size_t(const Ponteiros&, const std::vector<int>&)>;

// O que é igual em todos os nós da mesma construção.
struct Contexto {
  const std::vector<Pergunta>& perguntas;
  Alvo alvo;
  int min_perfis;
  Escolha escolher;
};

Ponteiros apontar(const std::vector<Perfil>& perfis) {
  Ponteiros ps;
  ps.reserve(perfis.size());
  for (const Perfil& p : perfis) ps.push_back(&p);
  return ps;
}

std::map<std::string, int> contar_rotulos(const Ponteiros& ps, Alvo alvo) {
  std::map<std::string, int> contagem;
  for (const Perfil* p : ps) ++contagem[rotulo(*p, alvo)];
  return contagem;
}

double entropia_ptr(const Ponteiros& ps, Alvo alvo) {
  if (ps.empty()) return 0.0;
  const double n = static_cast<double>(ps.size());
  double h = 0.0;
  for (const auto& [rot, c] : contar_rotulos(ps, alvo)) {
    const double prob = c / n;
    h -= prob * std::log2(prob);
  }
  return h;
}

// Rótulo mais frequente; no empate, o menor em ordem alfabética.
std::string mais_comum(const Ponteiros& ps, Alvo alvo) {
  std::string melhor;
  int max = 0;
  for (const auto& [rot, c] : contar_rotulos(ps, alvo)) {
    if (c > max) {
      max = c;
      melhor = rot;
    }
  }
  return melhor;
}

bool todos_iguais(const Ponteiros& ps, Alvo alvo) {
  for (const Perfil* p : ps) {
    if (rotulo(*p, alvo) != rotulo(*ps.front(), alvo)) return false;
  }
  return true;
}

// Separa os perfis pela resposta à pergunta `id` (índice 0 = 'a').
// Perfil sem resposta válida para a pergunta é ignorado.
std::vector<Ponteiros> dividir(const Ponteiros& ps, int id, std::size_t n_alternativas) {
  std::vector<Ponteiros> partes(n_alternativas);
  for (const Perfil* p : ps) {
    const auto it = p->respostas.find(id);
    if (it == p->respostas.end()) continue;
    const int indice = it->second - 'a';
    if (indice < 0 || indice >= static_cast<int>(n_alternativas)) continue;
    partes[static_cast<std::size_t>(indice)].push_back(p);
  }
  return partes;
}

// Gain(S, q) = H(S) - soma_v (|S_v| / |S|) * H(S_v)
double ganho_ptr(const Ponteiros& ps, const Pergunta& pergunta, Alvo alvo) {
  const std::vector<Ponteiros> partes = dividir(ps, pergunta.id, pergunta.alternativas.size());

  Ponteiros cobertos;  // perfis que têm resposta para esta pergunta
  for (const Ponteiros& parte : partes) cobertos.insert(cobertos.end(), parte.begin(), parte.end());
  if (cobertos.empty()) return 0.0;

  const double total = static_cast<double>(cobertos.size());
  double h_depois = 0.0;
  for (const Ponteiros& parte : partes) {
    if (!parte.empty()) h_depois += (static_cast<double>(parte.size()) / total) * entropia_ptr(parte, alvo);
  }
  return entropia_ptr(cobertos, alvo) - h_depois;
}

// Núcleo comum às três estratégias: só `ctx.escolher` muda.
// `restantes` = índices (em ctx.perguntas) ainda não usados neste caminho.
std::unique_ptr<No> construir_rec(const Contexto& ctx, const Ponteiros& perfis,
                                  const std::vector<int>& restantes,
                                  const std::string& rotulo_pai) {
  // Ramo sem perfis: rótulo mais comum do pai.
  if (perfis.empty()) return criar_folha(rotulo_pai);

  const std::string comum = mais_comum(perfis, ctx.alvo);
  if (todos_iguais(perfis, ctx.alvo) || restantes.empty() ||
      static_cast<int>(perfis.size()) < ctx.min_perfis) {
    return criar_folha(comum);
  }

  const std::size_t pos = ctx.escolher(perfis, restantes);
  const Pergunta& pergunta = ctx.perguntas[static_cast<std::size_t>(restantes[pos])];

  std::vector<int> proximas = restantes;  // sem a pergunta usada
  proximas.erase(proximas.begin() + static_cast<std::ptrdiff_t>(pos));

  auto no = criar_no_pergunta(pergunta.id, pergunta.texto);
  const std::vector<Ponteiros> partes = dividir(perfis, pergunta.id, pergunta.alternativas.size());

  // Um filho por alternativa, mesmo quando nenhum perfil chega nela.
  for (std::size_t i = 0; i < pergunta.alternativas.size(); ++i) {
    const Alternativa& alt = pergunta.alternativas[i];
    adicionar_filho(*no, construir_rec(ctx, partes[i], proximas, comum),
                    std::string(1, alt.letra) + ") " + alt.texto);
  }
  return no;
}

Arvore construir_com(const std::vector<Pergunta>& perguntas, const std::vector<Perfil>& treino,
                     const std::vector<int>& ordem_indices, Alvo alvo, int min_perfis,
                     Escolha escolher) {
  if (perguntas.empty()) throw std::invalid_argument("construtores: banco de perguntas vazio");
  if (treino.empty()) throw std::invalid_argument("construtores: conjunto de treino vazio");

  const Contexto ctx{perguntas, alvo, min_perfis, std::move(escolher)};
  const Ponteiros ps = apontar(treino);
  return Arvore(construir_rec(ctx, ps, ordem_indices, mais_comum(ps, alvo)));
}

std::vector<int> todos_os_indices(const std::vector<Pergunta>& perguntas) {
  std::vector<int> indices(perguntas.size());
  std::iota(indices.begin(), indices.end(), 0);
  return indices;
}

// Manual e aleatória: o nó usa sempre a próxima pergunta da lista.
const Escolha PRIMEIRA_DA_LISTA = [](const Ponteiros&, const std::vector<int>&) -> std::size_t {
  return 0;
};

}  // namespace

// ---------------------------------------------------------------------------
// API de construção
// ---------------------------------------------------------------------------

double entropia(const std::vector<Perfil>& perfis, Alvo alvo) {
  return entropia_ptr(apontar(perfis), alvo);
}

double ganho_informacao(const std::vector<Perfil>& perfis, const Pergunta& pergunta, Alvo alvo) {
  return ganho_ptr(apontar(perfis), pergunta, alvo);
}

Arvore construir_ordem_manual(const std::vector<Pergunta>& perguntas,
                              const std::vector<Perfil>& treino, const std::vector<int>& ordem,
                              Alvo alvo, int min_perfis) {
  std::vector<int> indices;
  for (int id : ordem) {
    const auto it = std::find_if(perguntas.begin(), perguntas.end(),
                                 [id](const Pergunta& p) { return p.id == id; });
    if (it == perguntas.end()) {
      throw std::invalid_argument("construir_ordem_manual: id " + std::to_string(id) +
                                  " da ordem nao existe no banco de perguntas");
    }
    indices.push_back(static_cast<int>(it - perguntas.begin()));
  }
  return construir_com(perguntas, treino, indices, alvo, min_perfis, PRIMEIRA_DA_LISTA);
}

Arvore construir_ordem_aleatoria(const std::vector<Pergunta>& perguntas,
                                 const std::vector<Perfil>& treino, unsigned semente, Alvo alvo,
                                 int min_perfis) {
  std::vector<int> indices = todos_os_indices(perguntas);
  std::mt19937 gerador(semente);
  std::shuffle(indices.begin(), indices.end(), gerador);
  return construir_com(perguntas, treino, indices, alvo, min_perfis, PRIMEIRA_DA_LISTA);
}

Arvore construir_por_ganho(const std::vector<Pergunta>& perguntas,
                           const std::vector<Perfil>& treino, Alvo alvo, int min_perfis) {
  const Escolha por_ganho = [&perguntas, alvo](const Ponteiros& ps,
                                               const std::vector<int>& restantes) -> std::size_t {
    std::size_t melhor = 0;
    double melhor_ganho = -1.0;
    for (std::size_t i = 0; i < restantes.size(); ++i) {
      const double g = ganho_ptr(ps, perguntas[static_cast<std::size_t>(restantes[i])], alvo);
      if (g > melhor_ganho + 1e-12) {  // estrito: no empate fica a primeira do banco
        melhor_ganho = g;
        melhor = i;
      }
    }
    return melhor;
  };
  return construir_com(perguntas, treino, todos_os_indices(perguntas), alvo, min_perfis, por_ganho);
}

// ---------------------------------------------------------------------------
// Camada do experimento
// ---------------------------------------------------------------------------

const char* nome_estrategia(Estrategia estrategia) {
  switch (estrategia) {
    case Estrategia::MANUAL: return "manual";
    case Estrategia::ALEATORIA: return "aleatoria";
    case Estrategia::GANHO: return "ganho";
  }
  throw std::invalid_argument("nome_estrategia: estrategia desconhecida");
}

Arvore construir(Estrategia estrategia, const std::vector<Pergunta>& perguntas,
                 const std::vector<Perfil>& treino, Alvo alvo, unsigned semente, int min_perfis) {
  switch (estrategia) {
    case Estrategia::MANUAL:
      return construir_ordem_manual(perguntas, treino, ordem_manual(), alvo, min_perfis);
    case Estrategia::ALEATORIA:
      return construir_ordem_aleatoria(perguntas, treino, semente, alvo, min_perfis);
    case Estrategia::GANHO:
      return construir_por_ganho(perguntas, treino, alvo, min_perfis);
  }
  throw std::invalid_argument("construir: estrategia desconhecida");
}

ParDeArvores construir_par(Estrategia estrategia, const std::vector<Pergunta>& perguntas,
                           const std::vector<Perfil>& treino, unsigned semente, int min_perfis) {
  return ParDeArvores{construir(estrategia, perguntas, treino, Alvo::PES, semente, min_perfis),
                      construir(estrategia, perguntas, treino, Alvo::CICLO, semente, min_perfis)};
}
