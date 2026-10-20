#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "fila.h"

/* No encadeado da fila; prox liga os itens em ordem de chegada. */
typedef struct _no{
  ITEM *item;
  struct _no *prox;
} NO;

struct fila {
  /* inicio e o proximo a sair; fim e o ultimo inserido. */
  NO *inicio;
  NO *fim;
  int tamanho;
};

FILA* fila_criar(){
  FILA *f = malloc(sizeof(FILA));
  if(f != NULL){
    f->inicio = NULL;
    f->fim = NULL;
    f->tamanho = 0;
  } 
  return f;
}

bool fila_inserir(FILA *fila, ITEM *item){
  if(fila == NULL || item == NULL) return false;

  NO *novo = malloc(sizeof(NO));
  if(novo == NULL) return false;
  if(!item_reter(item)){
    free(novo);
    return false;
  }

  novo->item = item;
  novo->prox = NULL;

  /* Na primeira insercao, inicio e fim apontam para o mesmo no. */
  if(fila_vazia(fila)){
    fila->inicio = novo;
    fila->fim = novo;
  } else{
    fila->fim->prox = novo;
    fila->fim = novo;
  }

  fila->tamanho++;
  return true;
}

ITEM* fila_remover(FILA *fila){
  if(fila == NULL || fila_vazia(fila)) return NULL;

  NO *removido = fila->inicio;
  ITEM *item = removido->item;

  /* Remover atualiza inicio e devolve o ITEM sem libera-lo. */
  fila->inicio = removido->prox;
  
  if(fila->inicio == NULL) fila->fim = NULL;
  

  free(removido);
  fila->tamanho--;
  return item;
}

void fila_apagar(FILA **fila){
  if(fila == NULL || *fila == NULL) return;

  /* Remover transfere cada referencia local; aqui a fila a libera. */
  while(!fila_vazia(*fila)){
    ITEM *item = fila_remover(*fila);
    item_apagar(&item);
  }

  free(*fila);
  *fila = NULL;
}


bool fila_vazia(FILA *fila){
  /* A ausencia de estrutura e tratada como fila vazia. */
  return fila == NULL || fila->tamanho == 0;
}

int fila_tamanho(FILA *fila){
  if(fila != NULL) return fila->tamanho;
  else return 0;
}

void fila_imprimir_status(FILA *fila){
  NO *atual;

  if(fila == NULL){
    printf("FILA INEXISTENTE!");
    return;
  }
  if(fila_vazia(fila)){
    printf("FILA VAZIA!");
    return;
  }

  printf("inicio->");
  atual = fila->inicio;

  while(atual != NULL){
    const char *mensagem = item_get_mensagem(atual->item);
    printf("%s\n", mensagem != NULL ? mensagem : "(sem mensagem)");
    atual = atual->prox;
  }
  printf("<-fim");
}
