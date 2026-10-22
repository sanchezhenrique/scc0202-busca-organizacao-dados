#include "estruturas.h"
#include "../consultas/consulta.h"

#include <string.h>

typedef struct folha {
  ITEM *item;
  struct folha *proxima;
} FOLHA;

typedef struct grupo {
  char *chave;
  struct grupo *proximo;
  struct grupo *subgrupos;
  FOLHA *inicio;
  FOLHA *fim;
} GRUPO;

struct lista_gen {
  GRUPO *grupos;
  GRUPO *grupo_atual;
  GRUPO *subgrupo_atual;
  FOLHA *atual;
  const struct consulta *consulta;
};

static GRUPO *encontrar_grupo(GRUPO *inicio, const char *chave) {
  while (inicio != NULL && strcmp(inicio->chave, chave) != 0) {
    inicio = inicio->proximo;
  }
  return inicio;
}

static GRUPO *criar_grupo(const char *chave) {
  GRUPO *grupo = calloc(1, sizeof(GRUPO));

  if (grupo != NULL) {
    grupo->chave = texto_copiar(chave);
    if (grupo->chave == NULL) {
      free(grupo);
      grupo = NULL;
    }
  }
  return grupo;
}

static void anexar_grupo(GRUPO **inicio, GRUPO *grupo) {
  GRUPO *ultimo = *inicio;

  if (ultimo == NULL) {
    *inicio = grupo;
  } else {
    while (ultimo->proximo != NULL) {
      ultimo = ultimo->proximo;
    }
    ultimo->proximo = grupo;
  }
}

LISTA_GEN *lista_gen_criar(const struct consulta *consulta) {
  LISTA_GEN *lista = NULL;

  if (consulta_estrutura(consulta, COLECAO_LISTA_GEN)) {
    lista = calloc(1, sizeof(LISTA_GEN));
    if (lista != NULL) {
      lista->consulta = consulta;
    }
  }
  return lista;
}

bool lista_gen_inserir(LISTA_GEN *lista, ITEM *item) {
  const char *chave;
  GRUPO *grupo;
  GRUPO *subgrupo = NULL;
  FOLHA *folha;
  bool novo;

  if (lista == NULL || item == NULL) {
    return false;
  }
  chave = consulta_grupo(lista->consulta, item, 0);
  grupo = encontrar_grupo(lista->grupos, chave);
  novo = grupo == NULL;
  if (novo) {
    grupo = criar_grupo(chave);
  }
  folha = malloc(sizeof(FOLHA));
  if (grupo != NULL && folha != NULL) {
    chave = consulta_grupo(lista->consulta, item, 1);
    subgrupo = encontrar_grupo(grupo->subgrupos, chave);
    if (subgrupo == NULL) {
      subgrupo = criar_grupo(chave);
      if (subgrupo != NULL) {
        anexar_grupo(&grupo->subgrupos, subgrupo);
      }
    }
  }
  if (subgrupo == NULL) {
    free(folha);
    if (novo && grupo != NULL) {
      free(grupo->chave);
      free(grupo);
    }
    return false;
  }
  /* A hierarquia so recebe o novo grupo depois de concluir as alocacoes. */
  folha->item = item;
  folha->proxima = NULL;
  if (subgrupo->fim == NULL) {
    subgrupo->inicio = folha;
  } else {
    subgrupo->fim->proxima = folha;
  }
  subgrupo->fim = folha;
  if (novo) {
    anexar_grupo(&lista->grupos, grupo);
  }
  lista_gen_reiniciar(lista);
  return true;
}

ITEM *lista_gen_retirar(LISTA_GEN *lista) {
  GRUPO *grupo;
  GRUPO *subgrupo;
  FOLHA *folha;
  ITEM *item;

  if (lista == NULL || lista->grupos == NULL) {
    return NULL;
  }
  grupo = lista->grupos;
  subgrupo = grupo->subgrupos;
  folha = subgrupo->inicio;
  item = folha->item;
  subgrupo->inicio = folha->proxima;
  free(folha);
  if (subgrupo->inicio == NULL) {
    grupo->subgrupos = subgrupo->proximo;
    free(subgrupo->chave);
    free(subgrupo);
    if (grupo->subgrupos == NULL) {
      lista->grupos = grupo->proximo;
      free(grupo->chave);
      free(grupo);
    }
  }
  lista_gen_reiniciar(lista);
  return item;
}

void lista_gen_reiniciar(LISTA_GEN *lista) {
  if (lista != NULL) {
    lista->grupo_atual = lista->grupos;
    lista->subgrupo_atual = lista->grupos != NULL ? lista->grupos->subgrupos : NULL;
    lista->atual = lista->subgrupo_atual != NULL ? lista->subgrupo_atual->inicio : NULL;
  }
}

ITEM *lista_gen_proximo(LISTA_GEN *lista) {
  ITEM *item;

  if (lista == NULL || lista->atual == NULL) {
    return NULL;
  }
  item = lista->atual->item;
  lista->atual = lista->atual->proxima;
  if (lista->atual == NULL) {
    lista->subgrupo_atual = lista->subgrupo_atual->proximo;
    if (lista->subgrupo_atual == NULL) {
      lista->grupo_atual = lista->grupo_atual->proximo;
      if (lista->grupo_atual != NULL) {
        lista->subgrupo_atual = lista->grupo_atual->subgrupos;
      }
    }
    if (lista->subgrupo_atual != NULL) {
      lista->atual = lista->subgrupo_atual->inicio;
    }
  }
  return item;
}

void lista_gen_apagar(LISTA_GEN **lista, bool liberar_itens) {
  ITEM *item;

  if (lista != NULL && *lista != NULL) {
    while ((item = lista_gen_retirar(*lista)) != NULL) {
      if (liberar_itens) {
        item_apagar(&item);
      }
    }
    free(*lista);
    *lista = NULL;
  }
}
