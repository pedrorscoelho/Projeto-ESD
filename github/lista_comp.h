#ifndef LISTA_COMP_H
#define LISTA_COMP_H

#include "registro.h"

/* No da lista COMPACTADA: em vez de um ponteiro para o proximo no,
   guarda o INDICE do proximo no dentro de um mesmo bloco (-1 = fim).
   Indice (int, 4 bytes) ocupa menos que ponteiro (8 bytes em 64 bits). */
typedef struct {
    Registro dado;
    int      prox;   /* indice do proximo no, ou -1 */
} NoComp;

/* Toda a lista mora num unico bloco continuo ("arena"), alocado de uma
   vez so. Isso melhora o uso do cache e elimina N chamadas de malloc. */
typedef struct {
    NoComp *arena;        /* bloco continuo de nos        */
    int     capacidade;   /* tamanho do bloco             */
    int     topo;         /* proximo slot nunca usado     */
    int     cabeca;       /* indice do primeiro no (-1 vazio) */
    int     livre;        /* topo da lista de slots livres (-1) */
    int     n;            /* quantos nos em uso           */
} ListaComp;

ListaComp* listacomp_criar(int capacidade);
void       listacomp_inserir(ListaComp *L, Registro r);
NoComp*    listacomp_buscar(ListaComp *L, int id);
void       listacomp_remover(ListaComp *L, int id);
long       listacomp_memoria(ListaComp *L);   /* bytes do bloco */
void       listacomp_liberar(ListaComp *L);

/* ---- VERSAO 2: separa o dado "quente" (chave+prox) do "frio" (registro) ----
   A busca percorre apenas o array denso de celulas (8 bytes cada), tocando
   o registro completo so quando encontra. Muito mais amigavel ao cache. */
typedef struct {
    int chave;   /* id_registro */
    int prox;    /* indice do proximo, ou -1 */
} Celula;        /* 8 bytes: cabem 8 numa linha de cache de 64 bytes */

typedef struct {
    Celula   *cel;        /* array denso, so para percorrer */
    Registro *reg;        /* array paralelo com o dado completo */
    int       capacidade, topo, cabeca, livre, n;
} ListaComp2;

ListaComp2* listacomp2_criar(int capacidade);
void        listacomp2_inserir(ListaComp2 *L, Registro r);
Registro*   listacomp2_buscar(ListaComp2 *L, int id);
long        listacomp2_memoria(ListaComp2 *L);
void        listacomp2_liberar(ListaComp2 *L);

#endif
