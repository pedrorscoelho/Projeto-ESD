#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "leitor.h"
#include "lista.h"
#include "hash.h"

#define MAX_REG 2000000

static double agora_ms(void) {
    struct timespec t; clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec * 1000.0 + t.tv_nsec / 1e6;
}

Registro *dados;
int total;

/* ===== R5 (MEMORIA): janela deslizante — descarta o registro mais antigo
   quando a estrutura atinge o limite W. A memoria para de crescer. ===== */
static void R5_memoria(int W) {
    TabelaHash *h = hash_criar(W * 2);
    int *ordem = malloc(sizeof(int) * W);   /* fila circular dos ultimos W ids */
    int ini = 0, qtd = 0, pico = 0;

    for (int i = 0; i < total; i++) {
        if (qtd == W) {                          /* cheia: remove o mais antigo */
            hash_remover(h, ordem[ini]);
            ini = (ini + 1) % W; qtd--;
        }
        hash_inserir(h, dados[i]);
        ordem[(ini + qtd) % W] = dados[i].id_registro;
        qtd++;
        if (qtd > pico) pico = qtd;
    }
    printf("[R5 memoria] stream de %d registros, janela W=%d\n", total, W);
    printf("   elementos vivos no fim: %d (pico: %d) -> memoria NAO cresce com o stream\n",
           h->n, pico);
    free(ordem); hash_liberar(h);
}

/* ===== R7 (PROCESSAMENTO): orcamento de tempo por busca. Conta quantas
   buscas estouram o prazo em cada estrutura. ===== */
static void R7_processamento(int N, double orcamento_us) {
    No *lista = NULL;
    for (int i = 0; i < N; i++) lista = lista_inserir(lista, dados[i]);
    TabelaHash *h = hash_criar(N);
    for (int i = 0; i < N; i++) hash_inserir(h, dados[i]);

    int K = 2000, miss_l = 0, miss_h = 0;
    volatile long s = 0;
    for (int b = 0; b < K; b++) {
        int c = rand() % N;
        double t0 = agora_ms(); s += lista_buscar(lista, c) ? 1 : 0;
        if ((agora_ms() - t0) * 1000.0 > orcamento_us) miss_l++;
    }
    for (int b = 0; b < K; b++) {
        int c = rand() % N;
        double t0 = agora_ms(); s += hash_buscar(h, c) ? 1 : 0;
        if ((agora_ms() - t0) * 1000.0 > orcamento_us) miss_h++;
    }
    printf("[R7 processamento] orcamento de %.1f us por busca (N=%d, %d buscas)\n",
           orcamento_us, N, K);
    printf("   lista: %5.1f%% das buscas estouraram o prazo\n", 100.0 * miss_l / K);
    printf("   hash : %5.1f%% das buscas estouraram o prazo\n", 100.0 * miss_h / K);
    lista_liberar(lista); hash_liberar(h);
}

/* ===== R12 (LATENCIA): fonte de dados lenta (delay por registro).
   Mostra que, sob alta latencia, a estrutura quase nao importa. ===== */
static void R12_latencia(int N, int delay_us) {
    TabelaHash *h = hash_criar(N);
    No *lista = NULL;
    double th = agora_ms();
    for (int i = 0; i < N; i++) { usleep(delay_us); hash_inserir(h, dados[i]); }
    th = agora_ms() - th;
    double tl = agora_ms();
    for (int i = 0; i < N; i++) { usleep(delay_us); lista = lista_inserir(lista, dados[i]); }
    tl = agora_ms() - tl;
    printf("[R12 latencia] fonte lenta: %d us por registro (N=%d)\n", delay_us, N);
    printf("   ingestao via hash : %7.1f ms\n", th);
    printf("   ingestao via lista: %7.1f ms\n", tl);
    printf("   piso teorico (so a latencia): %7.1f ms -> a estrutura e irrelevante aqui\n",
           (double) N * delay_us / 1000.0);
    hash_liberar(h); lista_liberar(lista);
}

/* ===== R18 (DADOS): 10%% dos registros com valor anomalo. Mostra como a
   media (e logo o ranking de MVP) fica comprometida. ===== */
static void R18_dados(double frac) {
    double soma_ok = 0, soma_ruim = 0;
    int corrompidos = 0;
    for (int i = 0; i < total; i++) {
        soma_ok += dados[i].pontos;
        double p = dados[i].pontos;
        if ((double) rand() / RAND_MAX < frac) { p = 9999; corrompidos++; }
        soma_ruim += p;
    }
    printf("[R18 dados] %.0f%% de valores anomalos (sensor defeituoso)\n", frac * 100);
    printf("   media de pontos REAL      : %.2f\n", soma_ok / total);
    printf("   media de pontos CORROMPIDA: %.2f  (%d registros adulterados)\n",
           soma_ruim / total, corrompidos);
    printf("   -> as estruturas funcionam, mas a CONCLUSAO (MVP) fica errada\n");
}

/* ===== R24 (ALGORITMICA): usar selection sort O(n^2) no lugar de
   quicksort O(n log n). ===== */
static int cmp_int(const void *a, const void *b) {
    return *(const int*)a - *(const int*)b;
}
static void R24_algoritmica(int N) {
    int *v1 = malloc(sizeof(int) * N), *v2 = malloc(sizeof(int) * N);
    for (int i = 0; i < N; i++) v1[i] = v2[i] = (int) dados[i].pontos;

    double tq = agora_ms();
    qsort(v1, N, sizeof(int), cmp_int);
    tq = agora_ms() - tq;

    double ts = agora_ms();
    for (int i = 0; i < N - 1; i++) {           /* selection sort */
        int m = i;
        for (int j = i + 1; j < N; j++) if (v2[j] < v2[m]) m = j;
        int tmp = v2[i]; v2[i] = v2[m]; v2[m] = tmp;
    }
    ts = agora_ms() - ts;

    printf("[R24 algoritmica] ordenar %d pontuacoes\n", N);
    printf("   quicksort      : %8.2f ms\n", tq);
    printf("   selection sort : %8.2f ms  (%.0fx mais lento)\n",
           ts, ts / (tq > 0 ? tq : 0.001));
    free(v1); free(v2);
}

int main(int argc, char **argv) {
    const char *arq = (argc > 1) ? argv[1] : "nba_grande.csv";
    srand(7);
    dados = malloc(sizeof(Registro) * MAX_REG);
    total = carregar_csv(arq, dados, MAX_REG);
    if (total <= 0) return 1;
    printf("Carregados %d registros.\n\n", total);

    R5_memoria(5000);                printf("\n");
    R7_processamento(50000, 1.0);    printf("\n");
    R12_latencia(2000, 100);         printf("\n");
    R18_dados(0.10);                 printf("\n");
    R24_algoritmica(20000);

    free(dados);
    return 0;
}
