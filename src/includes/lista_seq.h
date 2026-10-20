#ifndef LISTA_SEQ_H
#define LISTA_SEQ_H

#include <stdbool.h>
#include "item.h"
#include "lista_interface.h"

#ifndef TAM_MAX
#define TAM_MAX 100000
#endif

typedef struct lista_seq_ LISTA_SEQ;

/* Inserir retém; destruir a lista libera as referencias mantidas por seus nós. */
LISTA_SEQ *lista_seq_criar(bool ordenada);
void lista_seq_apagar(LISTA_SEQ **lista);
bool lista_seq_cheia(const LISTA_SEQ *lista);
bool lista_seq_inserir(LISTA_SEQ *lista, ITEM *item);
bool lista_seq_percorrer(const LISTA_SEQ *lista, LISTA_VISITANTE visitante,
                         void *contexto);
int lista_seq_buscar_indice(const LISTA_SEQ *lista, int chave);
ITEM *lista_seq_buscar(const LISTA_SEQ *lista, int chave);
int lista_seq_tamanho(const LISTA_SEQ *lista);

#endif
