#include "estruturas.h"

typedef struct no {
  ITEM *item;
  struct no *proximo;
} NO;

struct pilha {
  NO *inicio;
  NO *atual;
};

PILHA *pilha_criar(void) {
  PILHA *estrutura = calloc(1, sizeof(PILHA));
  return estrutura;
}

bool pilha_inserir(PILHA *estrutura, ITEM *item) {
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
  /* Inserir no inicio faz o ultimo item lido ser o primeiro acessado. */
  novo->proximo = estrutura->inicio;
  estrutura->inicio = novo;
  pilha_reiniciar(estrutura);
  return true;
}

ITEM *pilha_retirar(PILHA *estrutura) {
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
  pilha_reiniciar(estrutura);
  return item;
}

void pilha_reiniciar(PILHA *estrutura) {
  if (estrutura != NULL) {
    estrutura->atual = estrutura->inicio;
  }
}

ITEM *pilha_proximo(PILHA *estrutura) {
  ITEM *item = NULL;

  if (estrutura != NULL && estrutura->atual != NULL) {
    item = estrutura->atual->item;
    estrutura->atual = estrutura->atual->proximo;
  }
  return item;
}

void pilha_apagar(PILHA **estrutura, bool liberar_itens) {
  ITEM *item;

  if (estrutura != NULL && *estrutura != NULL) {
    while ((item = pilha_retirar(*estrutura)) != NULL) {
      if (liberar_itens) {
        item_apagar(&item);
      }
    }
    free(*estrutura);
    *estrutura = NULL;
  }
}
