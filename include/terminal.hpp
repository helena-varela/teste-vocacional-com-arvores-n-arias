#pragma once

#include <iostream>

// Lê a resposta de uma pergunta com `num_alternativas` alternativas (a, b, c, ...).
// Aceita maiúscula ou minúscula e espaços em volta. Se a entrada for inválida,
// avisa e pergunta de novo. Devolve a letra minúscula ('a', 'b', ...).
// Se a entrada acabar, devolve '\0'.
// Usada pelo teste interativo (Pessoa 5) e pelo menu
char ler_opcao(int num_alternativas, std::istream& in = std::cin, std::ostream& out = std::cout);

// Lê um número inteiro entre `minimo` e `maximo` (inclusive), perguntando de
// novo enquanto for inválido. Se a entrada acabar, devolve `minimo - 1`.
int ler_numero(int minimo, int maximo, std::istream& in = std::cin, std::ostream& out = std::cout);
