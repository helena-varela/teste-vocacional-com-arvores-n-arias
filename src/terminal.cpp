#include "terminal.hpp"

#include <cctype>
#include <stdexcept>
#include <string>

namespace {

std::string aparar(const std::string& s) {
    size_t inicio = 0;
    size_t fim = s.size();
    while (inicio < fim && std::isspace(static_cast<unsigned char>(s[inicio]))) ++inicio;
    while (fim > inicio && std::isspace(static_cast<unsigned char>(s[fim - 1]))) --fim;
    return s.substr(inicio, fim - inicio);
}

}  // namespace

char ler_opcao(int num_alternativas, std::istream& in, std::ostream& out) {
    if (num_alternativas < 1 || num_alternativas > 26) {
        throw std::invalid_argument("num_alternativas deve estar entre 1 e 26");
    }
    const char ultima = static_cast<char>('a' + num_alternativas - 1);
    std::string linha;
    while (true) {
        out << "> " << std::flush;
        if (!std::getline(in, linha)) {
            out << '\n';
            return '\0';
        }
        const std::string resposta = aparar(linha);
        if (resposta.size() == 1) {
            const char c = static_cast<char>(std::tolower(static_cast<unsigned char>(resposta[0])));
            if (c >= 'a' && c <= ultima) {
                return c;
            }
        }
        out << "Opcao invalida. Digite uma letra de a ate " << ultima << ".\n";
    }
}

int ler_numero(int minimo, int maximo, std::istream& in, std::ostream& out) {
    std::string linha;
    while (true) {
        out << "> " << std::flush;
        if (!std::getline(in, linha)) {
            out << '\n';
            return minimo - 1;
        }
        const std::string resposta = aparar(linha);
        bool so_digitos = !resposta.empty() && resposta.size() <= 9;
        for (char c : resposta) {
            if (!std::isdigit(static_cast<unsigned char>(c))) so_digitos = false;
        }
        if (so_digitos) {
            const int valor = std::stoi(resposta);
            if (valor >= minimo && valor <= maximo) {
                return valor;
            }
        }
        out << "Opcao invalida. Digite um numero de " << minimo << " a " << maximo << ".\n";
    }
}
