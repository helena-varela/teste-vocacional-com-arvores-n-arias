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
include/              cabeçalhos (.hpp)
src/                  implementação (.cpp)
  arvore.cpp          estrutura N-ária: nós, pré-ordem, BFS, altura, contagens
  terminal.cpp        leitura e validação das respostas no terminal
  perguntas.cpp       banco de perguntas: carregar_perguntas(), nome_rotulo(), ordem_manual()
  perfis.cpp          carregar_perfis(): lê os CSVs de perfis sintéticos
  construtores.cpp    árvores por ordem manual, ordem aleatória e ganho de informação (ID3)
  classificador.cpp   classificar(): percorre a árvore da raiz até uma folha
  experimento.cpp     teste interativo (opção 1) e experimentos (opção 4)
  exemplo.cpp         árvores pequenas montadas à mão (usadas só nos testes)
  main.cpp            menu; as árvores do teste são as do ganho de informação,
                      construídas com dados/sinteticos/ruido_10pct_treino.csv
dados/
  perguntas.txt       banco de perguntas no formato P;id;texto / A;letra;texto;ROTULO=peso
  sinteticos/         perfis sintéticos (treino e teste por nível de ruído)
resultados/           CSVs gerados pela opção 4 do menu
scripts/
  gerar_perfis.py     gera os perfis sintéticos e o gráfico de aderência
tests/                testes unitários (C++ e Python)
  dados/              CSV pequeno e fixo usado pelos testes
```

## Menu

| Opção | O que faz |
| --- | --- |
| 1 | Faz o teste: percorre a árvore de PES e depois a de ciclo. Pergunta já respondida não se repete (cache). |
| 2 | Mostra a árvore de PES em pré-ordem, com altura, nós, folhas e profundidade média |
| 3 | O mesmo para a árvore de 2º ciclo |
| 4 | Roda os experimentos e grava os CSVs em `resultados/` |

## Divisão dos módulos

| Pessoa | Módulo |
| --- | --- |
| Pessoa 1 | Estrutura da árvore N-ária, menu e leitura de respostas (`arvore`, `terminal`, `main`) |
| Pessoa 2 | Banco de perguntas (`perguntas.txt`) e `carregar_perguntas()` |
| Pessoa 3 | Gerador de perfis simulados e script Python dos gráficos |
| Pessoa 4 | Construtores das árvores (ordem manual, aleatória e por ganho de informação) |
| Pessoa 5 | Leitura dos perfis, classificador, teste interativo e experimentos (gera os CSVs) |

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

## Experimentos

A opção 4 do menu (`make run` e depois `4`) constrói as árvores com os perfis
de treino de cada nível de ruído e as avalia nos perfis de teste:

- estratégias: ordem manual, ordem aleatória (30 execuções, sementes 42 a 71)
  e ganho de informação;
- alvos: `PES`, `CICLO` e `CONJUNTO` (PES e ciclo juntos, com o cache: só
  acerta se os dois rótulos estiverem certos, e conta as perguntas distintas
  dos dois caminhos).

Saída em `resultados/`:

| Arquivo | Conteúdo |
| --- | --- |
| `resultados_execucoes.csv` | uma linha por árvore construída: `ruido,alvo,arvore,semente,acuracia,media_perguntas,max_perguntas,altura,nos,folhas` |
| `resultados_resumo.csv` | média e desvio padrão por ruído, alvo e estratégia: `ruido,alvo,arvore,execucoes,acuracia_media,acuracia_dp,media_perguntas_media,media_perguntas_dp` |

Os resultados são determinísticos: rodar de novo com os mesmos perfis gera
os mesmos CSVs.

## Combinados do grupo

- Cada pessoa trabalha no seu próprio branch e abre um pull request. Outra pessoa revisa antes do merge.
- Rode `make testes` antes de abrir o pull request.
