#ifndef OPERACOES_H
#define OPERACOES_H

#include "registro.h"

/* Resumo de um jogador numa temporada (resultado da agregacao). */
typedef struct {
    int    id_jogador;
    char   nome[64];
    int    temporada;
    int    jogos;            /* quantos jogos disputou        */
    double media_pts, desvio_pts;
    double media_reb, media_ast, media_stl;
    double aproveitamento_ll; /* % de lances livres (0..1)    */
    double pontuacao_mvp;     /* indice composto (operacao 2) */
} ResumoJogador;

/* Posicao de um time numa temporada (resultado da operacao 3). */
typedef struct {
    char tri[4];
    int  temporada;
    int  vitorias;
    int  jogos;
} TimeTemporada;

/* OPERACAO 1: agrega os registros por (jogador, temporada).
   Devolve um vetor alocado e escreve a quantidade em *qtd. */
ResumoJogador* op_estatisticas(Registro *dados, int n, int *qtd);

/* OPERACAO 2: calcula o indice de MVP de cada resumo e ORDENA do maior
   para o menor (classificacao). Altera o vetor recebido. */
void op_ranking_mvp(ResumoJogador *res, int qtd);

/* OPERACAO 3: descobre quem venceu cada jogo (preenche dados[i].vitoria)
   e devolve a tabela de vitorias por time/temporada, ordenada. */
TimeTemporada* op_playoffs(Registro *dados, int n, int *qtd);

#endif
