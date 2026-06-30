#ifndef AVL_H
#define AVL_H

#include "registro.h"

/* No da AVL: guarda um registro, aponta para dois filhos (esquerda/direita)
   e guarda a propria altura (usada para medir o desequilibrio). */
typedef struct NoAVL {
    Registro       dado;
    struct NoAVL  *esq;
    struct NoAVL  *dir;
    int            altura;
} NoAVL;

NoAVL* avl_inserir(NoAVL *raiz, Registro r);   /* insere e reequilibra        */
NoAVL* avl_buscar(NoAVL *raiz, int id);        /* busca por id_registro        */
NoAVL* avl_remover(NoAVL *raiz, int id);       /* remove e reequilibra         */
int    avl_altura(NoAVL *n);                   /* altura de um no (0 se NULL)  */
int    avl_tamanho(NoAVL *raiz);               /* quantos nos existem          */
void   avl_liberar(NoAVL *raiz);               /* devolve toda a memoria       */

#endif
