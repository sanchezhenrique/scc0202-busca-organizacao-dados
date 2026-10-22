#include "estruturas.h"

#include <string.h>

struct lista_seq {
  REGISTRO **registros;
  size_t tamanho;
  size_t capacidade;
  size_t atual;
};

LISTA_SEQ *lista_seq_criar(void) {
  return calloc(1, sizeof(LISTA_SEQ));
}

bool lista_seq_inserir(LISTA_SEQ *lista, REGISTRO *registro) {
  REGISTRO **novo;
  size_t capacidade;

  if (lista == NULL || registro == NULL) {
    return false;
  }
  if (lista->tamanho == lista->capacidade) {
    if (lista->capacidade > (size_t)-1 / sizeof(REGISTRO *) / 2) {
      return false;
    }
    capacidade = lista->capacidade == 0 ? 16 : lista->capacidade * 2;
    novo = realloc(lista->registros, capacidade * sizeof(REGISTRO *));
    if (novo == NULL) {
      return false;
    }
    lista->registros = novo;
    lista->capacidade = capacidade;
  }
  lista->registros[lista->tamanho] = registro;
  lista->tamanho++;
  lista->atual = 0;
  return true;
}

REGISTRO *lista_seq_retirar(LISTA_SEQ *lista) {
  REGISTRO *registro;

  if (lista == NULL || lista->tamanho == 0) {
    return NULL;
  }
  registro = lista->registros[0];
  lista->tamanho--;
  memmove(lista->registros, lista->registros + 1,
          lista->tamanho * sizeof(REGISTRO *));
  lista->atual = 0;
  return registro;
}

void lista_seq_reiniciar(LISTA_SEQ *lista) {
  if (lista != NULL) {
    lista->atual = 0;
  }
}

REGISTRO *lista_seq_proximo(LISTA_SEQ *lista) {
  if (lista == NULL || lista->atual == lista->tamanho) {
    return NULL;
  }
  return lista->registros[lista->atual++];
}

void lista_seq_apagar(LISTA_SEQ **lista, bool liberar_registros) {
  size_t i;

  if (lista != NULL && *lista != NULL) {
    for (i = 0; liberar_registros && i < (*lista)->tamanho; i++) {
      registro_apagar(&(*lista)->registros[i]);
    }
    free((*lista)->registros);
    free(*lista);
    *lista = NULL;
  }
}
