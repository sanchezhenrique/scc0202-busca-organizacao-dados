#include "estruturas.h"
#include "../consultas/consulta.h"

typedef struct no {
  REGISTRO *registro;
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

bool lista_ord_inserir(LISTA_ORD *estrutura, REGISTRO *registro) {
  NO *novo;
  NO *anterior = NULL;
  NO *atual;

  if (estrutura == NULL || registro == NULL) {
    return false;
  }
  novo = malloc(sizeof(NO));
  if (novo == NULL) {
    return false;
  }
  novo->registro = registro;
  novo->proximo = NULL;
  /* A lista ja fica ordenada nesta insercao; empates entram depois. */
  atual = estrutura->inicio;
  while (atual != NULL &&
         consulta_comparar(estrutura->consulta, atual->registro, registro) <= 0) {
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

REGISTRO *lista_ord_retirar(LISTA_ORD *estrutura) {
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
  lista_ord_reiniciar(estrutura);
  return registro;
}

void lista_ord_reiniciar(LISTA_ORD *estrutura) {
  if (estrutura != NULL) {
    estrutura->atual = estrutura->inicio;
  }
}

REGISTRO *lista_ord_proximo(LISTA_ORD *estrutura) {
  REGISTRO *registro = NULL;

  if (estrutura != NULL && estrutura->atual != NULL) {
    registro = estrutura->atual->registro;
    estrutura->atual = estrutura->atual->proximo;
  }
  return registro;
}

void lista_ord_apagar(LISTA_ORD **estrutura, bool liberar_registros) {
  REGISTRO *registro;

  if (estrutura != NULL && *estrutura != NULL) {
    while ((registro = lista_ord_retirar(*estrutura)) != NULL) {
      if (liberar_registros) {
        registro_apagar(&registro);
      }
    }
    free(*estrutura);
    *estrutura = NULL;
  }
}
