#include "registro.h"

#include <math.h>
#include <string.h>

struct tabela {
  char *cabecalho;
  char **campos;
  size_t quantidade;
  char delimitador;
};

/* A linha original preserva o CSV; os campos separados permitem consultas. */
struct registro {
  char *original;
  char **campos;
  size_t quantidade;
  size_t posicao;
};

void campos_apagar(char ***campos, size_t quantidade) {
  size_t i;

  if (campos == NULL || *campos == NULL) {
    return;
  }
  for (i = 0; i < quantidade; i++) {
    free((*campos)[i]);
  }
  free(*campos);
  *campos = NULL;
}

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

REGISTRO *registro_criar(char *original, char **campos, size_t quantidade,
                        size_t posicao) {
  REGISTRO *registro;
  size_t i;

  if (original == NULL || campos == NULL || quantidade == 0) {
    return NULL;
  }
  for (i = 0; i < quantidade; i++) {
    if (campos[i] == NULL) {
      return NULL;
    }
  }
  registro = malloc(sizeof(REGISTRO));
  /* Apenas armazena os ponteiros: os dados ja foram alocados pelo leitor. */
  if (registro != NULL) {
    registro->original = original;
    registro->campos = campos;
    registro->quantidade = quantidade;
    registro->posicao = posicao;
  }
  return registro;
}

void registro_apagar(REGISTRO **registro) {
  if (registro == NULL || *registro == NULL) {
    return;
  }
  campos_apagar(&(*registro)->campos, (*registro)->quantidade);
  free((*registro)->original);
  free(*registro);
  *registro = NULL;
}

const char *registro_campo(const REGISTRO *registro, size_t indice) {
  if (registro == NULL) {
    return NULL;
  }
  return indice < registro->quantidade ? registro->campos[indice] : "";
}

const char *registro_original(const REGISTRO *registro) {
  return registro != NULL ? registro->original : NULL;
}

size_t registro_posicao(const REGISTRO *registro) {
  return registro != NULL ? registro->posicao : 0;
}

bool texto_numero(const char *texto, double *numero) {
  char *fim;
  double valor;

  if (texto == NULL || numero == NULL) {
    return false;
  }
  valor = strtod(texto, &fim);
  /* strtod pode aceitar apenas um prefixo; o restante precisa ser espaco ou fim. */
  if (fim == texto || !isfinite(valor)) {
    return false;
  }
  while (*fim == ' ' || *fim == '\t' || *fim == '\r' || *fim == '\n' ||
         *fim == '\f' || *fim == '\v') {
    fim++;
  }
  if (*fim != '\0') {
    return false;
  }
  *numero = valor;
  return true;
}

bool registro_numero(const REGISTRO *registro, size_t indice, double *numero) {
  return texto_numero(registro_campo(registro, indice), numero);
}

char *texto_copiar(const char *texto) {
  char *copia;

  if (texto == NULL) {
    return NULL;
  }
  copia = malloc(strlen(texto) + 1);
  if (copia != NULL) {
    strcpy(copia, texto);
  }
  return copia;
}

bool texto_booleano(const char *texto, double *valor) {
  char minusculo[6];
  size_t i;

  if (texto == NULL || valor == NULL || strlen(texto) > 5) {
    return false;
  }
  for (i = 0; texto[i] != '\0'; i++) {
    minusculo[i] = texto[i] >= 'A' && texto[i] <= 'Z' ? texto[i] - 'A' + 'a' : texto[i];
  }
  minusculo[i] = '\0';
  if (strcmp(minusculo, "true") == 0 || strcmp(minusculo, "false") == 0) {
    *valor = strcmp(minusculo, "true") == 0 ? 1 : 0;
    return true;
  }
  return false;
}

/* Classes fixas conservam uma ordem coerente mesmo em colunas de tipos mistos. */
static int texto_classe(const char *texto, double *numero) {
  if (texto == NULL || texto[0] == '\0') {
    *numero = 0;
    return 0;
  }
  if (texto_numero(texto, numero)) {
    return 1;
  }
  return texto_booleano(texto, numero) ? 2 : 3;
}

int texto_comparar(const char *a, const char *b) {
  double x;
  double y;
  int classe_a = texto_classe(a, &x);
  int classe_b = texto_classe(b, &y);
  int resultado;

  if (classe_a != classe_b) {
    return (classe_a > classe_b) - (classe_a < classe_b);
  }
  if (classe_a < 3) {
    return (x > y) - (x < y);
  }
  resultado = strcmp(a, b);
  return (resultado > 0) - (resultado < 0);
}
