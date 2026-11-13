#pragma once

#include <map>
#include <string>
#include <vector>

// Uma alternativa de uma pergunta do teste vocacional.
// pesos pode ser vazio (pergunta sem influência no score).
struct Alternativa {
    char letra;                        // 'a'..'d'
    std::string texto;
    std::map<std::string, int> pesos;  // ex.: {"PES_IOT",3},{"CICLO_ES",1}; pode ficar vazio
};

// Uma pergunta do banco de perguntas.
struct Pergunta {
    int id;
    std::string texto;
    std::vector<Alternativa> alternativas;  // 3 ou 4; índice 0 = 'a'
};

// Rótulos válidos de PES (8 no total).
const std::vector<std::string>& rotulos_pes();

// Rótulos válidos de ciclo (3 no total).
const std::vector<std::string>& rotulos_ciclo();

// Lê o arquivo `arquivo` no formato P;id;texto / A;letra;texto;ROTULO=peso;...
// Lança std::runtime_error com "arquivo:linha: motivo" em caso de erro de formato.
std::vector<Pergunta> carregar_perguntas(const std::string& arquivo);

// Devolve os ids das perguntas na ordem manual definida pelo grupo
// (do mais geral ao mais específico). O grupo pode editar a lista em
// perguntas.cpp, na definição desta função.
std::vector<int> ordem_manual();
