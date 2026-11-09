#pragma once

#include <iosfwd>
#include <memory>
#include <string>
#include <vector>

// Nó de uma árvore N-ária do teste vocacional.
//
// - Nó de pergunta (nó interno): guarda uma pergunta. Cada filho corresponde a
//   uma alternativa, na ordem: filhos[0] = 'a', filhos[1] = 'b', ...
// - Folha: guarda o resultado (um PES na árvore 1, um 2º ciclo na árvore 2).
//
// O papel do nó é fixo: quem é criado como folha nunca recebe filhos.
struct No {
    int id_pergunta = -1;          // id da pergunta no perguntas.txt (-1 nas folhas)
    std::string texto;             // texto da pergunta (vazio nas folhas)
    std::string rotulo_resposta;   // alternativa que leva do pai até este nó, ex.: "a) Hardware" (vazio na raiz)
    std::string resultado;         // PES ou 2º ciclo (só nas folhas)
    std::vector<std::unique_ptr<No>> filhos;

    bool eh_folha() const { return id_pergunta < 0; }
};

// Funções para montar a árvore (usadas pela Pessoa 4 nos construtores).
std::unique_ptr<No> criar_no_pergunta(int id_pergunta, std::string texto);
std::unique_ptr<No> criar_folha(std::string resultado);

// Liga `filho` como o próximo filho de `pai` e devolve um ponteiro para ele,
// para que a construção possa continuar a partir do filho.
// Lança std::invalid_argument se `pai` for uma folha ou se `filho` for nulo.
No* adicionar_filho(No& pai, std::unique_ptr<No> filho, std::string rotulo_resposta);

// Árvore N-ária. A memória dos nós é liberada automaticamente pelos
// std::unique_ptr quando a árvore é destruída.
class Arvore {
public:
    Arvore() = default;
    explicit Arvore(std::unique_ptr<No> raiz);

    void definir_raiz(std::unique_ptr<No> raiz);
    No* raiz() { return raiz_.get(); }
    const No* raiz() const { return raiz_.get(); }
    bool vazia() const { return raiz_ == nullptr; }

    // Pré-ordem: imprime a árvore indentada, um nó por linha.
    void imprimir_preordem(std::ostream& out) const;

    // Busca em largura (BFS): imprime os nós agrupados por nível.
    void imprimir_por_nivel(std::ostream& out) const;

    // Altura = número de arestas da raiz até a folha mais funda.
    // Árvore só com a raiz: 0. Árvore vazia: -1.
    int altura() const;
    int contar_nos() const;
    int contar_folhas() const;

    // Média simples da profundidade das folhas (cada folha conta uma vez).
    // Árvore vazia: 0.
    double profundidade_media_folhas() const;

private:
    std::unique_ptr<No> raiz_;
};
