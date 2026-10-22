#include "estruturas.h"

typedef struct no {
  ITEM *item;
  struct no *proximo;
} NO;

struct lista_cabeca {
  NO *inicio;
  NO *atual;
  NO *fim;
};

LISTA_CABECA *lista_cabeca_criar(void) {
  LISTA_CABECA *estrutura = calloc(1, sizeof(LISTA_CABECA));

  if (estrutura != NULL) {
    /* A sentinela existe mesmo sem itens e nunca e entregue ao cliente. */
    estrutura->inicio = calloc(1, sizeof(NO));
    if (estrutura->inicio == NULL) {
      free(estrutura);
      estrutura = NULL;
    } else {
      estrutura->fim = estrutura->inicio;
    }
  }
  return estrutura;
}

bool lista_cabeca_inserir(LISTA_CABECA *estrutura, ITEM *item) {
  NO *novo;

  if (estrutura == NULL || item == NULL) {
    return false;
  }
  novo = malloc(sizeof(NO));
  if (novo == NULL) {
    return false;
  }
  novo->item = item;
  novo->proximo = NULL;
  estrutura->fim->proximo = novo;
  estrutura->fim = novo;
  lista_cabeca_reiniciar(estrutura);
  return true;
}

ITEM *lista_cabeca_retirar(LISTA_CABECA *estrutura) {
  NO *removido;
  ITEM *item;

  if (estrutura == NULL) {
    return NULL;
  }
  removido = estrutura->inicio->proximo;
  if (removido == NULL) {
    return NULL;
  }
  item = removido->item;
  estrutura->inicio->proximo = removido->proximo;
  if (estrutura->fim == removido) {
    estrutura->fim = estrutura->inicio;
  }
  free(removido);
  lista_cabeca_reiniciar(estrutura);
  return item;
}

void lista_cabeca_reiniciar(LISTA_CABECA *estrutura) {
  if (estrutura != NULL) {
    estrutura->atual = estrutura->inicio->proximo;
  }
}

ITEM *lista_cabeca_proximo(LISTA_CABECA *estrutura) {
  ITEM *item = NULL;

  if (estrutura != NULL && estrutura->atual != NULL) {
    item = estrutura->atual->item;
    estrutura->atual = estrutura->atual->proximo;
  }
  return item;
}

void lista_cabeca_apagar(LISTA_CABECA **estrutura, bool liberar_itens) {
  ITEM *item;

  if (estrutura != NULL && *estrutura != NULL) {
    while ((item = lista_cabeca_retirar(*estrutura)) != NULL) {
      if (liberar_itens) {
        item_apagar(&item);
      }
    }
    free((*estrutura)->inicio);
    free(*estrutura);
    *estrutura = NULL;
  }
}
