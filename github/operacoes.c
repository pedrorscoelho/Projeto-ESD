#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "operacoes.h"

/* ===================== OPERACAO 1: ESTATISTICAS ===================== */
/* Estrategia: ORDENAR os registros por (jogador, temporada) e depois
   percorrer uma unica vez, somando enquanto a chave nao muda. Quando
   muda, fechamos o resumo do jogador anterior. Isso e O(n log n). */

static int cmp_jogador_temp(const void *a, const void *b) {
    const Registro *x = a, *y = b;
    if (x->id_jogador != y->id_jogador) return x->id_jogador - y->id_jogador;
    return x->temporada - y->temporada;
}

ResumoJogador* op_estatisticas(Registro *dados, int n, int *qtd) {
    /* trabalhamos numa COPIA para nao baguncar a ordem original */
    Registro *copia = malloc(sizeof(Registro) * n);
    memcpy(copia, dados, sizeof(Registro) * n);
    qsort(copia, n, sizeof(Registro), cmp_jogador_temp);

    ResumoJogador *res = malloc(sizeof(ResumoJogador) * n); /* no maximo n resumos */
    int r = 0;

    int i = 0;
    while (i < n) {
        int j = i;
        /* acumuladores do jogador/temporada atual */
        double s_pts = 0, s_pts2 = 0, s_reb = 0, s_ast = 0, s_stl = 0;
        long ftm = 0, fta = 0;
        int jogos = 0;

        while (j < n &&
               copia[j].id_jogador == copia[i].id_jogador &&
               copia[j].temporada == copia[i].temporada) {
            double p = copia[j].pontos;
            s_pts  += p;
            s_pts2 += p * p;                 /* para o desvio padrao */
            s_reb  += copia[j].rebotes;
            s_ast  += copia[j].assistencias;
            s_stl  += copia[j].roubos;
            ftm    += copia[j].lances_conv;
            fta    += copia[j].lances_tent;
            jogos++;
            j++;
        }

        ResumoJogador *rj = &res[r++];
        rj->id_jogador = copia[i].id_jogador;
        strcpy(rj->nome, copia[i].nome);
        rj->temporada = copia[i].temporada;
        rj->jogos = jogos;
        rj->media_pts = s_pts / jogos;
        /* desvio = sqrt(media dos quadrados - quadrado da media) */
        double var = s_pts2 / jogos - rj->media_pts * rj->media_pts;
        rj->desvio_pts = var > 0 ? sqrt(var) : 0.0;
        rj->media_reb = s_reb / jogos;
        rj->media_ast = s_ast / jogos;
        rj->media_stl = s_stl / jogos;
        /* AQUI esta o cuidado com a divisao por zero dos lances livres! */
        rj->aproveitamento_ll = (fta > 0) ? (double) ftm / fta : 0.0;
        rj->pontuacao_mvp = 0;

        i = j; /* pula para o proximo jogador/temporada */
    }

    free(copia);
    *qtd = r;
    return res;
}

/* ===================== OPERACAO 2: RANKING DE MVP ===================== */
/* Indice composto e TRANSPARENTE: cada estatistica vale um peso que
   reflete, grosso modo, o quanto ela "agrega" ao jogo. Isto e uma
   heuristica nossa, justificada no relatorio -- nao a votacao real. */
static double calcular_mvp(const ResumoJogador *r) {
    return r->media_pts
         + 1.2 * r->media_reb
         + 1.5 * r->media_ast
         + 3.0 * r->media_stl;
}

static int cmp_mvp_desc(const void *a, const void *b) {
    double da = ((const ResumoJogador*)a)->pontuacao_mvp;
    double db = ((const ResumoJogador*)b)->pontuacao_mvp;
    if (db > da) return 1;
    if (db < da) return -1;
    return 0;
}

void op_ranking_mvp(ResumoJogador *res, int qtd) {
    for (int i = 0; i < qtd; i++)
        res[i].pontuacao_mvp = calcular_mvp(&res[i]);
    qsort(res, qtd, sizeof(ResumoJogador), cmp_mvp_desc);
}

/* ===================== OPERACAO 3: PLAYOFFS ===================== */
/* 1) Ordena por jogo. 2) Em cada jogo, soma os pontos de cada time e
   descobre o vencedor. 3) Marca dados[i].vitoria e conta a vitoria do
   time na tabela de classificacao. */

static int cmp_por_jogo(const void *a, const void *b) {
    const Registro *x = a, *y = b;
    if (x->id_jogo != y->id_jogo) return x->id_jogo - y->id_jogo;
    return strcmp(x->time, y->time);
}

/* acha (ou cria) a linha do time/temporada na tabela */
static TimeTemporada* achar_time(TimeTemporada *tab, int *ntab,
                                 const char *tri, int temp) {
    for (int k = 0; k < *ntab; k++)
        if (tab[k].temporada == temp && strcmp(tab[k].tri, tri) == 0)
            return &tab[k];
    TimeTemporada *t = &tab[(*ntab)++];
    strncpy(t->tri, tri, 3); t->tri[3] = '\0';
    t->temporada = temp; t->vitorias = 0; t->jogos = 0;
    return t;
}

static int cmp_vitorias_desc(const void *a, const void *b) {
    return ((const TimeTemporada*)b)->vitorias - ((const TimeTemporada*)a)->vitorias;
}

TimeTemporada* op_playoffs(Registro *dados, int n, int *qtd) {
    qsort(dados, n, sizeof(Registro), cmp_por_jogo);

    TimeTemporada *tab = malloc(sizeof(TimeTemporada) * n);
    int ntab = 0;

    int i = 0;
    while (i < n) {
        int fim = i;
        while (fim < n && dados[fim].id_jogo == dados[i].id_jogo) fim++;
        /* [i, fim) sao todos os registros deste jogo */

        /* soma pontos por time (um jogo tem no maximo 2 times) */
        char tA[4] = "", tB[4] = "";
        double pA = 0, pB = 0;
        for (int k = i; k < fim; k++) {
            if (tA[0] == '\0' || strcmp(dados[k].time, tA) == 0) {
                strncpy(tA, dados[k].time, 3); tA[3] = '\0'; pA += dados[k].pontos;
            } else {
                strncpy(tB, dados[k].time, 3); tB[3] = '\0'; pB += dados[k].pontos;
            }
        }

        int temp = dados[i].temporada;
        const char *vencedor = (pA >= pB) ? tA : tB;

        /* marca cada jogador e conta jogos/vitorias do time */
        for (int k = i; k < fim; k++) {
            dados[k].vitoria = (strcmp(dados[k].time, vencedor) == 0) ? 1 : 0;
        }
        if (tA[0]) { TimeTemporada *t = achar_time(tab, &ntab, tA, temp); t->jogos++; if (strcmp(tA,vencedor)==0) t->vitorias++; }
        if (tB[0]) { TimeTemporada *t = achar_time(tab, &ntab, tB, temp); t->jogos++; if (strcmp(tB,vencedor)==0) t->vitorias++; }

        i = fim;
    }

    qsort(tab, ntab, sizeof(TimeTemporada), cmp_vitorias_desc);
    *qtd = ntab;
    return tab;
}
