#ifndef LISTA_H
#define LISTA_H

#include "registro.h"

/* Um "No" (nó) da lista: guarda UM registro e aponta para o proximo No.
   Repare em "struct No *prox": um No contem um ponteiro para outro No. */

typedef struct No {
    Registro    dado;
    struct No  *prox;   /* endereco do proximo no; NULL = fim da lista */
} No;

No*  lista_inserir(No *cabeca, Registro r);  /* insere no inicio, devolve nova cabeca */
No*  lista_buscar(No *cabeca, int id);       /* devolve o no com aquele id, ou NULL    */
No*  lista_remover(No *cabeca, int id);      /* remove o no com aquele id, devolve cabeca */
int  lista_tamanho(No *cabeca);              /* conta quantos nos existem               */
void lista_liberar(No *cabeca);              /* devolve toda a memoria ao sistema       */

#endif
