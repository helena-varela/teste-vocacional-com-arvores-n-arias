#include "perfis.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>

namespace {

// Separa `linha` em campos usando `sep`.
std::vector<std::string> separar(const std::string& linha, char sep) {
    std::vector<std::string> campos;
    std::istringstream ss(linha);
    std::string campo;
    while (std::getline(ss, campo, sep)) {
        campos.push_back(campo);
    }
    return campos;
}

// Remove o '\r' do fim da linha (arquivos com quebra de linha do Windows).
void tirar_cr(std::string& linha) {
    if (!linha.empty() && linha.back() == '\r') {
        linha.pop_back();
    }
}

std::string prefixo(const std::string& arquivo, int linha) {
    return arquivo + ":" + std::to_string(linha) + ": ";
}

}  // namespace

const std::string& rotulo(const Perfil& perfil, Alvo alvo) {
    if (alvo == Alvo::PES) {
        return perfil.pes;
    }
    return perfil.ciclo;
}

std::vector<Perfil> carregar_perfis(const std::string& arquivo_csv) {
    std::ifstream f(arquivo_csv);
    if (!f.is_open()) {
        throw std::runtime_error(arquivo_csv + ": nao foi possivel abrir o arquivo");
    }

    // ----- Cabeçalho -----
    std::string linha;
    if (!std::getline(f, linha)) {
        throw std::runtime_error(arquivo_csv + ": arquivo vazio");
    }
    tirar_cr(linha);
    const std::vector<std::string> cabecalho = separar(linha, ',');

    int col_id = -1, col_divisao = -1, col_pes = -1, col_ciclo = -1;
    std::vector<std::pair<int, int>> colunas_q;  // (posição da coluna, id da pergunta)

    for (int i = 0; i < static_cast<int>(cabecalho.size()); ++i) {
        const std::string& nome = cabecalho[i];
        if (nome == "id") col_id = i;
        else if (nome == "divisao") col_divisao = i;
        else if (nome == "pes") col_pes = i;
        else if (nome == "ciclo") col_ciclo = i;
        else if (nome.size() > 1 && nome[0] == 'Q') {
            colunas_q.push_back({i, std::stoi(nome.substr(1))});
        }
    }
    if (col_id < 0 || col_divisao < 0 || col_pes < 0 || col_ciclo < 0 || colunas_q.empty()) {
        throw std::runtime_error(prefixo(arquivo_csv, 1) + "cabecalho sem colunas obrigatorias");
    }

    // ----- Linhas de dados -----
    std::vector<Perfil> perfis;
    int num_linha = 1;
    while (std::getline(f, linha)) {
        ++num_linha;
        tirar_cr(linha);
        if (linha.empty()) continue;

        const std::vector<std::string> campos = separar(linha, ',');
        if (campos.size() != cabecalho.size()) {
            throw std::runtime_error(prefixo(arquivo_csv, num_linha) + "numero de colunas diferente do cabecalho");
        }

        Perfil p;
        try {
            p.id = std::stoi(campos[col_id]);
        } catch (const std::exception&) {
            throw std::runtime_error(prefixo(arquivo_csv, num_linha) + "id invalido: \"" + campos[col_id] + "\"");
        }
        p.treino = (campos[col_divisao] == "treino");
        p.pes = campos[col_pes];
        p.ciclo = campos[col_ciclo];

        for (const auto& [coluna, id_pergunta] : colunas_q) {
            const std::string& valor = campos[coluna];
            if (valor.size() != 1 || valor[0] < 'a' || valor[0] > 'd') {
                throw std::runtime_error(prefixo(arquivo_csv, num_linha) + "resposta invalida na Q" +
                                         std::to_string(id_pergunta) + ": \"" + valor + "\"");
            }
            p.respostas[id_pergunta] = valor[0];
        }

        perfis.push_back(std::move(p));
    }

    return perfis;
}