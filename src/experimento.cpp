#include "experimento.hpp"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <map>
#include <set>
#include <stdexcept>

#include "classificador.hpp"
#include "construtores.hpp"
#include "perfis.hpp"
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


// ---------------------------------------------------------------------------
// Experimentos (opção 4)
// ---------------------------------------------------------------------------

namespace {

// Métricas de uma árvore avaliada no conjunto de teste.
struct Metricas {
    double acuracia = 0.0;         // acertos / total
    double media_perguntas = 0.0;  // média de perguntas até a folha
    int max_perguntas = 0;         // maior número de perguntas feitas
};

Metricas avaliar(const Arvore& arvore, const std::vector<Perfil>& teste, Alvo alvo) {
    Metricas m;
    if (teste.empty()) return m;

    int acertos = 0;
    long soma_perguntas = 0;
    for (const Perfil& p : teste) {
        const Classificacao c = classificar(arvore, p.respostas);
        if (c.resultado == rotulo(p, alvo)) ++acertos;
        soma_perguntas += c.perguntas_feitas();
        m.max_perguntas = std::max(m.max_perguntas, c.perguntas_feitas());
    }
    m.acuracia = static_cast<double>(acertos) / teste.size();
    m.media_perguntas = static_cast<double>(soma_perguntas) / teste.size();
    return m;
}

// Avalia o par (T_PES, T_CICLO) como o aluno usaria: o par só acerta se os
// dois rótulos estiverem certos, e as perguntas contadas são as distintas dos
// dois caminhos (a árvore de ciclo reaproveita as respostas via cache).
Metricas avaliar_conjunto(const ParDeArvores& par, const std::vector<Perfil>& teste) {
    Metricas m;
    if (teste.empty()) return m;

    int acertos = 0;
    long soma_perguntas = 0;
    for (const Perfil& p : teste) {
        const Classificacao cp = classificar(par.pes, p.respostas);
        const Classificacao cc = classificar(par.ciclo, p.respostas);
        if (cp.resultado == p.pes && cc.resultado == p.ciclo) ++acertos;

        std::set<int> distintas(cp.caminho.begin(), cp.caminho.end());
        distintas.insert(cc.caminho.begin(), cc.caminho.end());
        const int n = static_cast<int>(distintas.size());
        soma_perguntas += n;
        m.max_perguntas = std::max(m.max_perguntas, n);
    }
    m.acuracia = static_cast<double>(acertos) / teste.size();
    m.media_perguntas = static_cast<double>(soma_perguntas) / teste.size();
    return m;
}

double media(const std::vector<double>& v) {
    double s = 0.0;
    for (double x : v) s += x;
    return v.empty() ? 0.0 : s / v.size();
}

// Desvio padrão amostral (0 quando há só um valor).
double desvio_padrao(const std::vector<double>& v) {
    if (v.size() < 2) return 0.0;
    const double m = media(v);
    double s = 0.0;
    for (double x : v) s += (x - m) * (x - m);
    return std::sqrt(s / (v.size() - 1));
}

}  // namespace

