#include "estruturas.h"
#include "../consultas/consulta.h"

typedef struct no {
  ITEM *item;
  struct no *proximo;
} NO;

struct lista_ord {
  NO *inicio;
  NO *atual;
  const struct consulta *consulta;
};

LISTA_ORD *lista_ord_criar(const struct consulta *consulta) {
  LISTA_ORD *estrutura = calloc(1, sizeof(LISTA_ORD));

  if (estrutura != NULL) {
    estrutura->consulta = consulta;
  }
  return estrutura;
}

bool lista_ord_inserir(LISTA_ORD *estrutura, ITEM *item) {
  NO *novo;
  NO *anterior = NULL;
  NO *atual;

  if (estrutura == NULL || item == NULL) {
    return false;
  }
  novo = malloc(sizeof(NO));
  if (novo == NULL) {
    return false;
  }
  novo->item = item;
  novo->proximo = NULL;
  /* A lista ja fica ordenada nesta insercao; empates entram depois. */
  atual = estrutura->inicio;
  while (atual != NULL &&
         consulta_comparar(estrutura->consulta, atual->item, item) <= 0) {
    anterior = atual;
    atual = atual->proximo;
  }
  novo->proximo = atual;
  if (anterior == NULL) {
    estrutura->inicio = novo;
  } else {
    anterior->proximo = novo;
  }
  lista_ord_reiniciar(estrutura);
  return true;
}

ITEM *lista_ord_retirar(LISTA_ORD *estrutura) {
  NO *removido;
  ITEM *item;

  if (estrutura == NULL) {
    return NULL;
  }
  removido = estrutura->inicio;
  if (removido == NULL) {
    return NULL;
  }
  item = removido->item;
  estrutura->inicio = removido->proximo;
  free(removido);
  lista_ord_reiniciar(estrutura);
  return item;
}

void lista_ord_reiniciar(LISTA_ORD *estrutura) {
  if (estrutura != NULL) {
    estrutura->atual = estrutura->inicio;
  }
}

ITEM *lista_ord_proximo(LISTA_ORD *estrutura) {
  ITEM *item = NULL;

  if (estrutura != NULL && estrutura->atual != NULL) {
    item = estrutura->atual->item;
    estrutura->atual = estrutura->atual->proximo;
  }
  return item;
}

void lista_ord_apagar(LISTA_ORD **estrutura, bool liberar_itens) {
  ITEM *item;

  if (estrutura != NULL && *estrutura != NULL) {
    while ((item = lista_ord_retirar(*estrutura)) != NULL) {
      if (liberar_itens) {
        item_apagar(&item);
      }
    }
    free(*estrutura);
    *estrutura = NULL;
  }
}
