#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include "leitor.h"
#include "lista.h"
#include "avl.h"
#include "hash.h"
#include "skiplist.h"
#include "operacoes.h"

#define MAX_REG 2000000

/* estado global do sistema */
static Registro      *dados;
static int            total;
static TabelaHash    *indice;     /* busca rapida por id_registro */
static ResumoJogador *resumos;    /* estatisticas (ja ordenadas por MVP) */
static int            qres;
static TimeTemporada *tabela;     /* classificacao dos times */
static int            qtab;

/* le um inteiro do teclado; devolve INT_MIN se a entrada for invalida */
static int ler_int(void) {
    int x;
    if (scanf("%d", &x) != 1) {
        int c; while ((c = getchar()) != '\n' && c != EOF) {}
        return INT_MIN;
    }
    return x;
}

static double agora_ms(void) {
    struct timespec t; clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec * 1000.0 + t.tv_nsec / 1e6;
}

/* ---------- opcoes do menu ---------- */

static void buscar_registro(void) {
    printf("Digite o id_registro a buscar: ");
    int id = ler_int();
    NoHash *r = hash_buscar(indice, id);
    if (!r) { printf(">> id %d nao encontrado.\n", id); return; }
    Registro d = r->dado;
    printf(">> ENCONTRADO (busca O(1) na hash):\n");
    printf("   %s | temporada %d | time %s | %.0f pts, %.0f reb, %.0f ast, %.0f roubos | %s\n",
           d.nome, d.temporada, d.time, d.pontos, d.rebotes, d.assistencias, d.roubos,
           d.vitoria == 1 ? "venceu" : (d.vitoria == 0 ? "perdeu" : "?"));
}

static void estatisticas_jogador(void) {
    printf("Digite o id_jogador (personId): ");
    int idj = ler_int();
    int achou = 0;
    for (int i = 0; i < qres; i++) {
        if (resumos[i].id_jogador == idj) {
            ResumoJogador r = resumos[i];
            if (!achou) printf(">> Estatisticas de %s:\n", r.nome);
            printf("   temporada %d: %d jogos | %.1f pts (desvio %.1f) | %.1f reb | %.1f ast | LL %.0f%%\n",
                   r.temporada, r.jogos, r.media_pts, r.desvio_pts,
                   r.media_reb, r.media_ast, r.aproveitamento_ll * 100);
            achou = 1;
        }
    }
    if (!achou) printf(">> jogador %d nao encontrado.\n", idj);
}

static void ranking_mvp(void) {
    printf("Filtrar por temporada (0 = todas): ");
    int temp = ler_int();
    printf(">> TOP 10 do indice de MVP%s:\n", temp ? " (temporada filtrada)" : "");
    int mostrados = 0;
    for (int i = 0; i < qres && mostrados < 10; i++) {
        if (temp != 0 && resumos[i].temporada != temp) continue;
        printf("   %2d. %-22s (temp %d) indice = %.2f | %.1f pts/jogo\n",
               mostrados + 1, resumos[i].nome, resumos[i].temporada,
               resumos[i].pontuacao_mvp, resumos[i].media_pts);
        mostrados++;
    }
    if (mostrados == 0) printf("   (nenhum jogador nessa temporada)\n");
}

static void classificacao_times(void) {
    printf("Filtrar por temporada (0 = todas): ");
    int temp = ler_int();
    printf(">> CLASSIFICACAO (mais vitorias primeiro):\n");
    int mostrados = 0;
    for (int i = 0; i < qtab && mostrados < 16; i++) {
        if (temp != 0 && tabela[i].temporada != temp) continue;
        printf("   %2d. %-4s (temp %d): %d vitorias em %d jogos\n",
               mostrados + 1, tabela[i].tri, tabela[i].temporada,
               tabela[i].vitorias, tabela[i].jogos);
        mostrados++;
    }
    if (mostrados == 0) printf("   (nenhum time nessa temporada)\n");
}

