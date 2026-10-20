#ifndef LISTA_GEN_H
#define LISTA_GEN_H

#include <stdbool.h>
#include "item.h"
#include "lista_interface.h"

typedef struct lista_gen_ LISTA_GEN;

/* Inserir átomo retém; destruir a lista libera os nós e suas referências. */
LISTA_GEN *lista_gen_criar(void);
bool lista_gen_inserir_atomo(LISTA_GEN *lista, ITEM *item);
bool lista_gen_inserir_sublista(LISTA_GEN *lista, LISTA_GEN *sublista);
bool lista_gen_percorrer(const LISTA_GEN *lista, LISTA_VISITANTE visitante,
                         void *contexto);
void lista_gen_apagar(LISTA_GEN **lista);
void lista_gen_imprimir(LISTA_GEN *lista);
ITEM *lista_gen_buscar(const LISTA_GEN *lista, int chave);
int lista_gen_profundidade_maxima(LISTA_GEN *lista);

#endif
