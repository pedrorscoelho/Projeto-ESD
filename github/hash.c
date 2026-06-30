#include <stdio.h>
#include <stdlib.h>
#include "hash.h"

/* A funcao hash: transforma a chave num indice de balde (0 .. M-1).
   Para chaves inteiras, o resto da divisao por M e simples e eficaz. */
static int funcao_hash(int chave, int num_baldes) {
    /* (unsigned) evita indice negativo caso a chave fosse negativa */
    return ((unsigned int) chave) % num_baldes;
}

TabelaHash* hash_criar(int num_baldes) {
    TabelaHash *t = malloc(sizeof(TabelaHash));
    t->num_baldes = num_baldes;
    t->n = 0;
    t->colisoes = 0;
    /* calloc ja deixa todos os ponteiros como NULL (baldes vazios) */
    t->baldes = calloc(num_baldes, sizeof(NoHash*));
    return t;
}

void hash_inserir(TabelaHash *t, Registro r) {
    int i = funcao_hash(r.id_registro, t->num_baldes);

    if (t->baldes[i] != NULL)   /* o balde ja tinha alguem: colisao */
        t->colisoes++;

    NoHash *novo = malloc(sizeof(NoHash));
    novo->dado = r;
    novo->prox = t->baldes[i];  /* insere no inicio da listinha do balde */
    t->baldes[i] = novo;
    t->n++;
}

NoHash* hash_buscar(TabelaHash *t, int id) {
    int i = funcao_hash(id, t->num_baldes);   /* vai direto ao balde certo */
    NoHash *p = t->baldes[i];
    while (p != NULL) {                         /* percorre so aquela listinha */
        if (p->dado.id_registro == id) return p;
        p = p->prox;
    }
    return NULL;
}

void hash_remover(TabelaHash *t, int id) {
    int i = funcao_hash(id, t->num_baldes);
    NoHash *p = t->baldes[i];
    NoHash *ant = NULL;
    while (p != NULL && p->dado.id_registro != id) {
        ant = p;
        p = p->prox;
    }
    if (p == NULL) return;            /* nao achou */
    if (ant == NULL) t->baldes[i] = p->prox;  /* era o primeiro do balde */
    else             ant->prox = p->prox;
    free(p);
    t->n--;
}

double hash_fator_carga(TabelaHash *t) {
    return (double) t->n / t->num_baldes;
}

int hash_maior_cadeia(TabelaHash *t) {
    int maior = 0;
    for (int i = 0; i < t->num_baldes; i++) {
        int c = 0;
        for (NoHash *p = t->baldes[i]; p != NULL; p = p->prox) c++;
        if (c > maior) maior = c;
    }
    return maior;
}

void hash_liberar(TabelaHash *t) {
    for (int i = 0; i < t->num_baldes; i++) {
        NoHash *p = t->baldes[i];
        while (p != NULL) {
            NoHash *prox = p->prox;
            free(p);
            p = prox;
        }
    }
    free(t->baldes);
    free(t);
}
