#include <stdlib.h>
#include "../includes/lista_seq.h"

/* Vetor de capacidade fixa; fim e a posicao livre seguinte a ultima entrada. */
struct lista_seq_{
  ITEM *itens[TAM_MAX];
  int tamanho;
  int fim;
  bool ordenada;
};

LISTA_SEQ *lista_seq_criar(bool ordenada){
  LISTA_SEQ *nova = malloc(sizeof(LISTA_SEQ));
  if(nova != NULL){
    nova->tamanho = 0;
    nova->fim = 0;
    nova->ordenada = ordenada;
    for(int i = 0; i < TAM_MAX; i++){
      nova->itens[i] = NULL;
    }
  }

  return nova;
}

void lista_seq_apagar(LISTA_SEQ **lista){
  if(lista != NULL && *lista != NULL){
    /* Destruir a lista libera uma referencia por item armazenado. */
    for(int i = 0; i < (*lista)->fim; i++){
      item_apagar(&(*lista)->itens[i]);
    }
    free(*lista);
    *lista = NULL;
  }
}

bool lista_seq_cheia(const LISTA_SEQ *lista){
  /* A capacidade vem de TAM_MAX definida no header da lista. */
  return lista == NULL || lista->tamanho >= TAM_MAX;
}

bool lista_seq_inserir(LISTA_SEQ *lista, ITEM *item){
  if(lista == NULL || item == NULL || lista_seq_cheia(lista)) return false;
  if(!item_reter(item)) return false;

  int posicao = lista->fim;
  if(lista->ordenada){
    /* Insere apos chaves iguais e desloca a cauda para manter ordem estavel. */
    int chave = item_get_chave(item);
    posicao = 0;
    while(posicao < lista->fim &&
          item_get_chave(lista->itens[posicao]) <= chave)
      posicao++;
    for(int i = lista->fim; i > posicao; i--)
      lista->itens[i] = lista->itens[i - 1];
  }

  lista->itens[posicao] = item;
  lista->fim++;
  lista->tamanho++;
  return true;
}

bool lista_seq_percorrer(const LISTA_SEQ *lista, LISTA_VISITANTE visitante,
                         void *contexto){
  if(lista == NULL || visitante == NULL) return false;
  for(int i = 0; i < lista->fim; i++)
    if(!visitante(lista->itens[i], contexto)) return false;
  return true;
}

int lista_seq_buscar_indice(const LISTA_SEQ *lista, int chave){
  if(lista == NULL) return -1;
  /* Busca linear tambem funciona quando a lista foi criada sem ordenacao. */
  for(int i = 0; i < lista->fim; i++)
    if(lista->itens[i] != NULL && item_get_chave(lista->itens[i]) == chave)
      return i;
  return -1;
}

ITEM *lista_seq_buscar(const LISTA_SEQ *lista, int chave){
  int indice = lista_seq_buscar_indice(lista, chave);
  return indice >= 0 ? lista->itens[indice] : NULL;
}

int lista_seq_tamanho(const LISTA_SEQ *lista){
  return lista != NULL ? lista->tamanho : 0;
}
