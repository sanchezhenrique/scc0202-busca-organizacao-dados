#include "estruturas.h"

typedef struct no {
  REGISTRO *registro;
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

bool fila_inserir(FILA *estrutura, REGISTRO *registro) {
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
  if (estrutura->fim == NULL) {
    estrutura->inicio = novo;
  } else {
    estrutura->fim->proximo = novo;
  }
  estrutura->fim = novo;
  fila_reiniciar(estrutura);
  return true;
}

REGISTRO *fila_retirar(FILA *estrutura) {
  NO *removido;
  REGISTRO *registro;

  if (estrutura == NULL) {
    return NULL;
  }
  removido = estrutura->inicio;
  if (removido == NULL) {
    return NULL;
  }
  registro = removido->registro;
  estrutura->inicio = removido->proximo;
  if (estrutura->fim == removido) {
    estrutura->fim = NULL;
  }
  free(removido);
  fila_reiniciar(estrutura);
  return registro;
}

void fila_reiniciar(FILA *estrutura) {
  if (estrutura != NULL) {
    estrutura->atual = estrutura->inicio;
  }
}

REGISTRO *fila_proximo(FILA *estrutura) {
  REGISTRO *registro = NULL;

  if (estrutura != NULL && estrutura->atual != NULL) {
    registro = estrutura->atual->registro;
    estrutura->atual = estrutura->atual->proximo;
  }
  return registro;
}

void fila_apagar(FILA **estrutura, bool liberar_registros) {
  REGISTRO *registro;

  if (estrutura != NULL && *estrutura != NULL) {
    while ((registro = fila_retirar(*estrutura)) != NULL) {
      if (liberar_registros) {
        registro_apagar(&registro);
      }
    }
    free(*estrutura);
    *estrutura = NULL;
  }
}
