#ifndef LEITOR_H
#define LEITOR_H

#include "registro.h"

/* Le o CSV e preenche o vetor 'destino' (que o chamador alocou).
   Carrega no maximo 'max' registros. Retorna quantos leu (ou -1 em erro). */
int carregar_csv(const char *caminho, Registro *destino, int max);

#endif
