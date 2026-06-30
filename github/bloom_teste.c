#include <stdio.h>
#include "bloom.h"

int main(void) {
    /* m = 10000 bits, k = 7 hashes: bom para guardar ~1000 elementos. */
    BloomFilter *b = bloom_criar(10000, 7);

    /* Inserimos as chaves 0..999 */
    for (int id = 0; id < 1000; id++)
        bloom_inserir(b, id);

    /* 1) FALSO NEGATIVO nunca pode acontecer: todo inserido tem que dar "sim". */
    int falsos_negativos = 0;
    for (int id = 0; id < 1000; id++)
        if (!bloom_contem(b, id)) falsos_negativos++;

    /* 2) FALSO POSITIVO pode acontecer: chaves 1000..1999 NAO foram inseridas,
          mas algumas podem dar "provavelmente sim" por azar dos bits. */
    int falsos_positivos = 0;
    for (int id = 1000; id < 2000; id++)
        if (bloom_contem(b, id)) falsos_positivos++;

    printf("Inseridos: %d elementos\n", b->n);
    printf("Falsos negativos: %d  (OBS: sempre ser 0)\n", falsos_negativos);
    printf("Falsos positivos: %d em 1000 testes (%.1f%%)\n",
           falsos_positivos, falsos_positivos / 10.0);
    printf("Memoria do Bloom: %d bytes\n", bloom_bytes(b));
    printf("Uma tabela hash guardando 1000 ints gastaria ~%d bytes\n",
           1000 * (int)(sizeof(int) + sizeof(void*)));

    bloom_liberar(b);
    return 0;
}
