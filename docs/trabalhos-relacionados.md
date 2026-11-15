# Trabalhos relacionados e lacuna

## Contexto do projeto

O projeto recomenda um perfil de especialização (PES) e um segundo ciclo. A
base atual contém 14 perguntas com alternativas ponderadas para oito PES e três
ciclos; a pergunta 5 não tem pesos e, por isso, não discrimina perfis. O plano
experimental do README compara a ordem manual, a ordem aleatória e a ordem por
ganho de informação, observando quantidade de perguntas e acurácia.

## Trabalhos relacionados

Dados sintéticos são úteis para testar procedimentos quando se conhecem os
rótulos verdadeiros e se quer controlar a dificuldade, mas não substituem a
validação com respondentes reais. Em divulgação estatística, Reiter (2005) e
Drechsler (2011) descrevem a criação e o uso de microdados sintéticos; Nowok,
Raab e Dibben (2016) apresentam uma ferramenta prática de geração. Goncalves
et al. (2020) discutem geração e avaliação de dados sintéticos em saúde, e
Jordon et al. (2022) sistematizam objetivos e critérios de avaliação.

Em testes adaptativos computadorizados (CAT), a próxima questão é escolhida
com base nas respostas anteriores, buscando equilibrar precisão e custo de
aplicação. Weiss (1982) discute eficiência de testes adaptativos; Wainer et al.
(2000) oferecem uma introdução prática; e van der Linden e Glas (2010)
reúnem métodos e fundamentos. Chang e Ying (1996) propõem seleção de itens
baseada em informação global. Lord (1980) e Embretson e Reise (2000) apresentam
fundamentos da teoria de resposta ao item que sustentam várias abordagens CAT.

Árvores de decisão e ganho de informação também têm relação direta com a
pergunta experimental do projeto. Quinlan (1986) descreve a indução de árvores
de decisão, e Shannon (1948) estabelece a noção de entropia usada em medidas de
informação. Para interpretação vocacional, Holland (1997) é uma referência
clássica sobre interesses e escolhas profissionais; no entanto, os rótulos PES
e ciclos deste projeto são categorias locais, não equivalentes automáticas a
uma tipologia vocacional validada.

## Lacuna no escopo do projeto

Na combinação específica proposta pelo grupo — oito PES, três ciclos,
perguntas com alternativas de três ou quatro opções, e comparação de três
ordenações pelo número de perguntas e pela acurácia — falta uma avaliação
controlada que explicite o efeito de diferentes níveis de ruído e preserve
perfis de referência conhecidos. O gerador implementado permite esse primeiro
experimento reproduzível. Isso é uma lacuna operacional do estudo, não uma
afirmação de que não existam pesquisas semelhantes na literatura.

Os resultados sintéticos medem o comportamento do algoritmo sob as hipóteses
codificadas nos pesos das alternativas. Como os perfis típicos são derivados
desses mesmos pesos, a avaliação não comprova validade vocacional nem
generalização para estudantes reais. Uma etapa futura deve confrontar as
recomendações com respostas reais, revisar pesos e perfis com especialistas e
relatar métricas por rótulo, além da acurácia agregada.

## Matriz de síntese — 15 referências-base candidatas

**Atenção:** não havia bibliografia do grupo no repositório inspecionado.
Portanto, esta matriz reúne 15 referências-base candidatas para consolidação;
ela não confirma que as referências já coletadas por todas as pessoas do grupo
somam 15. As referências 1–3 e 4–6 cobrem, respectivamente, dados sintéticos
e CAT.

