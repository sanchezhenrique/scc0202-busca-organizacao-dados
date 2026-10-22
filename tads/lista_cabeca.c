#include "estruturas.h"

typedef struct no {
  REGISTRO *registro;
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
    /* A sentinela existe mesmo sem registros e nunca e entregue ao cliente. */
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

bool lista_cabeca_inserir(LISTA_CABECA *estrutura, REGISTRO *registro) {
  NO *novo;

  if (estrutura == NULL || registro == NULL) {
    return false;
  }
  novo = malloc(sizeof(NO));
  if (novo == NULL) {
    return false;
  }
  novo->registro = registro;
  novo->proximo = NULL;
  estrutura->fim->proximo = novo;
  estrutura->fim = novo;
  lista_cabeca_reiniciar(estrutura);
  return true;
}

REGISTRO *lista_cabeca_retirar(LISTA_CABECA *estrutura) {
  NO *removido;
  REGISTRO *registro;

  if (estrutura == NULL) {
    return NULL;
  }
  removido = estrutura->inicio->proximo;
  if (removido == NULL) {
    return NULL;
  }
  registro = removido->registro;
  estrutura->inicio->proximo = removido->proximo;
  if (estrutura->fim == removido) {
    estrutura->fim = estrutura->inicio;
  }
  free(removido);
  lista_cabeca_reiniciar(estrutura);
  return registro;
}

void lista_cabeca_reiniciar(LISTA_CABECA *estrutura) {
  if (estrutura != NULL) {
    estrutura->atual = estrutura->inicio->proximo;
  }
}

REGISTRO *lista_cabeca_proximo(LISTA_CABECA *estrutura) {
  REGISTRO *registro = NULL;

  if (estrutura != NULL && estrutura->atual != NULL) {
    registro = estrutura->atual->registro;
    estrutura->atual = estrutura->atual->proximo;
  }
  return registro;
}

void lista_cabeca_apagar(LISTA_CABECA **estrutura, bool liberar_registros) {
  REGISTRO *registro;

  if (estrutura != NULL && *estrutura != NULL) {
    while ((registro = lista_cabeca_retirar(*estrutura)) != NULL) {
      if (liberar_registros) {
        registro_apagar(&registro);
      }
    }
    free((*estrutura)->inicio);
    free(*estrutura);
    *estrutura = NULL;
  }
}
