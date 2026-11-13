// Testes do módulo "banco de perguntas".

#include <algorithm>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>
#include <unistd.h>

#include "perguntas.hpp"

namespace {

int falhas = 0;
int total  = 0;

void verificar(bool condicao, const std::string& descricao) {
    ++total;
    if (!condicao) {
        ++falhas;
        std::cout << "FALHOU: " << descricao << '\n';
    }
}

// Cria um arquivo temporário com `conteudo` e devolve seu caminho.
std::string arquivo_temp(const std::string& conteudo) {
    char tmpl[] = "/tmp/test_perguntas_XXXXXX";
    int fd = mkstemp(tmpl);
    if (fd == -1) throw std::runtime_error("nao foi possivel criar arquivo temporario");
    ::write(fd, conteudo.data(), conteudo.size());
    ::close(fd);
    return std::string(tmpl);
}

// RAII para apagar o arquivo temporário no final do escopo.
struct Temp {
    std::string caminho;
    explicit Temp(const std::string& conteudo) : caminho(arquivo_temp(conteudo)) {}
    ~Temp() { std::remove(caminho.c_str()); }
};

// Verifica se carregar_perguntas lança runtime_error com `substr` na mensagem.
void verificar_erro(const std::string& conteudo, const std::string& substr,
                    const std::string& descricao) {
    Temp t(conteudo);
    bool lancou = false;
    try {
        carregar_perguntas(t.caminho);
    } catch (const std::runtime_error& e) {
        lancou = true;
        const std::string msg = e.what();
        if (msg.find(substr) == std::string::npos) {
            ++falhas;
            ++total;
            std::cout << "FALHOU (mensagem errada): " << descricao
                      << "\n  esperado substring: \"" << substr
                      << "\"\n  mensagem recebida:  \"" << msg << "\"\n";
            return;
        }
    }
    ++total;
    if (!lancou) std::cout << "FALHOU (nao lancou): " << descricao << '\n';
    if (!lancou) ++falhas;
}

// ---------------------------------------------------------------------------
// Testes do arquivo real
// ---------------------------------------------------------------------------

void teste_arquivo_real() {
    const std::string caminho = "dados/perguntas.txt";
    std::vector<Pergunta> perguntas;
    bool carregou = true;
    try {
        perguntas = carregar_perguntas(caminho);
    } catch (const std::exception& e) {
        carregou = false;
        std::cout << "FALHOU (excecao inesperada no arquivo real): " << e.what() << '\n';
    }
    ++total;
    if (!carregou) { ++falhas; return; }

    verificar(perguntas.size() >= 12 && perguntas.size() <= 15,
              "arquivo real tem entre 12 e 15 perguntas");

    for (const auto& p : perguntas) {
        verificar(p.alternativas.size() >= 3 && p.alternativas.size() <= 4,
                  "pergunta " + std::to_string(p.id) + " tem 3 ou 4 alternativas");
    }

    // ordem_manual deve ter exatamente os mesmos ids, sem repetição.
    const std::vector<int> ordem = ordem_manual();
    std::set<int> ids_arquivo;
    for (const auto& p : perguntas) ids_arquivo.insert(p.id);

    std::set<int> ids_ordem(ordem.begin(), ordem.end());
    verificar(ids_ordem == ids_arquivo,
              "ordem_manual contem exatamente os ids do arquivo real");
    verificar(ordem.size() == ids_ordem.size(),
              "ordem_manual nao tem ids repetidos");
}

// ---------------------------------------------------------------------------
// Testes de CRLF e comentários
// ---------------------------------------------------------------------------

void teste_crlf() {
    // Arquivo com quebras de linha Windows (\r\n).
    const std::string conteudo =
        "P;1;Pergunta um\r\n"
        "A;a;Alt a;PES_IOT=3\r\n"
        "A;b;Alt b;PES_DADOS=3;CICLO_IA=1\r\n"
        "A;c;Alt c;PES_BIO=3;PES_EDU=2;PES_INOV=1;PES_IA=1;PES_JOGOS=1;PES_SIG=1;CICLO_CC=1;CICLO_ES=1\r\n";
    Temp t(conteudo);
    bool ok = true;
    try {
        carregar_perguntas(t.caminho);
    } catch (const std::exception& e) {
        ok = false;
        std::cout << "FALHOU (CRLF): " << e.what() << '\n';
    }
    verificar(ok, "arquivo com CRLF carrega normalmente");
}

void teste_comentarios_e_linhas_vazias() {
    const std::string conteudo =
        "# comentario inicial\n"
        "\n"
        "P;1;Pergunta com comentario\n"
        "# comentario entre alternativas\n"
        "\n"
        "A;a;Alt a;PES_IOT=3\n"
        "A;b;Alt b;PES_DADOS=3;CICLO_IA=1\n"
        "A;c;Alt c;PES_BIO=3;PES_EDU=2;PES_INOV=1;PES_IA=1;PES_JOGOS=1;PES_SIG=1;CICLO_CC=1;CICLO_ES=1\n"
        "\n"
        "# fim\n";
    Temp t(conteudo);
    bool ok = true;
    std::vector<Pergunta> ps;
    try {
        ps = carregar_perguntas(t.caminho);
    } catch (const std::exception& e) {
        ok = false;
        std::cout << "FALHOU (comentarios): " << e.what() << '\n';
    }
    verificar(ok, "arquivo com comentarios e linhas vazias carrega normalmente");
    if (ok) verificar(ps.size() == 1 && ps[0].alternativas.size() == 3,
                      "arquivo com comentarios carregou 1 pergunta com 3 alternativas");
}

// ---------------------------------------------------------------------------
// Testes de erro de validação
// ---------------------------------------------------------------------------

// Conteúdo mínimo válido que cobre todos os rótulos num único arquivo de teste.
static const std::string COBERTURA_COMPLETA =
    "P;1;Pergunta\n"
    "A;a;Alt a;PES_IOT=3;PES_JOGOS=1\n"
    "A;b;Alt b;PES_DADOS=3;PES_BIO=1;CICLO_IA=1\n"
    "A;c;Alt c;PES_EDU=3;PES_INOV=2;PES_IA=1;PES_SIG=1;CICLO_CC=1;CICLO_ES=1\n";

void teste_arquivo_inexistente() {
    bool lancou = false;
    try {
        carregar_perguntas("/caminho/que/nao/existe.txt");
    } catch (const std::runtime_error&) {
        lancou = true;
    }
    verificar(lancou, "arquivo inexistente lanca runtime_error");
}

void teste_sem_perguntas() {
    verificar_erro("# so comentarios\n\n", "nenhuma pergunta", "arquivo sem perguntas lanca erro");
}

void teste_tipo_desconhecido() {
    verificar_erro(
        "P;1;Pergunta\n"
        "A;a;Alt a;PES_IOT=3\n"
        "X;lixo\n",
        "tipo de linha desconhecido",
        "linha com tipo desconhecido lanca erro");
}

void teste_id_nao_numerico() {
    verificar_erro("P;abc;Pergunta\n", "nao numerico", "id nao numerico lanca erro");
}

void teste_id_zero() {
    verificar_erro("P;0;Pergunta\n", "maior que zero", "id zero lanca erro");
}

void teste_id_repetido() {
    verificar_erro(
        "P;1;Pergunta A\n"
        "A;a;Alt a;PES_IOT=3\n"
        "A;b;Alt b;PES_DADOS=1\n"
        "A;c;Alt c\n"
        "P;1;Pergunta B\n"
        "A;a;Alt a\n"
        "A;b;Alt b\n"
        "A;c;Alt c\n",
        "repetido",
        "id repetido lanca erro");
}

void teste_texto_pergunta_vazio() {
    // "P;1;" — o campo de texto não existe após o último ';', disparando "falta o texto".
    // "P;1; " — campo existe mas é só espaço, disparando "vazio".
    verificar_erro("P;1;\n", "texto da pergunta", "texto de pergunta vazio/ausente lanca erro");
}

void teste_texto_alternativa_vazio() {
    verificar_erro(
        "P;1;Pergunta\n"
        "A;a;\n",
        "vazio",
        "texto de alternativa vazio lanca erro");
}

void teste_alternativa_antes_de_pergunta() {
    verificar_erro("A;a;Alt a;PES_IOT=1\n", "antes de qualquer pergunta",
                   "alternativa antes de pergunta lanca erro");
}

void teste_letra_fora_de_sequencia() {
    verificar_erro(
        "P;1;Pergunta\n"
        "A;b;Alt b;PES_IOT=3\n",
        "fora de sequencia",
        "letra fora de sequencia lanca erro");
}

void teste_poucas_alternativas() {
    verificar_erro(
        "P;1;Pergunta com 2 alternativas\n"
        "A;a;Alt a;PES_IOT=3\n"
        "A;b;Alt b;PES_DADOS=1\n",
        "tem 2 alternativa",
        "pergunta com 2 alternativas lanca erro");
}

void teste_muitas_alternativas() {
    // 5 alternativas - inválido (máximo 4).
    verificar_erro(
        "P;1;Pergunta com 5 alternativas\n"
        "A;a;Alt a;PES_IOT=3\n"
        "A;b;Alt b;PES_DADOS=1\n"
        "A;c;Alt c\n"
        "A;d;Alt d\n"
        "A;e;Alt e\n",  // A letra 'e' é inválida, disparado ao verificar sequência.
        // Pode ser "fora do intervalo a-d" ou "fora de sequencia" dependendo da impl.
        "a-d",
        "mais de 4 alternativas lanca erro");
}

void teste_peso_sem_igual() {
    verificar_erro(
        "P;1;Pergunta\n"
        "A;a;Alt a;PES_IOT\n",
        "sem '='",
        "peso sem '=' lanca erro");
}

void teste_peso_nao_numerico() {
    verificar_erro(
        "P;1;Pergunta\n"
        "A;a;Alt a;PES_IOT=x\n",
        "nao numerico",
        "peso nao numerico lanca erro");
}

void teste_peso_fora_do_intervalo() {
    verificar_erro(
        "P;1;Pergunta\n"
        "A;a;Alt a;PES_IOT=6\n",
        "fora do intervalo 1-5",
        "peso fora do intervalo lanca erro");
}

void teste_rotulo_invalido() {
    verificar_erro(
        "P;1;Pergunta\n"
        "A;a;Alt a;PES_INVALIDO=3\n",
        "rotulo invalido",
        "rotulo invalido lanca erro");
}

void teste_rotulo_repetido() {
    verificar_erro(
        "P;1;Pergunta\n"
        "A;a;Alt a;PES_IOT=3;PES_IOT=1\n",
        "rotulo repetido",
        "rotulo repetido na mesma alternativa lanca erro");
}

void teste_rotulo_sem_cobertura() {
    // Arquivo válido mas que não cobre PES_EDU.
    const std::string sem_edu =
        "P;1;Pergunta\n"
        "A;a;Alt a;PES_IOT=3;PES_JOGOS=1\n"
        "A;b;Alt b;PES_DADOS=3;PES_BIO=1;CICLO_IA=1\n"
        "A;c;Alt c;PES_INOV=2;PES_IA=1;PES_SIG=1;CICLO_CC=1;CICLO_ES=1\n";
    // PES_EDU falta → deve lançar.
    verificar_erro(sem_edu, "PES_EDU", "rotulo sem cobertura lanca erro");
}

// ---------------------------------------------------------------------------
// Testes de rótulos
// ---------------------------------------------------------------------------

void teste_rotulos() {
    const auto& pes   = rotulos_pes();
    const auto& ciclo = rotulos_ciclo();
    verificar(pes.size() == 8,   "rotulos_pes tem 8 elementos");
    verificar(ciclo.size() == 3, "rotulos_ciclo tem 3 elementos");

    // Verifica alguns nomes esperados.
    auto tem = [](const std::vector<std::string>& v, const std::string& s) {
        return std::find(v.begin(), v.end(), s) != v.end();
    };
    verificar(tem(pes, "PES_BIO"),   "PES_BIO presente");
    verificar(tem(pes, "PES_IA"),    "PES_IA presente");
    verificar(!tem(pes, "CICLO_IA"), "CICLO_IA nao esta em rotulos_pes");
    verificar(tem(ciclo, "CICLO_IA"), "CICLO_IA presente em rotulos_ciclo");
    verificar(!tem(ciclo, "PES_IA"),  "PES_IA nao esta em rotulos_ciclo");
}

}  // namespace

int main() {
    // Arquivo real.
    teste_arquivo_real();

    // CRLF e comentários.
    teste_crlf();
    teste_comentarios_e_linhas_vazias();

    // Erros de validação.
    teste_arquivo_inexistente();
    teste_sem_perguntas();
    teste_tipo_desconhecido();
    teste_id_nao_numerico();
    teste_id_zero();
    teste_id_repetido();
    teste_texto_pergunta_vazio();
    teste_texto_alternativa_vazio();
    teste_alternativa_antes_de_pergunta();
    teste_letra_fora_de_sequencia();
    teste_poucas_alternativas();
    teste_muitas_alternativas();
    teste_peso_sem_igual();
    teste_peso_nao_numerico();
    teste_peso_fora_do_intervalo();
    teste_rotulo_invalido();
    teste_rotulo_repetido();
    teste_rotulo_sem_cobertura();

    // Rótulos.
    teste_rotulos();

    std::cout << (total - falhas) << "/" << total << " testes passaram\n";
    return falhas == 0 ? 0 : 1;
}
