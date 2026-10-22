#include "estruturas.h"

typedef struct no {
  REGISTRO *registro;
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

bool lista_enc_inserir(LISTA_ENC *estrutura, REGISTRO *registro) {
  NO *novo;
  NO *ultimo;

  if (estrutura == NULL || registro == NULL) {
    return false;
  }
  novo = malloc(sizeof(NO));
  if (novo == NULL) {
    return false;
  }
  novo->registro = registro;
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

REGISTRO *lista_enc_retirar(LISTA_ENC *estrutura) {
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
  lista_enc_reiniciar(estrutura);
  return registro;
}

void lista_enc_reiniciar(LISTA_ENC *estrutura) {
  if (estrutura != NULL) {
    estrutura->atual = estrutura->inicio;
  }
}

REGISTRO *lista_enc_proximo(LISTA_ENC *estrutura) {
  REGISTRO *registro = NULL;

  if (estrutura != NULL && estrutura->atual != NULL) {
    registro = estrutura->atual->registro;
    estrutura->atual = estrutura->atual->proximo;
  }
  return registro;
}

void lista_enc_apagar(LISTA_ENC **estrutura, bool liberar_registros) {
  REGISTRO *registro;

  if (estrutura != NULL && *estrutura != NULL) {
    while ((registro = lista_enc_retirar(*estrutura)) != NULL) {
      if (liberar_registros) {
        registro_apagar(&registro);
      }
    }
    free(*estrutura);
    *estrutura = NULL;
  }
}
