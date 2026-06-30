#include <stdio.h>
#include <stdlib.h>
#include "skiplist.h"

/* Sorteia o nivel de um no novo: comeca em 1 e, a cada "cara" (50%),
   sobe um nivel. Resultado: ~1/2 dos nos tem nivel 1, ~1/4 tem nivel 2,
   ~1/8 tem nivel 3... e por isso as pistas de cima sao cada vez mais raras. */
static int nivel_aleatorio(void) {
    int nivel = 1;
    while ((rand() & 1) && nivel < SKIP_MAX_NIVEL)
        nivel++;
    return nivel;
}

static NoSkip* novo_no(Registro r, int nivel) {
    NoSkip *n = malloc(sizeof(NoSkip));
    n->dado = r;
    n->prox = calloc(nivel, sizeof(NoSkip*));  /* todos os niveis em NULL */
    return n;
}

SkipList* skip_criar(void) {
    SkipList *s = malloc(sizeof(SkipList));
    s->nivel = 1;
    s->n = 0;
    s->total_ponteiros = 0;
    s->cabeca = malloc(sizeof(NoSkip));
    s->cabeca->prox = calloc(SKIP_MAX_NIVEL, sizeof(NoSkip*));
    return s;
}

NoSkip* skip_buscar(SkipList *s, int id) {
    NoSkip *x = s->cabeca;
    /* desce do nivel mais alto ao mais baixo */
    for (int i = s->nivel - 1; i >= 0; i--) {
        /* anda para a direita enquanto o proximo for MENOR que o alvo */
        while (x->prox[i] != NULL && x->prox[i]->dado.id_registro < id)
            x = x->prox[i];
        /* quando o proximo passa do alvo, DESCE um nivel (o for cuida) */
    }
    x = x->prox[0];   /* no nivel 0, o proximo e o candidato */
    if (x != NULL && x->dado.id_registro == id) return x;
    return NULL;
}

void skip_inserir(SkipList *s, Registro r) {
    NoSkip *update[SKIP_MAX_NIVEL];  /* guarda, por nivel, o no antes do ponto de insercao */
    NoSkip *x = s->cabeca;

    for (int i = s->nivel - 1; i >= 0; i--) {
        while (x->prox[i] != NULL && x->prox[i]->dado.id_registro < r.id_registro)
            x = x->prox[i];
        update[i] = x;
    }

    x = x->prox[0];
    if (x != NULL && x->dado.id_registro == r.id_registro) return; /* duplicada */

    int nv = nivel_aleatorio();
    if (nv > s->nivel) {                       /* o no novo e mais alto que tudo */
        for (int i = s->nivel; i < nv; i++)
            update[i] = s->cabeca;             /* niveis novos saem da cabeca   */
        s->nivel = nv;
    }

    NoSkip *novo = novo_no(r, nv);
    for (int i = 0; i < nv; i++) {             /* costura em cada nivel ate nv  */
        novo->prox[i] = update[i]->prox[i];
        update[i]->prox[i] = novo;
    }
    s->total_ponteiros += nv;
    s->n++;
}

void skip_remover(SkipList *s, int id) {
    NoSkip *update[SKIP_MAX_NIVEL];
    NoSkip *x = s->cabeca;

    for (int i = s->nivel - 1; i >= 0; i--) {
        while (x->prox[i] != NULL && x->prox[i]->dado.id_registro < id)
            x = x->prox[i];
        update[i] = x;
    }
    x = x->prox[0];
    if (x == NULL || x->dado.id_registro != id) return; /* nao achou */

    for (int i = 0; i < s->nivel; i++) {
        if (update[i]->prox[i] != x) break;    /* x nao participa deste nivel */
        update[i]->prox[i] = x->prox[i];
        s->total_ponteiros--;
    }
    free(x->prox);
    free(x);

    while (s->nivel > 1 && s->cabeca->prox[s->nivel - 1] == NULL)
        s->nivel--;                            /* enxuga niveis que ficaram vazios */
    s->n--;
}

void skip_liberar(SkipList *s) {
    NoSkip *x = s->cabeca->prox[0];
    while (x != NULL) {
        NoSkip *prox = x->prox[0];
        free(x->prox);
        free(x);
        x = prox;
    }
    free(s->cabeca->prox);
    free(s->cabeca);
    free(s);
}
