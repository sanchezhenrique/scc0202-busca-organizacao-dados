#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../includes/lista_gen.h"

typedef struct no_ NO;

/* Cada nivel tem uma cadeia propria; nos podem guardar atomos ou sublistas. */
struct lista_gen_{
  NO *inicio;
  NO *fim;
};

struct no_{
  int tipo; /* 0 = atomo ITEM; 1 = inicio de um nivel de sublista. */
  union{
    ITEM *atomo;
    struct no_ *sublista;
  } info;
  NO *prox; /* Proximo no do mesmo nivel (nao da sublista). */
};

LISTA_GEN* lista_gen_criar(void){
  LISTA_GEN *lista = malloc(sizeof(LISTA_GEN));
  if(lista == NULL) return NULL;

  lista->inicio = NULL;
  lista->fim = NULL;

  return lista;
}

/* Cria no estrutural; o campo da uniao e preenchido pelo chamador. */
static NO *no_criar(int tipo){
  NO *p = malloc(sizeof(NO));
  if(p != NULL){
    p->tipo = tipo;
    p->prox = NULL;
  }
  return p;
}

/* Adiciona um registro como atomo no fim do nivel atual. */
bool lista_gen_inserir_atomo(LISTA_GEN *l, ITEM *item){
  if(l == NULL || item == NULL) return false;
  NO *novo = no_criar(0);
  if(novo == NULL) return false;
  if(!item_reter(item)){
    free(novo);
    return false;
  }
  novo->info.atomo = item;

  if(l->inicio == NULL) l->inicio = novo;
  else l->fim->prox = novo;

  l->fim = novo;

  return true;
}

/* Move os nos de sublista para dentro da lista e libera apenas seu invólucro. */
bool lista_gen_inserir_sublista(LISTA_GEN *l, LISTA_GEN *sublista){
  /* A lista nao pode ser inserida dentro de si mesma: isso criaria ciclo. */
  if(l == NULL || sublista == NULL || l == sublista) return false;

  NO *novo = no_criar(1);
  if(novo == NULL) return false;

  /* O no marcador guarda o inicio do novo nivel hierarquico. */
  novo->info.sublista = sublista->inicio;

  if(l->inicio == NULL) l->inicio = novo;
  else l->fim->prox = novo;

  l->fim = novo;

  /* A lista nao deve ser usada pelo chamador apos transferir seus nos. */
  free(sublista);

  return true;
}

/* Libera recursivamente os nos e cada referencia que eles mantêm. */
static void no_apagar(NO **p){
  if(p == NULL || *p == NULL) return;

  if((*p)->tipo == 0) item_apagar(&((*p)->info.atomo));
  else no_apagar(&((*p)->info.sublista));

  no_apagar(&((*p)->prox));
  free(*p);
  *p = NULL;
}

void lista_gen_apagar(LISTA_GEN **l){
  if(l == NULL || *l == NULL) return;
  no_apagar(&((*l)->inicio));
  free(*l);
  *l = NULL;
}

static void no_imprimir(NO *p){
  if(p == NULL) return;

  if(p->tipo == 0){
    item_imprimir(p->info.atomo);
  } else{ // sub-lista
    printf("(");
    no_imprimir(p->info.sublista);
    printf(")");
  }

  /* Continua a impressao dos demais elementos deste nivel. */
  if(p->prox != NULL){
    printf(",");
    no_imprimir(p->prox);
  }
}

void lista_gen_imprimir (LISTA_GEN *l){
  if(l == NULL) return;
  printf("(");
  no_imprimir(l->inicio);
  printf(")\n");
}

static bool no_percorrer(NO *p, LISTA_VISITANTE visitante, void *contexto){
  while(p != NULL){
    if(p->tipo == 0){
      if(!visitante(p->info.atomo, contexto)) return false;
    } else if(!no_percorrer(p->info.sublista, visitante, contexto)){
      return false;
    }
    p = p->prox;
  }
  return true;
}

bool lista_gen_percorrer(const LISTA_GEN *l, LISTA_VISITANTE visitante,
                         void *contexto){
  if(l == NULL || visitante == NULL) return false;
  return no_percorrer(l->inicio, visitante, contexto);
}

static NO *no_buscar_atomo(NO *p, int chave){
  if(p == NULL) return NULL;

  if(p->tipo == 0){
    if(item_get_chave(p->info.atomo) == chave) return p; 
    } else{
    /* Se for sublista, percorre primeiro o nivel interno. */
    NO *encontrado = no_buscar_atomo(p->info.sublista, chave);
    if(encontrado != NULL) return encontrado;
  }

  /* Se nao encontrou neste ramo, continua pelos irmaos do mesmo nivel. */
  return no_buscar_atomo(p->prox, chave);
}

ITEM *lista_gen_buscar(const LISTA_GEN *l, int chave){
  if(l == NULL) return NULL;

  NO *p = no_buscar_atomo(l->inicio, chave);

  if(p == NULL) return NULL;

  return p->info.atomo;
}

static int no_profundidade(NO *p){
  if(p == NULL) return 0;

  /* Profundidade do ramo atual, comparada com a dos irmaos restantes. */
  int prof_no = 0;
  if(p->tipo == 0){
    prof_no = 1;
  } else{
    prof_no = 1 + no_profundidade(p->info.sublista);
  }

  int prof_restante = no_profundidade(p->prox);

  return (prof_no > prof_restante) ? prof_no : prof_restante;
}

int lista_gen_profundidade_maxima(LISTA_GEN *l){
  if(l == NULL || l->inicio == NULL) return 0;
  return no_profundidade(l->inicio);
}
