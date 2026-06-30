#ifndef SKIPLIST_H
#define SKIPLIST_H

#include "registro.h"

#define SKIP_MAX_NIVEL 16   /* teto de niveis; 16 aguenta ~65 mil elementos */

/* Cada no tem um VETOR de ponteiros "prox": um para cada nivel em que ele
   participa. prox[0] e a lista de baixo (tem todo mundo); prox[1], prox[2]...
   sao as "pistas expressas" que pulam varios nos de uma vez. */
typedef struct NoSkip {
    Registro        dado;
    struct NoSkip **prox;   /* vetor de ponteiros (tamanho = nivel do no) */
} NoSkip;

typedef struct {
    NoSkip *cabeca;          /* no-sentinela: so serve de ponto de partida */
    int     nivel;           /* maior nivel atualmente em uso              */
    int     n;               /* quantos registros existem                  */
    long    total_ponteiros; /* soma dos niveis de todos os nos (memoria)  */
} SkipList;

SkipList* skip_criar(void);
void      skip_inserir(SkipList *s, Registro r);
NoSkip*   skip_buscar(SkipList *s, int id);
void      skip_remover(SkipList *s, int id);
void      skip_liberar(SkipList *s);

#endif
