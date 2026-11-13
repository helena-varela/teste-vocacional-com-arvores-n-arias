#include "perguntas.hpp"

#include <cctype>
#include <fstream>
#include <set>
#include <sstream>
#include <stdexcept>
#include <unordered_set>

// ---------------------------------------------------------------------------
// Rótulos válidos
// ---------------------------------------------------------------------------

const std::vector<std::string>& rotulos_pes() {
    static const std::vector<std::string> r = {
        "PES_BIO", "PES_DADOS", "PES_EDU", "PES_INOV",
        "PES_IA",  "PES_IOT",  "PES_JOGOS", "PES_SIG"
    };
    return r;
}

const std::vector<std::string>& rotulos_ciclo() {
    static const std::vector<std::string> r = {
        "CICLO_IA", "CICLO_CC", "CICLO_ES"
    };
    return r;
}

// ---------------------------------------------------------------------------
// Helpers internos
// ---------------------------------------------------------------------------

namespace {

// Remove espaços e '\r' das bordas.
std::string aparar(const std::string& s) {
    size_t inicio = 0;
    size_t fim = s.size();
    while (inicio < fim && (std::isspace(static_cast<unsigned char>(s[inicio])) || s[inicio] == '\r'))
        ++inicio;
    while (fim > inicio && (std::isspace(static_cast<unsigned char>(s[fim - 1])) || s[fim - 1] == '\r'))
        --fim;
    return s.substr(inicio, fim - inicio);
}

bool rotulo_valido(const std::string& r) {
    for (const auto& p : rotulos_pes())
        if (p == r) return true;
    for (const auto& c : rotulos_ciclo())
        if (c == r) return true;
    return false;
}

// Formata o prefixo de erro "arquivo:linha: ".
std::string prefixo(const std::string& arq, int linha) {
    return arq + ":" + std::to_string(linha) + ": ";
}

// Valida que a pergunta `p` tem entre 3 e 4 alternativas (chamada ao fechar).
void validar_alternativas(const Pergunta& p, const std::string& arq, int linha_p) {
    const size_t n = p.alternativas.size();
    if (n < 3 || n > 4) {
        throw std::runtime_error(
            prefixo(arq, linha_p) +
            "pergunta " + std::to_string(p.id) +
            " tem " + std::to_string(n) + " alternativa(s); esperado entre 3 e 4");
    }
}

}  // namespace

// ---------------------------------------------------------------------------
// carregar_perguntas
// ---------------------------------------------------------------------------

