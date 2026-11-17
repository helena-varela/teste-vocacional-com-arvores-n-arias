#include "experimento.hpp"

#include <map>
#include <stdexcept>

#include "classificador.hpp"
#include "terminal.hpp"

namespace {

// Procura a pergunta pelo id no banco. Devolve nullptr se não achar.
const Pergunta* buscar_pergunta(const std::vector<Pergunta>& perguntas, int id) {
    for (const Pergunta& p : perguntas) {
        if (p.id == id) return &p;
    }
    return nullptr;
}

// Percorre uma árvore perguntando no terminal. Respostas já presentes em
// `cache` não são perguntadas de novo. Devolve false se a entrada acabar.
bool percorrer(const Arvore& arvore, const std::vector<Pergunta>& perguntas,
               std::map<int, char>& cache, std::istream& in, std::ostream& out) {
    const No* atual = arvore.raiz();
    while (!atual->eh_folha()) {
        const int id = atual->id_pergunta;

        if (cache.count(id) == 0) {
            const Pergunta* p = buscar_pergunta(perguntas, id);
            const std::string texto = p ? p->texto : atual->texto;
            const int n = static_cast<int>(atual->filhos.size());

            out << "\n" << texto << "\n";
            for (int i = 0; i < n; ++i) {
                if (p && i < static_cast<int>(p->alternativas.size())) {
                    out << "  " << static_cast<char>('a' + i) << ") " << p->alternativas[i].texto << "\n";
                } else {
                    out << "  " << atual->filhos[i]->rotulo_resposta << "\n";
                }
            }

            const char letra = ler_opcao(n, in, out);
            if (letra == '\0') return false;
            cache[id] = letra;
        }

        atual = atual->filhos[cache[id] - 'a'].get();
    }
    return true;
}

}  // namespace

void executar_teste_interativo(const Arvore& arvore_pes,
                               const Arvore& arvore_ciclo,
                               const std::vector<Pergunta>& perguntas,
                               std::istream& in, std::ostream& out) {
    if (arvore_pes.vazia() || arvore_ciclo.vazia()) {
        out << "As arvores ainda nao foram construidas.\n";
        return;
    }

    std::map<int, char> cache;

    out << "\n=== Parte 1: PES ===\n";
    if (!percorrer(arvore_pes, perguntas, cache, in, out)) return;
    out << "\n=== Parte 2: 2o ciclo ===\n";
    if (!percorrer(arvore_ciclo, perguntas, cache, in, out)) return;

    const Classificacao pes = classificar(arvore_pes, cache);
    const Classificacao ciclo = classificar(arvore_ciclo, cache);

    out << "\n========== RESULTADO ==========\n";
    out << "PES recomendado: " << nome_rotulo(pes.resultado) << "\n";
    out << "2o ciclo recomendado: " << nome_rotulo(ciclo.resultado) << "\n";
    out << "Perguntas respondidas: " << cache.size() << "\n";
    out << "Caminho no PES: ";
    for (int id : pes.caminho) out << "P" << id << " ";
    out << "\nCaminho no ciclo: ";
    for (int id : ciclo.caminho) out << "P" << id << " ";
    out << "\n";
}