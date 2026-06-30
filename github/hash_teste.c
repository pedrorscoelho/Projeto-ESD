#include <stdio.h>
#include <string.h>
#include "hash.h"

Registro fazer_registro(int id) {
    Registro r;
    r.id_registro = id; r.id_jogador = id;
    snprintf(r.nome, 64, "jogador_%d", id);
    r.temporada = 2024; r.minutos = 30;
    r.pontos = id; r.rebotes = 5; r.assistencias = 4; r.roubos = 1;
    r.vitoria = -1; r.lances_conv = 2; r.lances_tent = 2;
    return r;
}

int main(void) {
    /* Tabela pequena de proposito (5 baldes) para FORCAR colisoes. */
    TabelaHash *t = hash_criar(5);

    /* Inserimos 12 registros (chaves 0..11). Com 5 baldes, varios
       vao cair no mesmo lugar: chave % 5. */
    for (int id = 0; id < 12; id++)
        hash_inserir(t, fazer_registro(id));

    printf("Inseri 12 registros em 5 baldes.\n");
    printf("Fator de carga (n/M): %.2f\n", hash_fator_carga(t));
    printf("Colisoes registradas: %d\n", t->colisoes);
    printf("Maior listinha (cadeia): %d elementos\n", hash_maior_cadeia(t));

    printf("\nDistribuicao por balde:\n");
    for (int i = 0; i < t->num_baldes; i++) {
        printf("  balde %d: ", i);
        for (NoHash *p = t->baldes[i]; p != NULL; p = p->prox)
            printf("%d ", p->dado.id_registro);
        printf("\n");
    }

    NoHash *a = hash_buscar(t, 7);
    printf("\nBusca id=7 -> %s\n", a ? a->dado.nome : "nao achei");

    printf("Removendo id=7...\n");
    hash_remover(t, 7);
    a = hash_buscar(t, 7);
    printf("Busca id=7 de novo -> %s\n", a ? a->dado.nome : "nao achei");

    hash_liberar(t);
    return 0;
}
