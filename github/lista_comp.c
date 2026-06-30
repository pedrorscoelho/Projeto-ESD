#include <stdio.h>
#include <stdlib.h>
#include "lista_comp.h"

ListaComp* listacomp_criar(int capacidade) {
    ListaComp *L = malloc(sizeof(ListaComp));
    L->arena = malloc(sizeof(NoComp) * capacidade);  /* UM unico malloc grande */
    L->capacidade = capacidade;
    L->topo = 0;
    L->cabeca = -1;     /* -1 faz o papel do NULL */
    L->livre = -1;
    L->n = 0;
    return L;
}

/* Pega um slot livre: reaproveita um removido, ou avanca o topo. */
static int alocar_slot(ListaComp *L) {
    if (L->livre != -1) {                 /* ha um slot reciclado disponivel */
        int s = L->livre;
        L->livre = L->arena[s].prox;      /* livre aponta para o proximo livre */
        return s;
    }
    if (L->topo < L->capacidade)          /* senao, usa um slot virgem */
        return L->topo++;
    return -1;                            /* arena cheia */
}

void listacomp_inserir(ListaComp *L, Registro r) {
    int s = alocar_slot(L);
    if (s == -1) { printf("ERRO: arena cheia\n"); return; }
    L->arena[s].dado = r;
    L->arena[s].prox = L->cabeca;   /* aponta para o antigo primeiro (por indice) */
    L->cabeca = s;                  /* novo primeiro                              */
    L->n++;
}

NoComp* listacomp_buscar(ListaComp *L, int id) {
    int i = L->cabeca;
    while (i != -1) {                       /* caminha por INDICES, na arena */
        if (L->arena[i].dado.id_registro == id) return &L->arena[i];
        i = L->arena[i].prox;
    }
    return NULL;
}

void listacomp_remover(ListaComp *L, int id) {
    int i = L->cabeca, ant = -1;
    while (i != -1 && L->arena[i].dado.id_registro != id) {
        ant = i;
        i = L->arena[i].prox;
    }
    if (i == -1) return;                    /* nao achou */

    if (ant == -1) L->cabeca = L->arena[i].prox;       /* era a cabeca */
    else           L->arena[ant].prox = L->arena[i].prox;

    /* devolve o slot para a lista de livres (sem free: a arena continua) */
    L->arena[i].prox = L->livre;
    L->livre = i;
    L->n--;
}

long listacomp_memoria(ListaComp *L) {
    return (long) L->capacidade * sizeof(NoComp);
}

void listacomp_liberar(ListaComp *L) {
    free(L->arena);   /* UM unico free libera todos os nos */
    free(L);
}

/* ===================== VERSAO 2: separacao quente/frio ===================== */

ListaComp2* listacomp2_criar(int capacidade) {
    ListaComp2 *L = malloc(sizeof(ListaComp2));
    L->cel = malloc(sizeof(Celula)   * capacidade);  /* array denso "quente" */
    L->reg = malloc(sizeof(Registro) * capacidade);  /* array "frio"         */
    L->capacidade = capacidade;
    L->topo = 0; L->cabeca = -1; L->livre = -1; L->n = 0;
    return L;
}

void listacomp2_inserir(ListaComp2 *L, Registro r) {
    int s;
    if (L->livre != -1) { s = L->livre; L->livre = L->cel[s].prox; }
    else if (L->topo < L->capacidade) s = L->topo++;
    else return;
    L->reg[s] = r;
    L->cel[s].chave = r.id_registro;
    L->cel[s].prox  = L->cabeca;
    L->cabeca = s;
    L->n++;
}

Registro* listacomp2_buscar(ListaComp2 *L, int id) {
    int i = L->cabeca;
    while (i != -1) {                 /* percorre SO o array denso de celulas */
        if (L->cel[i].chave == id) return &L->reg[i]; /* toca o registro so aqui */
        i = L->cel[i].prox;
    }
    return NULL;
}

long listacomp2_memoria(ListaComp2 *L) {
    return (long) L->capacidade * (sizeof(Celula) + sizeof(Registro));
}

void listacomp2_liberar(ListaComp2 *L) {
    free(L->cel); free(L->reg); free(L);
}
