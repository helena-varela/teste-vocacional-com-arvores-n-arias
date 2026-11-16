#pragma once

#include <map>
#include <string>
#include <vector>

#include "arvore.hpp"

// Resultado de percorrer uma árvore da raiz até uma folha (Pessoa 5).
struct Classificacao {
  std::string resultado;          // rótulo da folha alcançada
  std::vector<int> caminho;       // ids das perguntas feitas, na ordem
  int perguntas_feitas() const { return static_cast<int>(caminho.size()); }
};

// Desce da raiz até uma folha usando as respostas já conhecidas
// (id da pergunta -> letra 'a'..'d'). Serve tanto para os perfis
// simulados quanto para o cache de respostas do teste interativo.
// Lança std::invalid_argument se a árvore estiver vazia, se faltar a
// resposta de uma pergunta do caminho ou se a letra não tiver filho.
Classificacao classificar(const Arvore& arvore,
                          const std::map<int, char>& respostas);
