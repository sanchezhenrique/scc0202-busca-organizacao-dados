#include "estruturas.h"

typedef struct no {
  ITEM *item;
  struct no *proximo;
} NO;

struct fila {
  NO *inicio;
  NO *atual;
  NO *fim;
};

FILA *fila_criar(void) {
  FILA *estrutura = calloc(1, sizeof(FILA));
  return estrutura;
}

bool fila_inserir(FILA *estrutura, ITEM *item) {
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
  if (estrutura->fim == NULL) {
    estrutura->inicio = novo;
  } else {
    estrutura->fim->proximo = novo;
  }
  estrutura->fim = novo;
  fila_reiniciar(estrutura);
  return true;
}

ITEM *fila_retirar(FILA *estrutura) {
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
  if (estrutura->fim == removido) {
    estrutura->fim = NULL;
  }
  free(removido);
  fila_reiniciar(estrutura);
  return item;
}

void fila_reiniciar(FILA *estrutura) {
  if (estrutura != NULL) {
    estrutura->atual = estrutura->inicio;
  }
}

ITEM *fila_proximo(FILA *estrutura) {
  ITEM *item = NULL;

  if (estrutura != NULL && estrutura->atual != NULL) {
    item = estrutura->atual->item;
    estrutura->atual = estrutura->atual->proximo;
  }
  return item;
}

void fila_apagar(FILA **estrutura, bool liberar_itens) {
  ITEM *item;

  if (estrutura != NULL && *estrutura != NULL) {
    while ((item = fila_retirar(*estrutura)) != NULL) {
      if (liberar_itens) {
        item_apagar(&item);
      }
    }
    free(*estrutura);
    *estrutura = NULL;
  }
}
