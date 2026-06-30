#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "leitor.h"

/* Copia o campo de indice 'alvo' de uma linha CSV para 'destino'.
   Lida corretamente com campos vazios (ex.: ",,"). */
static int pegar_campo(const char *linha, int alvo, char *destino, int tam) {
    int campo = 0, j = 0;
    destino[0] = '\0';
    for (int i = 0; linha[i] != '\0'; i++) {
        char c = linha[i];
        if (c == ',') {
            if (campo == alvo) { destino[j] = '\0'; return 1; }
            campo++; j = 0;
        } else if (c == '\n' || c == '\r') {
            /* ignora */
        } else {
            if (campo == alvo && j < tam - 1) destino[j++] = c;
        }
    }
    if (campo == alvo) { destino[j] = '\0'; return 1; }
    return 0;
}

/* "38:24" -> 38 minutos. Campo vazio -> -1 (jogador nao entrou). */
static int parse_minutos(const char *s) {
    if (s[0] == '\0') return -1;
    int min = 0, seg = 0;
    sscanf(s, "%d:%d", &min, &seg);
    return min;
}

int carregar_csv(const char *caminho, Registro *destino, int max) {
    FILE *f = fopen(caminho, "r");
    if (!f) { printf("ERRO: nao consegui abrir %s\n", caminho); return -1; }

    char linha[2048], campo[128];
    char *cab = fgets(linha, sizeof(linha), f);  /* descarta o cabecalho */
    (void) cab;

    int n = 0;
    while (fgets(linha, sizeof(linha), f) != NULL && n < max) {
        pegar_campo(linha, 14, campo, sizeof(campo));
        int min = parse_minutos(campo);
        if (min < 0) continue;        /* DNP: descarta */

        Registro r;
        r.id_registro = n;            /* chave sequencial */
        pegar_campo(linha, 9,  campo, sizeof(campo)); r.id_jogador  = atoi(campo);
        pegar_campo(linha, 10, campo, sizeof(campo)); strncpy(r.nome, campo, 63); r.nome[63] = '\0';
        pegar_campo(linha, 0,  campo, sizeof(campo)); r.temporada   = atoi(campo);
        pegar_campo(linha, 2,  campo, sizeof(campo)); r.id_jogo     = atoi(campo);
        pegar_campo(linha, 7,  campo, sizeof(campo)); strncpy(r.time, campo, 3); r.time[3] = '\0';
        r.minutos = min;
        pegar_campo(linha, 32, campo, sizeof(campo)); r.pontos       = atof(campo);
        pegar_campo(linha, 26, campo, sizeof(campo)); r.rebotes      = atof(campo);
        pegar_campo(linha, 27, campo, sizeof(campo)); r.assistencias = atof(campo);
        pegar_campo(linha, 28, campo, sizeof(campo)); r.roubos       = atof(campo);
        pegar_campo(linha, 21, campo, sizeof(campo)); r.lances_conv  = atoi(campo);
        pegar_campo(linha, 22, campo, sizeof(campo)); r.lances_tent  = atoi(campo);
        r.vitoria = -1;

        destino[n++] = r;
    }
    fclose(f);
    return n;
}
