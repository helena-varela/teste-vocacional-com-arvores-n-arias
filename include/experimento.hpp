#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "arvore.hpp"
#include "perguntas.hpp"

// Teste interativo e experimentos (Pessoa 5). São as opções 1 e 4 do menu.

// Opção 1: percorre a árvore de PES e depois a de ciclo, perguntando no
// terminal (com ler_opcao). Usa um cache de respostas: uma pergunta que já
// apareceu na primeira árvore não é feita de novo na segunda. No fim mostra
// o PES, o ciclo e o caminho que levou até eles.
void executar_teste_interativo(const Arvore& arvore_pes,
                               const Arvore& arvore_ciclo,
                               const std::vector<Pergunta>& perguntas,
                               std::istream& in = std::cin,
                               std::ostream& out = std::cout);

// Opção 4: para cada estratégia (manual, aleatória x30, ganho), alvo e
// nível de ruído, constrói as árvores com o treino, avalia no teste e grava
// os dois CSVs de resultado em `diretorio_saida`.
void executar_experimentos(const std::string& diretorio_perfis,
                           const std::string& diretorio_saida);
