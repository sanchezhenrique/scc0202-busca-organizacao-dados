#include "estruturas.h"

typedef struct no {
  REGISTRO *registro;
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

bool pilha_inserir(PILHA *estrutura, REGISTRO *registro) {
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
  /* Inserir no inicio faz o ultimo registro lido ser o primeiro acessado. */
  novo->proximo = estrutura->inicio;
  estrutura->inicio = novo;
  pilha_reiniciar(estrutura);
  return true;
}

REGISTRO *pilha_retirar(PILHA *estrutura) {
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
  free(removido);
  pilha_reiniciar(estrutura);
  return registro;
}

void pilha_reiniciar(PILHA *estrutura) {
  if (estrutura != NULL) {
    estrutura->atual = estrutura->inicio;
  }
}

REGISTRO *pilha_proximo(PILHA *estrutura) {
  REGISTRO *registro = NULL;

  if (estrutura != NULL && estrutura->atual != NULL) {
    registro = estrutura->atual->registro;
    estrutura->atual = estrutura->atual->proximo;
  }
  return registro;
}

void pilha_apagar(PILHA **estrutura, bool liberar_registros) {
  REGISTRO *registro;

  if (estrutura != NULL && *estrutura != NULL) {
    while ((registro = pilha_retirar(*estrutura)) != NULL) {
      if (liberar_registros) {
        registro_apagar(&registro);
      }
    }
    free(*estrutura);
    *estrutura = NULL;
  }
}
