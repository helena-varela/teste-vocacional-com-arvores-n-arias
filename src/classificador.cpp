#include "classificador.hpp"

#include <stdexcept>

Classificacao classificar(const Arvore& arvore,
                          const std::map<int, char>& respostas) {
    if (arvore.vazia()) {
        throw std::invalid_argument("classificar: arvore vazia");
    }

    Classificacao c;
    const No* atual = arvore.raiz();

    while (!atual->eh_folha()) {
        const int id = atual->id_pergunta;
        c.caminho.push_back(id);

        auto it = respostas.find(id);
        if (it == respostas.end()) {
            throw std::invalid_argument("classificar: falta a resposta da pergunta " + std::to_string(id));
        }

        const int indice = it->second - 'a';
        if (indice < 0 || indice >= static_cast<int>(atual->filhos.size())) {
            throw std::invalid_argument("classificar: alternativa invalida '" + std::string(1, it->second) +
                                        "' na pergunta " + std::to_string(id));
        }

        atual = atual->filhos[indice].get();
    }

    c.resultado = atual->resultado;
    return c; 
}