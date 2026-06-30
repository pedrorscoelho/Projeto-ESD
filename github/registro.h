#ifndef REGISTRO_H
#define REGISTRO_H

/* Esta é a "caixa" que representa o desempenho de UM jogador em UM jogo.
   Todo o sistema gira em torno dela. */
typedef struct {
    int   id_registro;   /* CHAVE de busca: numeramos nós mesmos (0, 1, 2, ...) */
    int   id_jogador;    /* personId do CSV */
    char  nome[64];      /* personName */
    int   temporada;     /* "2010-11" guardamos como 2010 */
    int   id_jogo;       /* gameId: identifica a partida (para vitoria/derrota) */
    char  time[4];       /* teamTricode, ex.: "LAL" */
    int   minutos;       /* minutos jogados (convertidos de "MM:SS") */
    float pontos;        /* points */
    float rebotes;       /* reboundsTotal */
    float assistencias;  /* assists */
    float roubos;        /* steals */
    int   vitoria;       /* 1=venceu, 0=perdeu, -1=ainda nao calculado */
    int   lances_conv;   /* freeThrowsMade  */
    int   lances_tent;   /* freeThrowsAttempted */
} Registro;

#endif
