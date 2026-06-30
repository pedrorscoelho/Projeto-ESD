#include <stdio.h>
#include <string.h>
#include "lista.h"

/* Cria um registro de mentira so para testar (sem precisar do CSV). */
Registro fazer_registro(int id, const char *nome, float pts) {
    Registro r;
    r.id_registro = id;
    strncpy(r.nome, nome, 63); r.nome[63] = '\0';
    r.pontos = pts;
    r.id_jogador = id; r.temporada = 2024; r.minutos = 30;
    r.rebotes = 5; r.assistencias = 4; r.roubos = 1;
    r.vitoria = -1; r.lances_conv = 2; r.lances_tent = 2;
    return r;
}

void mostrar(No *cabeca) {
    printf("Lista (tamanho %d): ", lista_tamanho(cabeca));
    for (No *p = cabeca; p != NULL; p = p->prox)
        printf("[id=%d %s %.0fpts] -> ", p->dado.id_registro, p->dado.nome, p->dado.pontos);
    printf("NULL\n");
}

int main(void) {
    No *lista = NULL;  /* lista comeca VAZIA */

    /* INSERIR 4 registros (cada um entra no inicio) */
    lista = lista_inserir(lista, fazer_registro(0, "Curry",  30));
    lista = lista_inserir(lista, fazer_registro(1, "Doncic", 33));
    lista = lista_inserir(lista, fazer_registro(2, "Jokic",  27));
    lista = lista_inserir(lista, fazer_registro(3, "Tatum",  28));
    mostrar(lista);

    /* BUSCAR o id 1 */
    No *achado = lista_buscar(lista, 1);
    if (achado) printf("Busca id=1 -> achei: %s\n", achado->dado.nome);
    else        printf("Busca id=1 -> nao achei\n");

    /* REMOVER o id 2 (Jokic, esta no meio) */
    printf("Removendo id=2...\n");
    lista = lista_remover(lista, 2);
    mostrar(lista);

    /* REMOVER o id 3 (Tatum, e a cabeca) */
    printf("Removendo id=3 (cabeca)...\n");
    lista = lista_remover(lista, 3);
    mostrar(lista);

    lista_liberar(lista);  /* sempre devolver a memoria no fim */
    return 0;
}
