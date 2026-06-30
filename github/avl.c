#include <stdio.h>
#include <stdlib.h>
#include "avl.h"

/* ---------- funcoes auxiliares ---------- */

int avl_altura(NoAVL *n) {
    return n ? n->altura : 0;          /* no inexistente tem altura 0 */
}

static int maior(int a, int b) {
    return a > b ? a : b;
}

/* fator de balanceamento = altura(esq) - altura(dir).
   Se passar de +1 ou -1, o no esta desequilibrado. */
static int fator(NoAVL *n) {
    return n ? avl_altura(n->esq) - avl_altura(n->dir) : 0;
}

static NoAVL* novo_no(Registro r) {
    NoAVL *n = malloc(sizeof(NoAVL));
    n->dado = r;
    n->esq = n->dir = NULL;
    n->altura = 1;                     /* folha tem altura 1 */
    return n;
}

/* ---------- as duas rotacoes ---------- */
/* Rotacao a DIREITA: usada quando o lado esquerdo pesou demais. */
static NoAVL* rotaciona_direita(NoAVL *y) {
    NoAVL *x  = y->esq;
    NoAVL *T2 = x->dir;
    x->dir = y;                        /* y desce para a direita de x */
    y->esq = T2;                       /* T2 vira filho esquerdo de y */
    y->altura = 1 + maior(avl_altura(y->esq), avl_altura(y->dir));
    x->altura = 1 + maior(avl_altura(x->esq), avl_altura(x->dir));
    return x;                          /* x e a nova raiz desta subarvore */
}

/* Rotacao a ESQUERDA: espelho da anterior, quando a direita pesou. */
static NoAVL* rotaciona_esquerda(NoAVL *x) {
    NoAVL *y  = x->dir;
    NoAVL *T2 = y->esq;
    y->esq = x;
    x->dir = T2;
    x->altura = 1 + maior(avl_altura(x->esq), avl_altura(x->dir));
    y->altura = 1 + maior(avl_altura(y->esq), avl_altura(y->dir));
    return y;
}

/* ---------- insercao ---------- */
NoAVL* avl_inserir(NoAVL *raiz, Registro r) {
    int chave = r.id_registro;

    /* 1) insercao normal de arvore de busca */
    if (raiz == NULL) return novo_no(r);
    if (chave < raiz->dado.id_registro)
        raiz->esq = avl_inserir(raiz->esq, r);
    else if (chave > raiz->dado.id_registro)
        raiz->dir = avl_inserir(raiz->dir, r);
    else
        return raiz;                   /* chave repetida: ignora */

    /* 2) atualiza a altura deste no */
    raiz->altura = 1 + maior(avl_altura(raiz->esq), avl_altura(raiz->dir));

    /* 3) verifica o equilibrio e corrige com rotacoes (4 casos) */
    int fb = fator(raiz);
    if (fb > 1 && chave < raiz->esq->dado.id_registro)              /* Esq-Esq */
        return rotaciona_direita(raiz);
    if (fb < -1 && chave > raiz->dir->dado.id_registro)            /* Dir-Dir */
        return rotaciona_esquerda(raiz);
    if (fb > 1 && chave > raiz->esq->dado.id_registro) {           /* Esq-Dir */
        raiz->esq = rotaciona_esquerda(raiz->esq);
        return rotaciona_direita(raiz);
    }
    if (fb < -1 && chave < raiz->dir->dado.id_registro) {          /* Dir-Esq */
        raiz->dir = rotaciona_direita(raiz->dir);
        return rotaciona_esquerda(raiz);
    }
    return raiz;
}

/* ---------- busca ---------- */
/* A cada no, descarta METADE da arvore. Isto e O(log n). */
NoAVL* avl_buscar(NoAVL *raiz, int id) {
    while (raiz != NULL) {
        if (id == raiz->dado.id_registro) return raiz;
        raiz = (id < raiz->dado.id_registro) ? raiz->esq : raiz->dir;
    }
    return NULL;
}

/* ---------- remocao ---------- */
static NoAVL* menor_no(NoAVL *n) {
    while (n->esq != NULL) n = n->esq;
    return n;
}

NoAVL* avl_remover(NoAVL *raiz, int id) {
    if (raiz == NULL) return NULL;

    if (id < raiz->dado.id_registro)
        raiz->esq = avl_remover(raiz->esq, id);
    else if (id > raiz->dado.id_registro)
        raiz->dir = avl_remover(raiz->dir, id);
    else {
        /* achou o no a remover */
        if (raiz->esq == NULL || raiz->dir == NULL) {
            NoAVL *filho = raiz->esq ? raiz->esq : raiz->dir;
            if (filho == NULL) { free(raiz); return NULL; } /* 0 filhos */
            NoAVL *tmp = raiz; raiz = filho; free(tmp);     /* 1 filho  */
        } else {
            /* 2 filhos: copia o sucessor (menor da direita) e remove-o */
            NoAVL *suc = menor_no(raiz->dir);
            raiz->dado = suc->dado;
            raiz->dir  = avl_remover(raiz->dir, suc->dado.id_registro);
        }
    }

    if (raiz == NULL) return NULL;     /* arvore ficou vazia */

    raiz->altura = 1 + maior(avl_altura(raiz->esq), avl_altura(raiz->dir));
    int fb = fator(raiz);
    if (fb > 1 && fator(raiz->esq) >= 0) return rotaciona_direita(raiz);
    if (fb > 1 && fator(raiz->esq) <  0) { raiz->esq = rotaciona_esquerda(raiz->esq); return rotaciona_direita(raiz); }
    if (fb < -1 && fator(raiz->dir) <= 0) return rotaciona_esquerda(raiz);
    if (fb < -1 && fator(raiz->dir) >  0) { raiz->dir = rotaciona_direita(raiz->dir); return rotaciona_esquerda(raiz); }
    return raiz;
}

/* ---------- utilitarios ---------- */
int avl_tamanho(NoAVL *raiz) {
    if (raiz == NULL) return 0;
    return 1 + avl_tamanho(raiz->esq) + avl_tamanho(raiz->dir);
}

void avl_liberar(NoAVL *raiz) {
    if (raiz == NULL) return;
    avl_liberar(raiz->esq);
    avl_liberar(raiz->dir);
    free(raiz);
}