void executar_experimentos(const std::string& diretorio_perfis,
                           const std::string& diretorio_saida) {
    const std::vector<std::string> ruidos = {"10", "25", "40"};
    const std::vector<Estrategia> estrategias = {Estrategia::MANUAL, Estrategia::ALEATORIA,
                                                 Estrategia::GANHO};
    const std::vector<Alvo> alvos = {Alvo::PES, Alvo::CICLO};
    const int repeticoes_aleatoria = 30;

    const std::vector<Pergunta> perguntas = carregar_perguntas("dados/perguntas.txt");

    std::filesystem::create_directories(diretorio_saida);
    std::ofstream csv1(diretorio_saida + "/resultados_execucoes.csv");
    std::ofstream csv2(diretorio_saida + "/resultados_resumo.csv");
    if (!csv1 || !csv2) {
        throw std::runtime_error(diretorio_saida + ": nao foi possivel criar os CSVs de resultado");
    }

    csv1 << "ruido,alvo,arvore,semente,acuracia,media_perguntas,max_perguntas,altura,nos,folhas\n";
    csv2 << "ruido,alvo,arvore,execucoes,acuracia_media,acuracia_dp,"
            "media_perguntas_media,media_perguntas_dp\n";
    csv1 << std::fixed << std::setprecision(4);
    csv2 << std::fixed << std::setprecision(4);

    for (const std::string& r : ruidos) {
        const std::string base = diretorio_perfis + "/ruido_" + r + "pct_";
        const std::vector<Perfil> treino = carregar_perfis(base + "treino.csv");
        const std::vector<Perfil> teste = carregar_perfis(base + "teste.csv");
        const std::string ruido = "0." + r;

        // Avaliação por árvore: cada alvo separadamente.
        for (Alvo alvo : alvos) {
            const char* nome_alvo = (alvo == Alvo::PES) ? "PES" : "CICLO";

            for (Estrategia e : estrategias) {
                const int execucoes = (e == Estrategia::ALEATORIA) ? repeticoes_aleatoria : 1;
                std::vector<double> acuracias, perguntas_medias;

                for (int i = 0; i < execucoes; ++i) {
                    const unsigned semente = 42 + i;
                    const Arvore arvore = construir(e, perguntas, treino, alvo, semente);
                    const Metricas m = avaliar(arvore, teste, alvo);

                    acuracias.push_back(m.acuracia);
                    perguntas_medias.push_back(m.media_perguntas);

                    csv1 << ruido << ',' << nome_alvo << ',' << nome_estrategia(e) << ','
                         << semente << ',' << m.acuracia << ',' << m.media_perguntas << ','
                         << m.max_perguntas << ',' << arvore.altura() << ','
                         << arvore.contar_nos() << ',' << arvore.contar_folhas() << '\n';
                }

                csv2 << ruido << ',' << nome_alvo << ',' << nome_estrategia(e) << ','
                     << execucoes << ',' << media(acuracias) << ',' << desvio_padrao(acuracias)
                     << ',' << media(perguntas_medias) << ','
                     << desvio_padrao(perguntas_medias) << '\n';

                std::cout << "ruido " << ruido << " | " << nome_alvo << " | "
                          << nome_estrategia(e) << ": acuracia " << media(acuracias)
                          << ", perguntas " << media(perguntas_medias) << '\n';
            }
        }

        // Avaliação conjunta: PES + ciclo com o mesmo par de árvores e cache.
        for (Estrategia e : estrategias) {
            const int execucoes = (e == Estrategia::ALEATORIA) ? repeticoes_aleatoria : 1;
            std::vector<double> acuracias, perguntas_medias;

            for (int i = 0; i < execucoes; ++i) {
                const unsigned semente = 42 + i;
                const ParDeArvores par = construir_par(e, perguntas, treino, semente);
                const Metricas m = avaliar_conjunto(par, teste);

                acuracias.push_back(m.acuracia);
                perguntas_medias.push_back(m.media_perguntas);

                csv1 << ruido << ",CONJUNTO," << nome_estrategia(e) << ',' << semente << ','
                     << m.acuracia << ',' << m.media_perguntas << ',' << m.max_perguntas << ','
                     << std::max(par.pes.altura(), par.ciclo.altura()) << ','
                     << par.pes.contar_nos() + par.ciclo.contar_nos() << ','
                     << par.pes.contar_folhas() + par.ciclo.contar_folhas() << '\n';
            }

            csv2 << ruido << ",CONJUNTO," << nome_estrategia(e) << ',' << execucoes << ','
                 << media(acuracias) << ',' << desvio_padrao(acuracias) << ','
                 << media(perguntas_medias) << ',' << desvio_padrao(perguntas_medias) << '\n';

            std::cout << "ruido " << ruido << " | CONJUNTO | " << nome_estrategia(e)
                      << ": acuracia " << media(acuracias) << ", perguntas "
                      << media(perguntas_medias) << '\n';
        }
    }

    std::cout << "\nCSVs gravados em " << diretorio_saida << "/\n";
}