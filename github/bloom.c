#include <stdio.h>
#include <stdlib.h>
#include "bloom.h"

/* ---- manipulacao de UM bit dentro do vetor de bytes ----
   O bit de indice i mora no byte (i / 8), na posicao (i % 8) dentro dele. */
static void liga_bit(BloomFilter *b, int i) {
    b->bits[i / 8] |= (unsigned char)(1 << (i % 8));   /* poe 1 naquele bit */
}
static int le_bit(BloomFilter *b, int i) {
    return (b->bits[i / 8] >> (i % 8)) & 1;            /* devolve 0 ou 1    */
}

/* "Embaralhador" de bits (avalanche): espalha bem ate chaves sequenciais
   como 0,1,2,3..., evitando que os bits marcados fiquem agrupados. */
static unsigned mistura(unsigned x) {
    x ^= x >> 16; x *= 0x7feb352du;
    x ^= x >> 15; x *= 0x846ca68bu;
    x ^= x >> 16;
    return x;
}

/* Duas funcoes hash base. Combinando as duas geramos k posicoes
   diferentes sem escrever k funcoes: pos_i = (h1 + i*h2) % m. */
static unsigned hash1(int chave) { return mistura((unsigned) chave); }
static unsigned hash2(int chave) { return mistura((unsigned) chave + 0x9e3779b9u) | 1u; }

BloomFilter* bloom_criar(int num_bits, int num_hashes) {
    BloomFilter *b = malloc(sizeof(BloomFilter));
    b->num_bits   = num_bits;
    b->num_hashes = num_hashes;
    b->n          = 0;
    int bytes = (num_bits + 7) / 8;          /* arredonda para cima */
    b->bits = calloc(bytes, 1);              /* todos os bits comecam em 0 */
    return b;
}

void bloom_inserir(BloomFilter *b, int chave) {
    unsigned h1 = hash1(chave), h2 = hash2(chave);
    for (int i = 0; i < b->num_hashes; i++) {
        int pos = (int) ((h1 + (unsigned) i * h2) % (unsigned) b->num_bits);
        liga_bit(b, pos);
    }
    b->n++;
}

int bloom_contem(BloomFilter *b, int chave) {
    unsigned h1 = hash1(chave), h2 = hash2(chave);
    for (int i = 0; i < b->num_hashes; i++) {
        int pos = (int) ((h1 + (unsigned) i * h2) % (unsigned) b->num_bits);
        if (le_bit(b, pos) == 0)
            return 0;                /* achou um bit 0: COM CERTEZA nao existe */
    }
    return 1;                        /* todos em 1: PROVAVELMENTE existe        */
}

int bloom_bytes(BloomFilter *b) {
    return (b->num_bits + 7) / 8 + (int) sizeof(BloomFilter);
}

void bloom_liberar(BloomFilter *b) {
    free(b->bits);
    free(b);
}
