#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "leitor.h"
#include "lista.h"
#include "lista_comp.h"

#define MAX_REG 2000000
#define BUSCAS  20000

static double agora_ms(void) {
    struct timespec t; clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec * 1000.0 + t.tv_nsec / 1e6;
}

Registro *dados;
volatile long sink = 0;

int main(int argc, char **argv) {
    const char *arq = (argc > 1) ? argv[1] : "nba_grande.csv";
    srand(7);
    dados = malloc(sizeof(Registro) * MAX_REG);
    int total = carregar_csv(arq, dados, MAX_REG);
    if (total <= 0) return 1;
    printf("Carregados %d registros.\n", total);
    printf("Tamanho de um no: original=%zu bytes | compactado=%zu bytes\n\n",
           sizeof(No), sizeof(NoComp));

    int tamanhos[] = {10000, 25000, 50000, 100000, 200000};
    int nt = sizeof(tamanhos)/sizeof(tamanhos[0]);

    printf("%-18s %-8s %12s %14s %12s\n",
           "versao", "N", "ins(ms)", "busca(us/op)", "memoria(KB)");
    printf("-----------------------------------------------------------------\n");

    for (int t = 0; t < nt; t++) {
        int N = tamanhos[t];
        if (N > total) break;
        double ti, tb, mem;

        /* ----- LISTA ORIGINAL (N mallocs, nos espalhados) ----- */
        No *orig = NULL;
        ti = agora_ms();
        for (int i = 0; i < N; i++) orig = lista_inserir(orig, dados[i]);
        ti = agora_ms() - ti;
        tb = agora_ms();
        for (int b = 0; b < BUSCAS; b++) sink += lista_buscar(orig, rand()%N) ? 1 : 0;
        tb = agora_ms() - tb;
        mem = (double)((size_t)N * sizeof(No)) / 1024.0;
        printf("%-18s %-8d %12.2f %14.4f %12.1f\n", "original", N,
               ti, tb*1000/BUSCAS, mem);
        lista_liberar(orig);

        /* ----- LISTA COMPACTADA (1 malloc, bloco continuo) ----- */
        ListaComp *comp = listacomp_criar(N);
        ti = agora_ms();
        for (int i = 0; i < N; i++) listacomp_inserir(comp, dados[i]);
        ti = agora_ms() - ti;
        tb = agora_ms();
        for (int b = 0; b < BUSCAS; b++) sink += listacomp_buscar(comp, rand()%N) ? 1 : 0;
        tb = agora_ms() - tb;
        mem = (double) listacomp_memoria(comp) / 1024.0;
        printf("%-18s %-8d %12.2f %14.4f %12.1f\n", "compactada", N,
               ti, tb*1000/BUSCAS, mem);
        listacomp_liberar(comp);

        /* ----- LISTA COMPACTADA v2 (quente/frio separados) ----- */
        ListaComp2 *c2 = listacomp2_criar(N);
        ti = agora_ms();
        for (int i = 0; i < N; i++) listacomp2_inserir(c2, dados[i]);
        ti = agora_ms() - ti;
        tb = agora_ms();
        for (int b = 0; b < BUSCAS; b++) sink += listacomp2_buscar(c2, rand()%N) ? 1 : 0;
        tb = agora_ms() - tb;
        mem = (double) listacomp2_memoria(c2) / 1024.0;
        printf("%-18s %-8d %12.2f %14.4f %12.1f\n", "compactada v2", N,
               ti, tb*1000/BUSCAS, mem);
        listacomp2_liberar(c2);

        printf("-----------------------------------------------------------------\n");
    }
    free(dados);
    return 0;
}
