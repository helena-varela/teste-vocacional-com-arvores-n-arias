#!/usr/bin/env python3
"""Gera perfis sintéticos, CSVs de treino/teste e um PDF de diagnóstico."""

import argparse
import itertools
import random
from pathlib import Path
from typing import Dict, List, Mapping, Sequence, Tuple

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt
import pandas as pd


RAIZ_PROJETO = Path(__file__).resolve().parents[1]
ARQUIVO_PERGUNTAS = RAIZ_PROJETO / "dados" / "perguntas.txt"
DIRETORIO_SAIDA = RAIZ_PROJETO / "dados" / "sinteticos"
SEMENTE_PADRAO = 42
ROTULOS_PES = (
    "PES_BIO", "PES_DADOS", "PES_EDU", "PES_INOV",
    "PES_IA", "PES_IOT", "PES_JOGOS", "PES_SIG",
)
ROTULOS_CICLO = ("CICLO_IA", "CICLO_CC", "CICLO_ES")

Alternativa = Tuple[str, Mapping[str, int]]
Pergunta = Tuple[int, List[Alternativa]]


def carregar_perguntas(caminho: Path = ARQUIVO_PERGUNTAS) -> List[Pergunta]:
    """Lê perguntas e pesos do arquivo existente no formato P/A do projeto."""
    perguntas: List[Pergunta] = []
    atual = None
    rotulos_encontrados = set()

    for numero, linha in enumerate(caminho.read_text(encoding="utf-8").splitlines(), 1):
        linha = linha.strip()
        if not linha or linha.startswith("#"):
            continue
        campos = [campo.strip() for campo in linha.split(";")]
        if campos[0] == "P":
            if len(campos) < 3:
                raise ValueError("{}:{}: pergunta malformada".format(caminho, numero))
            atual = (int(campos[1]), [])
            perguntas.append(atual)
        elif campos[0] == "A":
            if atual is None or len(campos) < 3:
                raise ValueError("{}:{}: alternativa sem pergunta ou malformada".format(caminho, numero))
            pesos: Dict[str, int] = {}
            for campo in campos[3:]:
                if not campo:
                    continue
                if "=" not in campo:
                    raise ValueError("{}:{}: peso sem '=': {}".format(caminho, numero, campo))
                rotulo, valor = (parte.strip() for parte in campo.split("=", 1))
                pesos[rotulo] = int(valor)
                rotulos_encontrados.add(rotulo)
            atual[1].append((campos[1], pesos))
        else:
            raise ValueError("{}:{}: tipo de linha desconhecido".format(caminho, numero))

    if not perguntas:
        raise ValueError("{}: nenhuma pergunta encontrada".format(caminho))
    esperados = set(ROTULOS_PES + ROTULOS_CICLO)
    if not esperados.issubset(rotulos_encontrados):
        faltantes = ", ".join(sorted(esperados - rotulos_encontrados))
        raise ValueError("{}: rótulos sem pesos: {}".format(caminho, faltantes))
    return perguntas


def _perfis_tipicos(perguntas: Sequence[Pergunta]) -> Dict[Tuple[str, str], Dict[int, str]]:
    """Escolhe, em cada pergunta, a alternativa de maior peso combinado."""
    perfis = {}
    for pes, ciclo in itertools.product(ROTULOS_PES, ROTULOS_CICLO):
        escolhas = {}
        for id_pergunta, alternativas in perguntas:
            # max preserva a primeira alternativa em caso de empate.
            melhor = max(
                alternativas,
                key=lambda alternativa: alternativa[1].get(pes, 0)
                + alternativa[1].get(ciclo, 0),
            )
            escolhas[id_pergunta] = melhor[0]
        perfis[(pes, ciclo)] = escolhas
    return perfis


def _dividir_treino_teste(n: int, semente: int) -> List[str]:
    indices = list(range(n))
    random.Random(semente).shuffle(indices)
    treino = set(indices[: int(n * 0.70)])
    return ["treino" if indice in treino else "teste" for indice in range(n)]


def gerar_perfis(
    n: int,
    ruido: float,
    caminho_perguntas: Path = ARQUIVO_PERGUNTAS,
    semente: int = SEMENTE_PADRAO,
) -> pd.DataFrame:
    """Gera n respondentes; ruído é uma fração entre 0 e 1."""
    if n < 2:
        raise ValueError("n deve ser pelo menos 2 para haver treino e teste")
    if not 0 <= ruido <= 1:
        raise ValueError("ruido deve estar entre 0 e 1")

    perguntas = carregar_perguntas(caminho_perguntas)
    tipicos = _perfis_tipicos(perguntas)
    combinacoes = list(tipicos)
    rng_perfis = random.Random(semente)
    rng_respostas = random.Random(semente + 100_000 + round(ruido * 10_000))

    pares = [combinacoes[i % len(combinacoes)] for i in range(n)]
    rng_perfis.shuffle(pares)
    divisao = _dividir_treino_teste(n, semente)
    linhas = []
    for indice, (pes, ciclo) in enumerate(pares):
        linha = {
            "id": indice + 1,
            "divisao": divisao[indice],
            "ruido": ruido,
            "pes": pes,
            "ciclo": ciclo,
        }
        for id_pergunta, alternativas in perguntas:
            resposta_tipica = tipicos[(pes, ciclo)][id_pergunta]
            if rng_respostas.random() < ruido:
                resposta = rng_respostas.choice(alternativas)[0]
            else:
                resposta = resposta_tipica
            linha["Q{}".format(id_pergunta)] = resposta
        linhas.append(linha)
    return pd.DataFrame(linhas)


