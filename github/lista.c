#include <stdio.h>
#include <stdlib.h>   /* malloc, free */
#include "lista.h"

/* Insere um registro no INICIO da lista (operacao O(1), bem rapida).
   Recebe a cabeca atual e devolve a nova cabeca. */
No* lista_inserir(No *cabeca, Registro r) {
    No *novo = malloc(sizeof(No));   /* pede memoria para 1 no */
    if (novo == NULL) {              /* memoria pode acabar    */
        printf("ERRO: sem memoria\n");
        return cabeca;
    }
    novo->dado = r;        /* guarda o registro no no       */
    novo->prox = cabeca;   /* o novo aponta para o antigo 1o */
    return novo;           /* o novo passa a ser a cabeca    */
}

/* Procura, do inicio ao fim, o no cujo id_registro == id.
   Caminha no por no ate achar ou chegar em NULL. Isto e O(n). */
No* lista_buscar(No *cabeca, int id) {
    No *atual = cabeca;
    while (atual != NULL) {
        if (atual->dado.id_registro == id) return atual; /* achou */
        atual = atual->prox;                             /* anda  */
    }
    return NULL;  /* percorreu tudo e nao achou */
}

/* Remove o no com aquele id. Precisa "costurar" o anterior ao proximo. */
No* lista_remover(No *cabeca, int id) {
    No *atual = cabeca;
    No *anterior = NULL;

    /* anda ate achar o no ou acabar a lista */
    while (atual != NULL && atual->dado.id_registro != id) {
        anterior = atual;
        atual = atual->prox;
    }

    if (atual == NULL) return cabeca;             /* nao achou: nada muda     */

    if (anterior == NULL)                         /* o no era a propria cabeca */
        cabeca = atual->prox;
    else                                          /* costura: anterior pula o atual */
        anterior->prox = atual->prox;

    free(atual);                                  /* devolve a memoria do no   */
    return cabeca;
}

int lista_tamanho(No *cabeca) {
    int n = 0;
    for (No *p = cabeca; p != NULL; p = p->prox) n++;
    return n;
}

void lista_liberar(No *cabeca) {
    while (cabeca != NULL) {
        No *prox = cabeca->prox;  /* guarda o proximo ANTES de liberar */
        free(cabeca);
        cabeca = prox;
    }
}