std::vector<Pergunta> carregar_perguntas(const std::string& arquivo) {
    std::ifstream f(arquivo);
    if (!f.is_open()) {
        throw std::runtime_error(arquivo + ": nao foi possivel abrir o arquivo");
    }

    std::vector<Pergunta> perguntas;
    std::set<int> ids_vistos;

    // Rastreamos para verificar cobertura no final.
    std::set<std::string> rotulos_vistos;

    std::string linha_bruta;
    int num_linha = 0;
    bool tem_pergunta_aberta = false;
    int linha_pergunta_aberta = 0;
    char proxima_letra = 'a';

    auto fechar_pergunta = [&]() {
        if (tem_pergunta_aberta) {
            validar_alternativas(perguntas.back(), arquivo, linha_pergunta_aberta);
        }
    };

    while (std::getline(f, linha_bruta)) {
        ++num_linha;
        const std::string linha = aparar(linha_bruta);

        // Ignora linhas vazias e comentários.
        if (linha.empty() || linha[0] == '#') continue;

        // Extrai o tipo (primeiro campo).
        std::istringstream ss(linha);
        std::string tipo;
        std::getline(ss, tipo, ';');
        tipo = aparar(tipo);

        if (tipo == "P") {
            // ----- Fecha a pergunta anterior (se houver) -----
            fechar_pergunta();

            // ----- Lê id -----
            std::string campo_id;
            if (!std::getline(ss, campo_id, ';')) {
                throw std::runtime_error(prefixo(arquivo, num_linha) + "linha P mal formada: falta o id");
            }
            campo_id = aparar(campo_id);

            bool id_valido = !campo_id.empty();
            for (char c : campo_id)
                if (!std::isdigit(static_cast<unsigned char>(c))) { id_valido = false; break; }
            if (!id_valido) {
                throw std::runtime_error(prefixo(arquivo, num_linha) + "id nao numerico: \"" + campo_id + "\"");
            }
            const int id = std::stoi(campo_id);
            if (id <= 0) {
                throw std::runtime_error(prefixo(arquivo, num_linha) + "id deve ser maior que zero");
            }
            if (ids_vistos.count(id)) {
                throw std::runtime_error(prefixo(arquivo, num_linha) + "id repetido: " + std::to_string(id));
            }
            ids_vistos.insert(id);

            // ----- Lê texto -----
            std::string texto;
            std::getline(ss, texto);  // pode ficar vazio; a linha P existia, logo o campo existe
            texto = aparar(texto);
            if (texto.empty()) {
                throw std::runtime_error(prefixo(arquivo, num_linha) + "texto da pergunta esta vazio");
            }

            Pergunta p;
            p.id = id;
            p.texto = std::move(texto);
            perguntas.push_back(std::move(p));

            tem_pergunta_aberta = true;
            linha_pergunta_aberta = num_linha;
            proxima_letra = 'a';

        } else if (tipo == "A") {
            if (!tem_pergunta_aberta) {
                throw std::runtime_error(prefixo(arquivo, num_linha) + "alternativa antes de qualquer pergunta");
            }

            // ----- Lê letra -----
            std::string campo_letra;
            if (!std::getline(ss, campo_letra, ';')) {
                throw std::runtime_error(prefixo(arquivo, num_linha) + "linha A mal formada: falta a letra");
            }
            campo_letra = aparar(campo_letra);
            if (campo_letra.size() != 1) {
                throw std::runtime_error(prefixo(arquivo, num_linha) + "letra invalida: \"" + campo_letra + "\"");
            }
            const char letra = static_cast<char>(std::tolower(static_cast<unsigned char>(campo_letra[0])));
            if (letra != proxima_letra) {
                throw std::runtime_error(
                    prefixo(arquivo, num_linha) +
                    "letra fora de sequencia: esperado '" + proxima_letra + "', encontrado '" + letra + "'");
            }
            if (letra > 'd') {
                throw std::runtime_error(prefixo(arquivo, num_linha) + "letra fora do intervalo a-d");
            }

            // ----- Lê texto -----
            std::string texto_alt;
            if (!std::getline(ss, texto_alt, ';')) {
                // Tenta sem ';' no final (última alternativa sem pesos).
                std::getline(ss, texto_alt);
            }
            texto_alt = aparar(texto_alt);
            // Reabsorve o restante da linha se havia ';' como separador.
            // Na prática, getline(ss, texto_alt, ';') já faz isso.
            if (texto_alt.empty()) {
                throw std::runtime_error(prefixo(arquivo, num_linha) + "texto da alternativa esta vazio");
            }

            Alternativa alt;
            alt.letra = letra;
            alt.texto = std::move(texto_alt);

            // ----- Lê pesos (zero ou mais) -----
            std::string campo_peso;
            std::set<std::string> rotulos_na_alt;
            while (std::getline(ss, campo_peso, ';')) {
                campo_peso = aparar(campo_peso);
                if (campo_peso.empty()) continue;

                const size_t pos = campo_peso.find('=');
                if (pos == std::string::npos) {
                    throw std::runtime_error(prefixo(arquivo, num_linha) + "peso sem '=': \"" + campo_peso + "\"");
                }
                const std::string rotulo = aparar(campo_peso.substr(0, pos));
                const std::string valor_str = aparar(campo_peso.substr(pos + 1));

                if (!rotulo_valido(rotulo)) {
                    throw std::runtime_error(prefixo(arquivo, num_linha) + "rotulo invalido: \"" + rotulo + "\"");
                }
                if (rotulos_na_alt.count(rotulo)) {
                    throw std::runtime_error(prefixo(arquivo, num_linha) + "rotulo repetido na mesma alternativa: \"" + rotulo + "\"");
                }

                bool valor_ok = !valor_str.empty();
                for (char c : valor_str)
                    if (!std::isdigit(static_cast<unsigned char>(c))) { valor_ok = false; break; }
                if (!valor_ok) {
                    throw std::runtime_error(prefixo(arquivo, num_linha) + "peso nao numerico: \"" + valor_str + "\"");
                }
                const int peso = std::stoi(valor_str);
                if (peso < 1 || peso > 5) {
                    throw std::runtime_error(
                        prefixo(arquivo, num_linha) +
                        "peso fora do intervalo 1-5: " + std::to_string(peso));
                }

                rotulos_na_alt.insert(rotulo);
                rotulos_vistos.insert(rotulo);
                alt.pesos[rotulo] = peso;
            }

            perguntas.back().alternativas.push_back(std::move(alt));
            proxima_letra = static_cast<char>(proxima_letra + 1);

        } else {
            throw std::runtime_error(prefixo(arquivo, num_linha) + "tipo de linha desconhecido: \"" + tipo + "\"");
        }
    }

    // Fecha a última pergunta.
    fechar_pergunta();

    if (perguntas.empty()) {
        throw std::runtime_error(arquivo + ": nenhuma pergunta encontrada no arquivo");
    }

    // Verifica cobertura: cada PES e cada ciclo deve aparecer em pelo menos uma alternativa.
    for (const auto& r : rotulos_pes()) {
        if (!rotulos_vistos.count(r)) {
            throw std::runtime_error(arquivo + ": rotulo \"" + r + "\" nunca recebe peso em nenhuma alternativa");
        }
    }
    for (const auto& r : rotulos_ciclo()) {
        if (!rotulos_vistos.count(r)) {
            throw std::runtime_error(arquivo + ": rotulo \"" + r + "\" nunca recebe peso em nenhuma alternativa");
        }
    }

    return perguntas;
}

// ---------------------------------------------------------------------------
// ordem_manual
// ---------------------------------------------------------------------------

std::vector<int> ordem_manual() {
    // Ordem definida pelo grupo: do mais geral (interesses amplos) ao mais
    // específico (ambiente de trabalho, perfil acadêmico).
    // Edite aqui para ajustar a ordem sem mexer em mais nada.
    return {
        1,   // O que mais te atrai em TI?
        2,   // Em qual area voce gostaria de aplicar seus conhecimentos?
        3,   // Qual projeto voce escolheria?
        6,   // O que mais te motiva em um projeto?
        13,  // Qual noticia voce leria primeiro?
        7,   // Qual tarefa voce faria com mais prazer?
        4,   // Como e a sua relacao com a matematica?
        8,   // Qual disciplina do BTI voce mais gostou ou espera gostar?
        12,  // Como voce costuma resolver problemas?
        10,  // Diante de uma ideia nova, o que voce faz primeiro?
        9,   // Quanto te interessa biologia e saude?
        11,  // Como voce se imagina profissionalmente daqui a 10 anos?
        14,  // Em que ambiente voce gostaria de trabalhar?
        5,   // Voce prefere trabalhar sozinho ou em grupo? (sem pesos - contraste)
    };
}
