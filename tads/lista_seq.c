#include "estruturas.h"

#include <string.h>

struct lista_seq {
  ITEM **itens;
  size_t tamanho;
  size_t capacidade;
  size_t atual;
};

LISTA_SEQ *lista_seq_criar(void) {
  return calloc(1, sizeof(LISTA_SEQ));
}

bool lista_seq_inserir(LISTA_SEQ *lista, ITEM *item) {
  ITEM **novo;
  size_t capacidade;

  if (lista == NULL || item == NULL) {
    return false;
  }
  if (lista->tamanho == lista->capacidade) {
    if (lista->capacidade > (size_t)-1 / sizeof(ITEM *) / 2) {
      return false;
    }
    capacidade = lista->capacidade == 0 ? 16 : lista->capacidade * 2;
    novo = realloc(lista->itens, capacidade * sizeof(ITEM *));
    if (novo == NULL) {
      return false;
    }
    lista->itens = novo;
    lista->capacidade = capacidade;
  }
  lista->itens[lista->tamanho] = item;
  lista->tamanho++;
  lista->atual = 0;
  return true;
}

ITEM *lista_seq_retirar(LISTA_SEQ *lista) {
  ITEM *item;

  if (lista == NULL || lista->tamanho == 0) {
    return NULL;
  }
  item = lista->itens[0];
  lista->tamanho--;
  memmove(lista->itens, lista->itens + 1,
          lista->tamanho * sizeof(ITEM *));
  lista->atual = 0;
  return item;
}

void lista_seq_reiniciar(LISTA_SEQ *lista) {
  if (lista != NULL) {
    lista->atual = 0;
  }
}

ITEM *lista_seq_proximo(LISTA_SEQ *lista) {
  if (lista == NULL || lista->atual == lista->tamanho) {
    return NULL;
  }
  return lista->itens[lista->atual++];
}

void lista_seq_apagar(LISTA_SEQ **lista, bool liberar_itens) {
  size_t i;

  if (lista != NULL && *lista != NULL) {
    for (i = 0; liberar_itens && i < (*lista)->tamanho; i++) {
      item_apagar(&(*lista)->itens[i]);
    }
    free((*lista)->itens);
    free(*lista);
    *lista = NULL;
  }
}
