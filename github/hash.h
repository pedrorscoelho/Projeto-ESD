#ifndef HASH_H
#define HASH_H

#include "registro.h"

/* Cada balde (bucket) e uma listinha encadeada de nos.
   Quando duas chaves caem no mesmo balde (colisao), elas
   ficam na mesma listinha. */
typedef struct NoHash {
    Registro       dado;
    struct NoHash *prox;
} NoHash;

typedef struct {
    int       num_baldes;   /* M: quantos baldes existem        */
    NoHash  **baldes;       /* vetor de M ponteiros para listas  */
    int       n;            /* quantos registros estao guardados */
    int       colisoes;     /* quantas insercoes cairam em balde ja ocupado */
} TabelaHash;

TabelaHash* hash_criar(int num_baldes);
void        hash_inserir(TabelaHash *t, Registro r);
NoHash*     hash_buscar(TabelaHash *t, int id);
void        hash_remover(TabelaHash *t, int id);
double      hash_fator_carga(TabelaHash *t);   /* n / M */
int         hash_maior_cadeia(TabelaHash *t);  /* tamanho da maior listinha */
void        hash_liberar(TabelaHash *t);

#endif