def _salvar_perfis_tipicos(
    tipicos: Mapping[Tuple[str, str], Mapping[int, str]],
    destino: Path,
) -> None:
    linhas = []
    for (pes, ciclo), escolhas in tipicos.items():
        linha = {"pes": pes, "ciclo": ciclo}
        linha.update({"Q{}".format(pergunta): letra for pergunta, letra in escolhas.items()})
        linhas.append(linha)
    pd.DataFrame(linhas).to_csv(destino, index=False)


def _taxa_resposta_tipica(
    dados: pd.DataFrame,
    tipicos: Mapping[Tuple[str, str], Mapping[int, str]],
    perguntas: Sequence[Pergunta],
) -> float:
    total = 0
    iguais = 0
    for linha in dados.to_dict(orient="records"):
        escolhas = tipicos[(linha["pes"], linha["ciclo"])]
        for id_pergunta, _ in perguntas:
            total += 1
            iguais += linha["Q{}".format(id_pergunta)] == escolhas[id_pergunta]
    return iguais / total if total else 0.0


def plotar_csvs(
    arquivos_csv: Sequence[Path],
    arquivo_pdf: Path,
    tipicos: Mapping[Tuple[str, str], Mapping[int, str]],
    perguntas: Sequence[Pergunta],
) -> None:
    """Lê os CSVs gerados e grava um gráfico comparando a aderência aos perfis."""
    pontos = []
    for arquivo in arquivos_csv:
        dados = pd.read_csv(arquivo, dtype={"pes": str, "ciclo": str})
        if dados.empty:
            continue
        pontos.append(
            (
                float(dados["ruido"].iloc[0]),
                str(dados["divisao"].iloc[0]),
                _taxa_resposta_tipica(dados, tipicos, perguntas),
            )
        )

    if not pontos:
        raise ValueError("nenhum CSV com dados foi encontrado para plotar")

    ruido_percentual = sorted({int(round(ponto[0] * 100)) for ponto in pontos})
    divisoes = ("treino", "teste")
    largura = 0.36
    posicoes = list(range(len(ruido_percentual)))
    figura, eixo = plt.subplots(figsize=(8, 5))
    for indice, divisao in enumerate(divisoes):
        taxas = []
        for percentual in ruido_percentual:
            valores = [
                ponto[2] for ponto in pontos
                if int(round(ponto[0] * 100)) == percentual and ponto[1] == divisao
            ]
            taxas.append(sum(valores) / len(valores) if valores else 0.0)
        deslocamento = (indice - 0.5) * largura
        barras = eixo.bar(
            [posicao + deslocamento for posicao in posicoes],
            taxas,
            largura,
            label=divisao.capitalize(),
        )
        eixo.bar_label(barras, fmt="%.2f", padding=2)

    eixo.set(
        title="Respostas iguais à alternativa típica por nível de ruído",
        xlabel="Ruído configurado",
        ylabel="Proporção de respostas",
        xticks=posicoes,
        xticklabels=["{}%".format(valor) for valor in ruido_percentual],
        ylim=(0, 1.05),
    )
    eixo.legend()
    figura.tight_layout()
    arquivo_pdf.parent.mkdir(parents=True, exist_ok=True)
    figura.savefig(arquivo_pdf, format="pdf")
    plt.close(figura)


def executar(n: int, ruidos: Sequence[float], semente: int, saida: Path) -> None:
    perguntas = carregar_perguntas()
    tipicos = _perfis_tipicos(perguntas)
    saida.mkdir(parents=True, exist_ok=True)
    _salvar_perfis_tipicos(tipicos, saida / "perfis_tipicos.csv")

    csvs = []
    for ruido in ruidos:
        dados = gerar_perfis(n, ruido, semente=semente)
        percentual = int(round(ruido * 100))
        for divisao in ("treino", "teste"):
            arquivo = saida / "ruido_{}pct_{}.csv".format(percentual, divisao)
            dados[dados["divisao"] == divisao].to_csv(arquivo, index=False)
            csvs.append(arquivo)

    plotar_csvs(csvs, saida / "aderencia_perfil.pdf", tipicos, perguntas)
    print("CSVs e gráfico PDF salvos em {}".format(saida))


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--n", type=int, default=1200, help="número de respondentes por cenário")
    parser.add_argument(
        "--ruidos",
        default="0.10,0.25,0.40",
        help="frações separadas por vírgula (padrão: 0.10,0.25,0.40)",
    )
    parser.add_argument("--semente", type=int, default=SEMENTE_PADRAO)
    parser.add_argument("--saida", type=Path, default=DIRETORIO_SAIDA)
    argumentos = parser.parse_args()
    try:
        ruidos = [float(valor.strip()) for valor in argumentos.ruidos.split(",")]
        if not ruidos:
            raise ValueError("informe pelo menos um nível de ruído")
        for ruido in ruidos:
            if not 0 <= ruido <= 1:
                raise ValueError("cada nível de ruído deve estar entre 0 e 1")
        executar(argumentos.n, ruidos, argumentos.semente, argumentos.saida)
    except (OSError, ValueError) as erro:
        parser.error(str(erro))


if __name__ == "__main__":
    main()
