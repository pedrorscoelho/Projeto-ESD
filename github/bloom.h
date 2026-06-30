#ifndef BLOOM_H
#define BLOOM_H

/* O Bloom Filter NAO guarda os dados. Ele guarda apenas um vetor de BITS.
   Ao inserir uma chave, marca k posicoes do vetor com 1.
   Ao consultar, verifica se TODAS as k posicoes estao em 1.
   - Se alguma estiver em 0  -> a chave COM CERTEZA nunca foi inserida.
   - Se todas estiverem em 1  -> a chave PROVAVELMENTE foi inserida
                                 (pode ser um "falso positivo"). */
typedef struct {
    unsigned char *bits;       /* vetor de bytes; cada byte guarda 8 bits */
    int            num_bits;   /* m: total de bits                        */
    int            num_hashes; /* k: quantas posicoes marcamos por chave  */
    int            n;          /* quantas chaves foram inseridas          */
} BloomFilter;

BloomFilter* bloom_criar(int num_bits, int num_hashes);
void         bloom_inserir(BloomFilter *b, int chave);
int          bloom_contem(BloomFilter *b, int chave);   /* 0 = nao; 1 = provavelmente */
int          bloom_bytes(BloomFilter *b);               /* memoria usada, em bytes    */
void         bloom_liberar(BloomFilter *b);

#endif
