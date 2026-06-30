# Sistema de Análise de Dados da NBA — Estruturas de Dados 2026

Sistema em C que ingere um fluxo de estatísticas de jogos da NBA (box scores por
jogador), organiza os dados em diferentes estruturas e permite consultá-los,
analisá-los e compará-los empiricamente. Desenvolvido para a disciplina de
Estrutura de Dados.

Autor: «seu nome»

## Visão geral

Cada registro do sistema representa **o desempenho de um jogador em um jogo**
(pontos, rebotes, assistências, roubos, lances livres, etc.). Os dados são lidos
de um CSV, tratados, indexados em cinco estruturas de dados diferentes e expostos
através de uma interface de terminal. O sistema também executa benchmarks
comparativos e testes sob condições restritivas.

## Dataset

* Fonte: repositório público **NBA-Data-2010-2024** (box scores da temporada regular).
* Arquivo usado: `regular\_season\_box\_scores\_2010\_2024\_part\_1.csv`.
* Coloque o arquivo na pasta `dados/` (veja a estrutura abaixo).

> Observação: o CSV completo é grande. Se ultrapassar o limite do GitHub (100 MB
> por arquivo), não faça o commit do arquivo inteiro — documente o link de
> download aqui e, se quiser, inclua apenas uma amostra pequena no repositório.

### Tratamento de dados aplicado

* Conversão do campo de minutos do formato `"MM:SS"` para minutos inteiros.
* Descarte de linhas DNP (*Did Not Play*): registros sem minutos são ignorados.
* Lances livres armazenados como convertidos/tentados (e não como porcentagem),
com proteção contra divisão por zero ao calcular o aproveitamento.
* Atribuição de uma chave sequencial única (`id\_registro`) a cada registro válido.

## Requisitos

* Compilador C (`gcc`)
* `make`
* Biblioteca matemática padrão (`-lm`, já tratada no Makefile)

## Como compilar

```bash
make
```

Isso gera quatro executáveis:

|Executável|O que faz|
|-|-|
|`sistema`|Interface de terminal (menu) que integra tudo|
|`benchmark`|Compara lista, AVL, hash e skip list|
|`otimizacao\_bench`|Compara a lista original com a versão compactada|
|`restricoes`|Roda os cinco testes sob condições restritivas|

Para limpar os binários: `make clean`.

## Como executar

```bash
./sistema dados/regular\_season\_box\_scores\_2010\_2024\_part\_1.csv
./benchmark dados/regular\_season\_box\_scores\_2010\_2024\_part\_1.csv
./otimizacao\_bench dados/regular\_season\_box\_scores\_2010\_2024\_part\_1.csv
./restricoes dados/regular\_season\_box\_scores\_2010\_2024\_part\_1.csv
```

No menu do `sistema` é possível buscar registros, ver estatísticas de um jogador,
consultar o ranking de MVP, a classificação dos times, rodar um mini-benchmark e
inspecionar a Skip List por dentro.

## Estruturas de dados implementadas

Três da ementa e duas de fora. As quatro primeiras respondem à mesma pergunta
(busca por chave), o que torna o benchmark entre elas comparável; o Bloom Filter
responde a uma pergunta diferente (teste de pertinência) e tem seu próprio critério.

|Estrutura|Arquivo|Busca (caso médio)|
|-|-|-|
|Lista encadeada|`lista.c`|O(n)|
|Árvore AVL|`avl.c`|O(log n)|
|Tabela hash (encadeamento)|`hash.c`|O(1)|
|Skip list|`skiplist.c`|O(log n) esperado|
|Bloom filter|`bloom.c`|O(k)|

## Operações adicionais

Implementadas em `operacoes.c`:

1. Estatísticas por jogador/temporada (médias, desvio padrão, aproveitamento de
lances livres), via ordenação e varredura linear.
2. Ranking de MVP, a partir de um índice composto e transparente.
3. Classificação dos times (vitórias por temporada), que também deriva o resultado
de cada jogo (vitória/derrota) somando os pontos de cada time.

## Otimização

`lista\_comp.c` contém uma versão compactada da lista encadeada (bloco contíguo de
memória, índices no lugar de ponteiros), além de uma variante v2 que separa o dado
"quente" (chave) do "frio" (registro completo) para melhorar o uso do cache. A
comparação completa está no `otimizacao\_bench`.

## Estrutura do repositório

```
.
├── Makefile
├── README.md
├── USO\_DE\_IA.md
├── dados/
│   └── regular\_season\_box\_scores\_2010\_2024\_part\_1.csv
├── registro.h
├── leitor.c / leitor.h
├── lista.c / lista.h
├── lista\_comp.c / lista\_comp.h
├── avl.c / avl.h
├── hash.c / hash.h
├── skiplist.c / skiplist.h
├── bloom.c / bloom.h
├── operacoes.c / operacoes.h
├── main.c                (programa principal: sistema)
├── benchmark.c
├── otimizacao\_bench.c
├── restricoes.c
└── testes/               (programas de teste de cada módulo, opcional)
    ├── lista\_teste.c
    ├── avl\_teste.c
    ├── hash\_teste.c
    ├── skiplist\_teste.c
    ├── bloom\_teste.c
    └── operacoes\_teste.c
```

## Uso de inteligência artificial

O apoio de IA na codificação está declarado em `USO\_DE\_IA.md`, conforme exigido
pelo enunciado. O relatório foi escrito sem uso de IA.

## Limitações conhecidas

* A classificação para playoffs é simplificada (melhores por número de vitórias);
não modela as duas conferências da NBA real.
* O índice de MVP é uma heurística de projeto, não a votação oficial da liga.

