#include "exemplo.hpp"

#include <utility>

namespace exemplo {

// [P1] O que mais te atrai em TI?
//     (a) Hardware          => Internet das Coisas
//     (b) Dados             [P2] Em qual area...?
//         (a) Biologia      => Bioinformatica
//         (b) Negocios      => Sistemas de Informacoes Gerenciais
//         (c) Pesquisa      => Ciencia de Dados
//     (c) Jogos             => Jogos Digitais
//     (d) Ensinar           => Informatica Educacional
Arvore arvore_pes() {
    auto raiz = criar_no_pergunta(1, "O que mais te atrai em TI?");
    adicionar_filho(*raiz, criar_folha("Internet das Coisas"), "a) Mexer com hardware e sensores");
    No* p2 = adicionar_filho(*raiz,
                             criar_no_pergunta(2, "Em qual area voce gostaria de aplicar seus conhecimentos?"),
                             "b) Analisar dados e encontrar padroes");
    adicionar_filho(*p2, criar_folha("Bioinformatica"), "a) Biologia e saude");
    adicionar_filho(*p2, criar_folha("Sistemas de Informacoes Gerenciais"), "b) Negocios e empresas");
    adicionar_filho(*p2, criar_folha("Ciencia de Dados"), "c) Ciencia e pesquisa");
    adicionar_filho(*raiz, criar_folha("Jogos Digitais"), "c) Criar jogos ou experiencias interativas");
    adicionar_filho(*raiz, criar_folha("Informatica Educacional"), "d) Ajudar pessoas a aprender com tecnologia");
    return Arvore(std::move(raiz));
}

// [P3] Voce prefere estudar teoria e matematica ou construir produtos?
//     (a) Teoria            [P4] Voce gosta de modelos que aprendem com dados?
//         (a) Sim           => Inteligencia Artificial
//         (b) Nao           => Ciencia da Computacao
//     (b) Produtos          => Engenharia de Software
Arvore arvore_ciclo() {
    auto raiz = criar_no_pergunta(3, "Voce prefere estudar teoria e matematica ou construir produtos?");
    No* p4 = adicionar_filho(*raiz,
                             criar_no_pergunta(4, "Voce gosta da ideia de criar modelos que aprendem com dados?"),
                             "a) Teoria e matematica");
    adicionar_filho(*p4, criar_folha("Inteligencia Artificial"), "a) Sim");
    adicionar_filho(*p4, criar_folha("Ciencia da Computacao"), "b) Nao");
    adicionar_filho(*raiz, criar_folha("Engenharia de Software"), "b) Construir produtos");
    return Arvore(std::move(raiz));
}

}  // namespace exemplo
