#pragma once

#include <vector>

#include "arvore.hpp"
#include "perfis.hpp"
#include "perguntas.hpp"

// Construtores das árvores (Pessoa 4).
//
// As três estratégias usam os perfis de TREINO e a MESMA regra de parada;
// só muda a escolha da pergunta de cada nó:
//   - manual:    a próxima pergunta da ordem definida em ordem_manual()
//   - aleatória: a próxima pergunta de uma ordem embaralhada com `semente`
//   - ganho:     a pergunta restante de maior ganho de informação (ID3)
//
// Regra de parada (o nó vira folha) quando:
//   - todos os perfis que chegaram ao nó têm o mesmo rótulo do alvo;
//   - não sobram perguntas no caminho; ou
//   - chegaram menos de `min_perfis` perfis.
// A folha recebe o rótulo mais comum entre os perfis que chegaram a ela.
// Um ramo em que não chega nenhum perfil vira folha com o rótulo mais comum
// do PAI. Assim todo nó de pergunta tem exatamente um filho por alternativa.
//
// Nenhuma pergunta se repete num mesmo caminho da raiz até a folha.

Arvore construir_ordem_manual(const std::vector<Pergunta>& perguntas,
                              const std::vector<Perfil>& treino,
                              const std::vector<int>& ordem, Alvo alvo,
                              int min_perfis = 2);

Arvore construir_ordem_aleatoria(const std::vector<Pergunta>& perguntas,
                                 const std::vector<Perfil>& treino,
                                 unsigned semente, Alvo alvo,
                                 int min_perfis = 2);

Arvore construir_por_ganho(const std::vector<Pergunta>& perguntas,
                           const std::vector<Perfil>& treino, Alvo alvo,
                           int min_perfis = 2);

// Entropia (base 2) dos rótulos do alvo entre os perfis. 0 se vazio.
double entropia(const std::vector<Perfil>& perfis, Alvo alvo);

// Ganho de informação de dividir `perfis` pelas alternativas de `pergunta`.
double ganho_informacao(const std::vector<Perfil>& perfis,
                        const Pergunta& pergunta, Alvo alvo);
