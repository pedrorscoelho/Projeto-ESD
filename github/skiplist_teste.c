#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "skiplist.h"

Registro fazer_registro(int id) {
    Registro r;
    r.id_registro = id; r.id_jogador = id;
    snprintf(r.nome, 64, "j%d", id);
    r.temporada = 2024; r.minutos = 30;
    r.pontos = id; r.rebotes = 5; r.assistencias = 4; r.roubos = 1;
    r.vitoria = -1; r.lances_conv = 2; r.lances_tent = 2;
    return r;
}

/* Mostra cada nivel: do mais alto (pistas expressas) ao mais baixo. */
void mostrar_niveis(SkipList *s) {
    for (int i = s->nivel - 1; i >= 0; i--) {
        printf("nivel %d: ", i);
        for (NoSkip *p = s->cabeca->prox[i]; p != NULL; p = p->prox[i])
            printf("%d ", p->dado.id_registro);
        printf("\n");
    }
}

int main(void) {
    srand(42);  /* semente fixa: a mesma "sorte" toda vez, bom para teste */
    SkipList *s = skip_criar();

    for (int id = 1; id <= 12; id++)
        skip_inserir(s, fazer_registro(id));

    printf("Inseri as chaves 1..12. Estrutura em niveis:\n");
    mostrar_niveis(s);

    printf("\nBusca id=9 -> %s\n", skip_buscar(s, 9) ? "achei" : "nao achei");
    printf("Busca id=100 -> %s\n", skip_buscar(s, 100) ? "achei" : "nao achei");

    printf("Removendo id=6...\n");
    skip_remover(s, 6);
    mostrar_niveis(s);

    skip_liberar(s);
    return 0;
}
