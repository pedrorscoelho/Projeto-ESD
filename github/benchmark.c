#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "leitor.h"
#include "lista.h"
#include "avl.h"
#include "hash.h"
#include "skiplist.h"

#define MAX_REG   2000000
#define BUSCAS    5000     /* numero fixo de buscas (para comparar custo por busca) */
#define REMOCOES  2000     /* numero fixo de remocoes                              */

/* relogio de alta precisao, em milissegundos */
static double agora_ms(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec * 1000.0 + t.tv_nsec / 1e6;
}

Registro *dados;   /* vetor com todos os registros lidos do CSV */
volatile long sink = 0;  /* impede o compilador de "otimizar" as buscas embora */

int main(int argc, char **argv) {
    const char *arquivo = (argc > 1) ? argv[1] : "nba_grande.csv";
    srand(7);

    dados = malloc(sizeof(Registro) * MAX_REG);
    int total = carregar_csv(arquivo, dados, MAX_REG);
    if (total <= 0) { printf("Falha ao carregar.\n"); return 1; }
    printf("Carregados %d registros de %s\n\n", total, arquivo);

    int tamanhos[] = {1000, 5000, 10000, 25000, 50000, 100000, 200000, 500000};
    int num_tam = sizeof(tamanhos) / sizeof(tamanhos[0]);

    printf("%-10s %-8s %12s %12s %12s %12s\n",
           "estrutura", "N", "ins(us/op)", "busca(us/op)", "rem(us/op)", "memoria(KB)");
    printf("---------------------------------------------------------------------------\n");

    for (int t = 0; t < num_tam; t++) {
        int N = tamanhos[t];
        if (N > total) break;
        double ti, tb, tr, mem;

        /* ---------- LISTA ENCADEADA ---------- */
        No *lista = NULL;
        ti = agora_ms();
        for (int i = 0; i < N; i++) lista = lista_inserir(lista, dados[i]);
        ti = agora_ms() - ti;
        tb = agora_ms();
        for (int b = 0; b < BUSCAS; b++) sink += lista_buscar(lista, rand() % N) ? 1 : 0;
        tb = agora_ms() - tb;
        tr = agora_ms();
        for (int r = 0; r < REMOCOES; r++) lista = lista_remover(lista, r);
        tr = agora_ms() - tr;
        mem = (double)((size_t)N * sizeof(No)) / 1024.0;
        printf("%-10s %-8d %12.4f %12.4f %12.4f %12.1f\n", "lista", N,
               ti*1000/N, tb*1000/BUSCAS, tr*1000/REMOCOES, mem);
        lista_liberar(lista);

        /* ---------- AVL ---------- */
        NoAVL *avl = NULL;
        ti = agora_ms();
        for (int i = 0; i < N; i++) avl = avl_inserir(avl, dados[i]);
        ti = agora_ms() - ti;
        tb = agora_ms();
        for (int b = 0; b < BUSCAS; b++) sink += avl_buscar(avl, rand() % N) ? 1 : 0;
        tb = agora_ms() - tb;
        tr = agora_ms();
        for (int r = 0; r < REMOCOES; r++) avl = avl_remover(avl, r);
        tr = agora_ms() - tr;
        mem = (double)((size_t)N * sizeof(NoAVL)) / 1024.0;
        printf("%-10s %-8d %12.4f %12.4f %12.4f %12.1f\n", "avl", N,
               ti*1000/N, tb*1000/BUSCAS, tr*1000/REMOCOES, mem);
        avl_liberar(avl);

        /* ---------- TABELA HASH ---------- */
        TabelaHash *h = hash_criar(N);   /* fator de carga ~1 */
        ti = agora_ms();
        for (int i = 0; i < N; i++) hash_inserir(h, dados[i]);
        ti = agora_ms() - ti;
        int col_hash = h->colisoes;              /* registra ANTES das remocoes */
        double carga_hash = hash_fator_carga(h);
        tb = agora_ms();
        for (int b = 0; b < BUSCAS; b++) sink += hash_buscar(h, rand() % N) ? 1 : 0;
        tb = agora_ms() - tb;
        tr = agora_ms();
        for (int r = 0; r < REMOCOES; r++) hash_remover(h, r);
        tr = agora_ms() - tr;
        mem = (double)((size_t)N * sizeof(NoHash) + (size_t)N * sizeof(NoHash*)) / 1024.0;
        printf("%-10s %-8d %12.4f %12.4f %12.4f %12.1f  [colisoes=%d, carga=%.2f]\n",
               "hash", N, ti*1000/N, tb*1000/BUSCAS, tr*1000/REMOCOES, mem,
               col_hash, carga_hash);
        hash_liberar(h);

        /* ---------- SKIP LIST ---------- */
        SkipList *s = skip_criar();
        ti = agora_ms();
        for (int i = 0; i < N; i++) skip_inserir(s, dados[i]);
        ti = agora_ms() - ti;
        tb = agora_ms();
        for (int b = 0; b < BUSCAS; b++) sink += skip_buscar(s, rand() % N) ? 1 : 0;
        tb = agora_ms() - tb;
        tr = agora_ms();
        for (int r = 0; r < REMOCOES; r++) skip_remover(s, r);
        tr = agora_ms() - tr;
        mem = (double)((size_t)N * sizeof(NoSkip) + (size_t)s->total_ponteiros * sizeof(NoSkip*)) / 1024.0;
        printf("%-10s %-8d %12.4f %12.4f %12.4f %12.1f\n", "skiplist", N,
               ti*1000/N, tb*1000/BUSCAS, tr*1000/REMOCOES, mem);
        skip_liberar(s);

        printf("---------------------------------------------------------------------------\n");
    }

    free(dados);
    return 0;
}