| # | Referência | Síntese / contribuição | Relação e limite para o projeto |
|---|---|---|---|
| 1 | Reiter, J. P. (2005). “Releasing multiply imputed, synthetic public use microdata: An illustration and empirical study.” *Journal of the Royal Statistical Society: Series A*, 168(1), 185–205. | Discute liberação e avaliação de microdados sintéticos. | Fundamenta dados sintéticos; foco em divulgação estatística, não em perfil vocacional. |
| 2 | Drechsler, J. (2011). *Synthetic Datasets for Statistical Disclosure Control: Theory and Implementation*. Springer. | Apresenta princípios e implementação de conjuntos sintéticos para controle de divulgação. | Ajuda a justificar geração controlada; não define um CAT. |
| 3 | Nowok, B., Raab, G. M., & Dibben, C. (2016). “synthpop: Bespoke Creation of Synthetic Data in R.” *Journal of Statistical Software*, 74(11), 1–26. | Descreve métodos e ferramenta de geração de dados sintéticos. | Referência de metodologia; a ferramenta não é usada neste projeto. |
| 4 | Weiss, D. J. (1982). “Improving measurement quality and efficiency with adaptive testing.” *Applied Psychological Measurement*, 6(4), 473–492. | Relaciona adaptação, eficiência e qualidade de medida. | Referência CAT; usa teoria de medida, diferente dos pesos locais do projeto. |
| 5 | Wainer, H., Dorans, N. J., Flaugher, R., Green, B. F., Mislevy, R. J., Steinberg, L., & Thissen, D. (2000). *Computerized Adaptive Testing: A Primer* (2nd ed.). Lawrence Erlbaum Associates. | Introduz princípios, desenho e avaliação de CAT. | Base para discutir adaptação; não avalia as categorias PES/ciclo. |
| 6 | van der Linden, W. J., & Glas, C. A. W. (Eds.). (2010). *Elements of Adaptive Testing*. Springer. | Reúne modelos, seleção de itens e critérios em testes adaptativos. | Apoia a discussão metodológica; o algoritmo do projeto é baseado em árvore e ordem de perguntas. |
| 7 | Chang, H.-H., & Ying, Z. (1996). “A global information approach to computerized adaptive testing.” *Applied Psychological Measurement*, 20(3), 213–229. | Apresenta seleção de itens por informação global. | Próximo à ideia de ganho de informação; contexto de CAT/IRT não é idêntico ao experimento. |
| 8 | Lord, F. M. (1980). *Applications of Item Response Theory to Practical Testing Problems*. Lawrence Erlbaum Associates. | Apresenta fundamentos e aplicações de teoria de resposta ao item. | Contextualiza medidas adaptativas; não é fonte de validação dos rótulos do projeto. |
| 9 | Embretson, S. E., & Reise, S. P. (2000). *Item Response Theory for Psychologists*. Lawrence Erlbaum Associates. | Introduz modelos de resposta ao item e suas aplicações. | Apoia trabalho futuro com calibração de itens; não é implementado pelo protótipo atual. |
| 10 | Goncalves, A., Ray, P., Soper, B., Stevens, J., Coyle, L., & Sales, A. P. (2020). “Generation and evaluation of synthetic patient data.” *BMC Medical Research Methodology*, 20, 108. | Discute geração e avaliação de dados sintéticos em saúde. | Exemplo de avaliação contextualizada; domínio não vocacional. |
| 11 | Jordon, J., Szpruch, L., Houssiau, F., Bottarelli, M., Cherubin, G., Maple, C., et al. (2022). “Synthetic Data—What, Why and How?” arXiv:2205.03257. | Sistematiza conceitos, motivações e avaliação de dados sintéticos. | Referência panorâmica; recomenda avaliação além da mera geração. |
| 12 | Quinlan, J. R. (1986). “Induction of decision trees.” *Machine Learning*, 1, 81–106. | Apresenta indução de árvores de decisão. | Relaciona-se às árvores e à ordenação de perguntas, embora não trate de CAT vocacional. |
| 13 | Shannon, C. E. (1948). “A mathematical theory of communication.” *Bell System Technical Journal*, 27(3), 379–423; 27(4), 623–656. | Formula conceitos de entropia e informação. | Fundamento conceitual para ganho de informação; não é estudo de avaliação vocacional. |
| 14 | Holland, J. L. (1997). *Making Vocational Choices: A Theory of Vocational Personalities and Work Environments* (3rd ed.). Psychological Assessment Resources. | Discute interesses vocacionais e ambientes de trabalho. | Contextualiza recomendação vocacional; categorias não são intercambiáveis com os PES locais. |
| 15 | Reiter, J. P. (2003). “Inference for partially synthetic, public use microdata sets.” *Survey Methodology*, 29(2), 181–188. | Examina inferência a partir de dados parcialmente sintéticos. | Complementa o uso experimental de dados artificiais; não trata de CAT. |


