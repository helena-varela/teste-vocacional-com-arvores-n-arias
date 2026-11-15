# Teste vocacional com árvores N-árias

Projeto final de Estrutura de Dados Básicas II (IMD0039 – UFRN).

Um teste vocacional para alunos de TI, feito em C++ com árvores N-árias, que recomenda **um PES** e **um segundo ciclo** (Inteligência Artificial, Ciência da Computação ou Engenharia de Software).

**Pergunta de pesquisa:** ordenar as perguntas por ganho de informação reduz o número de perguntas necessárias para recomendar um PES e um segundo ciclo, sem perder acurácia, em comparação com uma ordem manual e uma ordem aleatória?

## Como começar

Repositório: https://github.com/helena-varela/teste-vocacional-com-arvores-n-arias

```bash
git clone git@github.com:helena-varela/teste-vocacional-com-arvores-n-arias.git
cd teste-vocacional-com-arvores-n-arias
git checkout -b pessoaN-nome-da-parte   # ex.: pessoa2-perguntas
```

Trabalhe no seu branch e, quando terminar uma parte, abra um pull request para o `main`.

## Como compilar e rodar

É preciso ter `g++` (C++17) e `make`.

```bash
make            # compila o programa em bin/teste_vocacional
make run        # compila e roda o menu
make testes     # compila e roda os testes unitários
make clean      # apaga bin/ e build/
```

## Estrutura do repositório

```
include/            cabeçalhos (.hpp)
src/                implementação (.cpp)
  arvore.cpp        estrutura N-ária: nós, pré-ordem, BFS, altura, contagens
  terminal.cpp      leitura e validação das respostas no terminal
  exemplo.cpp       árvores pequenas montadas à mão (provisórias, só para teste)
  perguntas.cpp     banco de perguntas: carregar_perguntas(), rotulos_*, ordem_manual()
  main.cpp          menu inicial
dados/              arquivos de dados
  perguntas.txt     banco de perguntas no formato P;id;texto / A;letra;texto;ROTULO=peso
tests/              testes unitários
```

## Divisão dos módulos

| Pessoa | Módulo |
| --- | --- |
| Pessoa 1 | Estrutura da árvore N-ária, menu e leitura de respostas (`arvore`, `terminal`, `main`) |
| Pessoa 2 | Banco de perguntas (`perguntas.txt`) e `carregar_perguntas()` |
| Pessoa 3 | Gerador de perfis simulados e script Python dos gráficos |
| Pessoa 4 | Construtores das árvores (ordem manual, aleatória e por ganho de informação) |
| Pessoa 5 | Teste interativo e experimentos (gera os CSVs) |

## Como usar a estrutura (para quem vai construir as árvores)

```cpp
#include "arvore.hpp"

auto raiz = criar_no_pergunta(1, "O que mais te atrai em TI?");
adicionar_filho(*raiz, criar_folha("Internet das Coisas"), "a) Hardware");
No* p2 = adicionar_filho(*raiz, criar_no_pergunta(2, "Em qual area...?"), "b) Dados");
adicionar_filho(*p2, criar_folha("Bioinformatica"), "a) Biologia");

Arvore arvore(std::move(raiz));
arvore.imprimir_preordem(std::cout);
```

- `filhos[0]` é a alternativa `a`, `filhos[1]` é a `b`, e assim por diante.
- Uma folha não pode receber filhos (`adicionar_filho` lança `std::invalid_argument`).
- A memória é liberada sozinha quando a `Arvore` é destruída (`std::unique_ptr`).

## Perfis sintéticos e gráficos

O script Python usa os pesos de `dados/perguntas.txt` para definir, para cada
combinação PES+ciclo, a alternativa típica de cada pergunta (maior soma dos
dois pesos; em empate, vence a primeira alternativa no arquivo). A pergunta 5
não tem pesos e, portanto, fica empatada; a primeira alternativa é escolhida
como desempate determinístico.

Instale as dependências e gere 1.200 respondentes por cenário:

```bash
python3 -m pip install -r requirements.txt
python3 scripts/gerar_perfis.py
```

O script cria três cenários (10%, 25% e 40%), cada qual com CSVs separados de
treino e teste (70/30, semente padrão 42), além de `perfis_tipicos.csv` e do
PDF `aderencia_perfil.pdf`, em `dados/sinteticos/`. Cada registro tem exatamente
um rótulo PES e um rótulo de ciclo. Em cada pergunta, segue a alternativa
típica com probabilidade `1 - ruído`; no restante, sorteia uma alternativa
uniformemente entre todas as opções da pergunta.

Argumentos opcionais: `--n`, `--ruidos` (frações entre 0 e 1),
`--semente` e `--saida`. Exemplo:

```bash
python3 scripts/gerar_perfis.py --n 600 --ruidos 0.1,0.25,0.4 --semente 42
```

Os dados são sintéticos e reproduzem os pesos atuais, não constituindo
validação empírica dos perfis ou das recomendações.

## Combinados do grupo

- Cada pessoa trabalha no seu próprio branch e abre um pull request. Outra pessoa revisa antes do merge.
- Rode `make testes` antes de abrir o pull request.
