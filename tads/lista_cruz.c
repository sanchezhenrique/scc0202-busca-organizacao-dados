#include "estruturas.h"
#include "../consultas/consulta.h"

typedef struct no {
  ITEM *item;
  struct no *proximo[2];
  struct no *anterior[2];
} NO;

struct lista_cruz {
  NO *inicio[2];
  NO *atual;
  unsigned int eixo;
  const struct consulta *consulta;
};

LISTA_CRUZ *lista_cruz_criar(const struct consulta *consulta) {
  LISTA_CRUZ *lista = NULL;

  if (consulta_estrutura(consulta, COLECAO_LISTA_CRUZ)) {
    lista = calloc(1, sizeof(LISTA_CRUZ));
    if (lista != NULL) {
      lista->consulta = consulta;
    }
  }
  return lista;
}

bool lista_cruz_inserir(LISTA_CRUZ *lista, ITEM *item) {
  NO *novo;
  NO *anterior;
  NO *atual;
  unsigned int eixo;

  if (lista == NULL || item == NULL) {
    return false;
  }
  novo = malloc(sizeof(NO));
  if (novo == NULL) {
    return false;
  }
  /* O mesmo no participa de duas cadeias, com uma unica referencia ao item. */
  novo->item = item;
  for (eixo = 0; eixo < 2; eixo++) {
    anterior = NULL;
    atual = lista->inicio[eixo];
    while (atual != NULL &&
           consulta_cruzar(lista->consulta, atual->item, item, eixo) <= 0) {
      anterior = atual;
      atual = atual->proximo[eixo];
    }
    novo->anterior[eixo] = anterior;
    novo->proximo[eixo] = atual;
    if (anterior == NULL) {
      lista->inicio[eixo] = novo;
    } else {
      anterior->proximo[eixo] = novo;
    }
    if (atual != NULL) {
      atual->anterior[eixo] = novo;
    }
  }
  lista_cruz_reiniciar(lista, 0);
  return true;
}

ITEM *lista_cruz_retirar(LISTA_CRUZ *lista) {
  NO *removido;
  ITEM *item;
  unsigned int eixo;

  if (lista == NULL || lista->inicio[0] == NULL) {
    return NULL;
  }
  removido = lista->inicio[0];
  item = removido->item;
  /* Desliga o no dos dois eixos antes de liberar sua unica alocacao. */
  for (eixo = 0; eixo < 2; eixo++) {
    if (removido->anterior[eixo] == NULL) {
      lista->inicio[eixo] = removido->proximo[eixo];
    } else {
      removido->anterior[eixo]->proximo[eixo] = removido->proximo[eixo];
    }
    if (removido->proximo[eixo] != NULL) {
      removido->proximo[eixo]->anterior[eixo] = removido->anterior[eixo];
    }
  }
  free(removido);
  lista_cruz_reiniciar(lista, 0);
  return item;
}

void lista_cruz_reiniciar(LISTA_CRUZ *lista, unsigned int eixo) {
  if (lista != NULL && eixo < 2) {
    lista->eixo = eixo;
    lista->atual = lista->inicio[eixo];
  }
}

ITEM *lista_cruz_proximo(LISTA_CRUZ *lista) {
  ITEM *item = NULL;

  if (lista != NULL && lista->atual != NULL) {
    item = lista->atual->item;
    lista->atual = lista->atual->proximo[lista->eixo];
  }
  return item;
}

void lista_cruz_apagar(LISTA_CRUZ **lista, bool liberar_itens) {
  ITEM *item;

  if (lista != NULL && *lista != NULL) {
    while ((item = lista_cruz_retirar(*lista)) != NULL) {
      if (liberar_itens) {
        item_apagar(&item);
      }
    }
    free(*lista);
    *lista = NULL;
  }
}
