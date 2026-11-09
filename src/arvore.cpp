#include "arvore.hpp"
#include <algorithm>
#include <ostream>
#include <queue>
#include <stdexcept>
#include <utility>

std::unique_ptr<No> criar_no_pergunta(int id_pergunta, std::string texto) {
  if (id_pergunta < 0) {
    throw std::invalid_argument("id_pergunta deve ser >= 0");
  }

  auto no = std::make_unique<No>();
  no->id_pergunta = id_pergunta;
  no->texto = std::move(texto);
  return no;
}

std::unique_ptr<No> criar_folha(std::string resultado) {
  auto no = std::make_unique<No>();
  no->resultado = std::move(resultado);
  return no;
}

No* adicionar_filho(No& pai, std::unique_ptr<No> filho, std::string rotulo_resposta) {
  if (pai.eh_folha()) {
    throw std::invalid_argument("uma folha nao pode receber filhos");
  }

  if (!filho) {
    throw std::invalid_argument("filho nulo");
  }

  filho->rotulo_resposta = std::move(rotulo_resposta);
  pai.filhos.push_back(std::move(filho));
  return pai.filhos.back().get();
}

Arvore::Arvore(std::unique_ptr<No> raiz) : raiz_(std::move(raiz)) {}

void Arvore::definir_raiz(std::unique_ptr<No> raiz) { raiz_ = std::move(raiz); }

namespace {

  // uma linha de descrição do nó
  void descrever(const No& no, std::ostream& out) {
    if (!no.rotulo_resposta.empty()) {
      out << no.rotulo_resposta << (no.eh_folha() ? " => " : " -> ");
    } else if (no.eh_folha()) {
      out << "=> ";
    }
    if (no.eh_folha()) {
      out << no.resultado;
    } else {
      out << "[P" << no.id_pergunta << "] " << no.texto;
    }
  }

  void preordem(const No& no, int nivel, std::ostream& out) {
    out << std::string(static_cast<size_t>(nivel) * 4, ' ');
    descrever(no, out);
    out << '\n';
    for (const auto& filho : no.filhos) {
      preordem(*filho, nivel + 1, out);
    }
  }

  int altura_rec(const No& no) {
    int maior = -1;
    for (const auto& filho : no.filhos) {
      maior = std::max(maior, altura_rec(*filho));
    }
    return maior + 1;  // sem filhos: -1 + 1 = 0
  }

  int contar_nos_rec(const No& no) {
    int total = 1;
    for (const auto& filho : no.filhos) {
      total += contar_nos_rec(*filho);
    }
    return total;
  }

  // acumula o número de folhas e a soma das profundidades delas
  void somar_folhas(const No& no, int profundidade, int& folhas, long long& soma) {
    if (no.eh_folha()) {
      ++folhas;
      soma += profundidade;
      return;
    }
    for (const auto& filho : no.filhos) {
      somar_folhas(*filho, profundidade + 1, folhas, soma);
    }
  }

}

void Arvore::imprimir_preordem(std::ostream& out) const {
  if (vazia()) {
    out << "(arvore vazia)\n";
    return;
  }
  preordem(*raiz_, 0, out);
}

void Arvore::imprimir_por_nivel(std::ostream& out) const {
  if (vazia()) {
    out << "(arvore vazia)\n";
    return;
  }
  std::queue<std::pair<const No*, int>> fila;
  fila.push({raiz_.get(), 0});
  int nivel_atual = -1;

  while (!fila.empty()) {
    auto [no, nivel] = fila.front();
    fila.pop();
    if (nivel != nivel_atual) {
      nivel_atual = nivel;
      out << "Nivel " << nivel << ":\n";
    }
    out << "    ";
    descrever(*no, out);
    out << '\n';
    for (const auto& filho : no->filhos) {
      fila.push({filho.get(), nivel + 1});
    }
  }
}

int Arvore::altura() const { return vazia() ? -1 : altura_rec(*raiz_); }

int Arvore::contar_nos() const { return vazia() ? 0 : contar_nos_rec(*raiz_); }

int Arvore::contar_folhas() const {
  if (vazia()) return 0;
  int folhas = 0;
  long long soma = 0;
  somar_folhas(*raiz_, 0, folhas, soma);
  return folhas;
}

double Arvore::profundidade_media_folhas() const {
  if (vazia()) return 0.0;
  int folhas = 0;
  long long soma = 0;
  somar_folhas(*raiz_, 0, folhas, soma);
  return folhas == 0 ? 0.0 : static_cast<double>(soma) / folhas;
}
