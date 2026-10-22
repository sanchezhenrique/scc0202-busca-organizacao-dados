#include "estruturas.h"

typedef struct no {
  ITEM *item;
  struct no *proximo;
} NO;

struct lista_enc {
  NO *inicio;
  NO *atual;
};

LISTA_ENC *lista_enc_criar(void) {
  LISTA_ENC *estrutura = calloc(1, sizeof(LISTA_ENC));
  return estrutura;
}

bool lista_enc_inserir(LISTA_ENC *estrutura, ITEM *item) {
  NO *novo;
  NO *ultimo;

  if (estrutura == NULL || item == NULL) {
    return false;
  }
  novo = malloc(sizeof(NO));
  if (novo == NULL) {
    return false;
  }
  novo->item = item;
  novo->proximo = NULL;
  if (estrutura->inicio == NULL) {
    estrutura->inicio = novo;
  } else {
    ultimo = estrutura->inicio;
    while (ultimo->proximo != NULL) {
      ultimo = ultimo->proximo;
    }
    ultimo->proximo = novo;
  }
  lista_enc_reiniciar(estrutura);
  return true;
}

ITEM *lista_enc_retirar(LISTA_ENC *estrutura) {
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
  lista_enc_reiniciar(estrutura);
  return item;
}

void lista_enc_reiniciar(LISTA_ENC *estrutura) {
  if (estrutura != NULL) {
    estrutura->atual = estrutura->inicio;
  }
}

ITEM *lista_enc_proximo(LISTA_ENC *estrutura) {
  ITEM *item = NULL;

  if (estrutura != NULL && estrutura->atual != NULL) {
    item = estrutura->atual->item;
    estrutura->atual = estrutura->atual->proximo;
  }
  return item;
}

void lista_enc_apagar(LISTA_ENC **estrutura, bool liberar_itens) {
  ITEM *item;

  if (estrutura != NULL && *estrutura != NULL) {
    while ((item = lista_enc_retirar(*estrutura)) != NULL) {
      if (liberar_itens) {
        item_apagar(&item);
      }
    }
    free(*estrutura);
    *estrutura = NULL;
  }
}
