#include "tabela.h"
#include "item.h"

#include <string.h>

/* O cabecalho e compartilhado por todos os itens e nao e uma linha de dados. */
struct tabela {
  char *cabecalho;
  char **campos;
  size_t quantidade;
  char delimitador;
};

TABELA *tabela_criar(char *cabecalho, char **campos, size_t quantidade,
                    char delimitador) {
  TABELA *tabela;
  size_t i;
  size_t j;

  if (cabecalho == NULL || campos == NULL || quantidade == 0 || delimitador == '\0') {
    return NULL;
  }
  /* Nomes vazios ou repetidos tornariam ambigua a resolucao de campos da consulta. */
  for (i = 0; i < quantidade; i++) {
    if (campos[i] == NULL || campos[i][0] == '\0') {
      return NULL;
    }
    for (j = 0; j < i; j++) {
      if (strcmp(campos[i], campos[j]) == 0) {
        return NULL;
      }
    }
  }
  tabela = malloc(sizeof(TABELA));
  if (tabela != NULL) {
    tabela->cabecalho = cabecalho;
    tabela->campos = campos;
    tabela->quantidade = quantidade;
    tabela->delimitador = delimitador;
  }
  return tabela;
}

void tabela_apagar(TABELA **tabela) {
  if (tabela == NULL || *tabela == NULL) {
    return;
  }
  campos_apagar(&(*tabela)->campos, (*tabela)->quantidade);
  free((*tabela)->cabecalho);
  free(*tabela);
  *tabela = NULL;
}

size_t tabela_colunas(const TABELA *tabela) {
  return tabela != NULL ? tabela->quantidade : 0;
}

bool tabela_indice(const TABELA *tabela, const char *nome, size_t *indice) {
  size_t i;

  if (tabela == NULL || nome == NULL || indice == NULL) {
    return false;
  }
  for (i = 0; i < tabela->quantidade; i++) {
    if (strcmp(tabela->campos[i], nome) == 0) {
      *indice = i;
      return true;
    }
  }
  return false;
}

const char *tabela_campo(const TABELA *tabela, size_t indice) {
  if (tabela == NULL || indice >= tabela->quantidade) {
    return NULL;
  }
  return tabela->campos[indice];
}

const char *tabela_cabecalho(const TABELA *tabela) {
  return tabela != NULL ? tabela->cabecalho : NULL;
}

char tabela_delimitador(const TABELA *tabela) {
  return tabela != NULL ? tabela->delimitador : '\0';
}
