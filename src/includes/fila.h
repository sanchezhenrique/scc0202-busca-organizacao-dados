#ifndef FILA_H
#define FILA_H

#include <stdbool.h>
#include "item.h"

typedef struct fila FILA;

/* Inserir retém uma referencia; remover transfere a referencia ao chamador. */
FILA *fila_criar(void);
void fila_apagar(FILA **fila);
bool fila_inserir(FILA *fila, ITEM *item);
ITEM *fila_remover(FILA *fila);
bool fila_vazia(FILA *fila);
int fila_tamanho(FILA *fila);
void fila_imprimir_status(FILA *fila);

#endif
