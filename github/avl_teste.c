#include <stdio.h>
#include <string.h>
#include "avl.h"

Registro fazer_registro(int id, const char *nome) {
    Registro r;
    r.id_registro = id; r.id_jogador = id;
    strncpy(r.nome, nome, 63); r.nome[63] = '\0';
    r.temporada = 2024; r.minutos = 30;
    r.pontos = id; r.rebotes = 5; r.assistencias = 4; r.roubos = 1;
    r.vitoria = -1; r.lances_conv = 2; r.lances_tent = 2;
    return r;
}

/* Mostra os ids em ordem crescente (percurso "em ordem" da arvore). */
void em_ordem(NoAVL *raiz) {
    if (raiz == NULL) return;
    em_ordem(raiz->esq);
    printf("%d ", raiz->dado.id_registro);
    em_ordem(raiz->dir);
}

int main(void) {
    NoAVL *arv = NULL;

    /* Inserimos as chaves 1..7 EM ORDEM. Numa arvore comum isso viraria
       uma "lista" de altura 7. A AVL deve segurar a altura baixa. */
    const char *nomes[] = {"a","b","c","d","e","f","g"};
    for (int i = 1; i <= 7; i++)
        arv = avl_inserir(arv, fazer_registro(i, nomes[i-1]));

    printf("Inseri as chaves 1..7 em ordem crescente.\n");
    printf("Tamanho: %d nos | Altura da arvore: %d\n", avl_tamanho(arv), avl_altura(arv));
    printf("(Numa arvore desequilibrada a altura seria 7; numa AVL fica ~3)\n");
    printf("Raiz: id=%d (deveria ser 4, o do meio)\n", arv->dado.id_registro);

    printf("Em ordem: "); em_ordem(arv); printf("\n");

    NoAVL *a = avl_buscar(arv, 6);
    printf("Busca id=6 -> %s\n", a ? "achei" : "nao achei");

    printf("Removendo id=4 (a raiz)...\n");
    arv = avl_remover(arv, 4);
    printf("Nova altura: %d | Nova raiz: id=%d\n", avl_altura(arv), arv->dado.id_registro);
    printf("Em ordem: "); em_ordem(arv); printf("\n");

    avl_liberar(arv);
    return 0;
}
