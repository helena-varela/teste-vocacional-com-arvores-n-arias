#pragma once

#include <map>
#include <string>
#include <vector>

// Um respondente simulado, lido dos CSVs gerados por scripts/gerar_perfis.py
// (dados/sinteticos/ruido_XXpct_treino.csv e ruido_XXpct_teste.csv).
//
// Colunas do CSV: id,divisao,ruido,pes,ciclo,Q1,Q2,...,Q14
struct Perfil {
  int id = 0;
  bool treino = false;            // coluna "divisao": true = "treino", false = "teste"
  std::string pes;                // rótulo verdadeiro, ex.: "PES_IOT"
  std::string ciclo;              // rótulo verdadeiro, ex.: "CICLO_ES"
  std::map<int, char> respostas;  // id da pergunta -> letra 'a'..'d' (coluna "Q<id>")
};

// Qual das duas árvores está sendo construída/avaliada.
enum class Alvo { PES, CICLO };

// Devolve o rótulo verdadeiro do perfil para o alvo (perfil.pes ou perfil.ciclo).
const std::string& rotulo(const Perfil& perfil, Alvo alvo);

// Lê um CSV de perfis. Lança std::runtime_error com "arquivo:linha: motivo"
// se faltar coluna, se uma resposta não for uma letra etc.
// Responsável: Pessoa 5.
std::vector<Perfil> carregar_perfis(const std::string& arquivo_csv);
