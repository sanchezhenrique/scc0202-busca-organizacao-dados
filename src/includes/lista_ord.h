#ifndef LISTA_ORD_H
#define LISTA_ORD_H

#include <stdbool.h>
#include "item.h"
#include "lista_interface.h"

typedef struct lista_ord_ LISTA_ORD;

/* Ordena pelo valor numérico de um campo do ITEM, em ordem crescente ou
 * decrescente. O valor é capturado na inserção; alterações posteriores no
 * campo não reordenam o nó. Valores iguais preservam a ordem de inserção. */
LISTA_ORD *lista_ord_criar(const char *campo, bool decrescente);
void lista_ord_apagar(LISTA_ORD **lista);
bool lista_ord_inserir(LISTA_ORD *lista, ITEM *item);
ITEM *lista_ord_buscar(const LISTA_ORD *lista, int chave);
ITEM *lista_ord_remover(LISTA_ORD *lista, int chave);
bool lista_ord_percorrer(const LISTA_ORD *lista, LISTA_VISITANTE visitante,
                         void *contexto);
int lista_ord_tamanho(const LISTA_ORD *lista);
bool lista_ord_vazia(const LISTA_ORD *lista);

#endif