static void mini_benchmark(void) {
    int N = total < 50000 ? total : 50000;
    int B = 5000;
    volatile long s = 0;
    printf(">> Mini-benchmark com N=%d registros, %d buscas:\n", N, B);

    No *l = NULL;
    for (int i = 0; i < N; i++) l = lista_inserir(l, dados[i]);
    double t = agora_ms();
    for (int b = 0; b < B; b++) s += lista_buscar(l, rand() % N) ? 1 : 0;
    printf("   lista: %.4f us/busca\n", (agora_ms() - t) * 1000 / B);
    lista_liberar(l);

    NoAVL *a = NULL;
    for (int i = 0; i < N; i++) a = avl_inserir(a, dados[i]);
    t = agora_ms();
    for (int b = 0; b < B; b++) s += avl_buscar(a, rand() % N) ? 1 : 0;
    printf("   avl  : %.4f us/busca\n", (agora_ms() - t) * 1000 / B);
    avl_liberar(a);

    t = agora_ms();
    for (int b = 0; b < B; b++) s += hash_buscar(indice, rand() % N) ? 1 : 0;
    printf("   hash : %.4f us/busca\n", (agora_ms() - t) * 1000 / B);
}

static void espiar_skiplist(void) {
    int N = total < 20000 ? total : 20000;
    SkipList *s = skip_criar();
    for (int i = 0; i < N; i++) skip_inserir(s, dados[i]);
    printf(">> Skip List com %d elementos (nos por nivel):\n", N);
    for (int nv = s->nivel - 1; nv >= 0; nv--) {
        int c = 0;
        for (NoSkip *p = s->cabeca->prox[nv]; p != NULL; p = p->prox[nv]) c++;
        printf("   nivel %2d: %d nos\n", nv, c);
    }
    printf("   (cada nivel acima tem ~metade do anterior: e o sorteio em acao)\n");
    skip_liberar(s);
}

int main(int argc, char **argv) {
    const char *arq = (argc > 1) ? argv[1] : "nba_grande.csv";
    srand(7);

    printf("Carregando %s ...\n", arq);
    dados = malloc(sizeof(Registro) * MAX_REG);
    total = carregar_csv(arq, dados, MAX_REG);
    if (total <= 0) { printf("Falha ao carregar o arquivo.\n"); return 1; }

    /* prepara as analises uma vez */
    resumos = op_estatisticas(dados, total, &qres);
    op_ranking_mvp(resumos, qres);
    tabela = op_playoffs(dados, total, &qtab);   /* tambem preenche vitoria */
    indice = hash_criar(total);
    for (int i = 0; i < total; i++) hash_inserir(indice, dados[i]);

    printf("Pronto! %d registros, %d jogadores-temporada, %d times-temporada.\n",
           total, qres, qtab);

    int op = -1;
    while (op != 0) {
        printf("\n========= SISTEMA NBA =========\n");
        printf(" 1 - Buscar registro por id\n");
        printf(" 2 - Estatisticas de um jogador\n");
        printf(" 3 - Ranking de MVP (top 10)\n");
        printf(" 4 - Classificacao dos times\n");
        printf(" 5 - Mini-benchmark das estruturas\n");
        printf(" 6 - Espiar a Skip List por dentro\n");
        printf(" 0 - Sair\n");
        printf("Escolha: ");

        op = ler_int();
        if (op == INT_MIN) { printf("Entrada invalida.\n"); continue; }

        switch (op) {
            case 1: buscar_registro();     break;
            case 2: estatisticas_jogador(); break;
            case 3: ranking_mvp();         break;
            case 4: classificacao_times(); break;
            case 5: mini_benchmark();      break;
            case 6: espiar_skiplist();     break;
            case 0: printf("Ate logo!\n"); break;
            default: printf("Opcao invalida.\n");
        }
    }
    free(resumos); free(tabela); hash_liberar(indice); free(dados);
    return 0;
}
