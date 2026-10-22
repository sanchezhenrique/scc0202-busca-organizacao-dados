#include "item.h"

#include <math.h>
#include <string.h>

/* A linha original preserva o CSV; os campos separados permitem consultas. */
struct item {
  char *original;       /* Texto completo para exportacao, sem normalizacoes. */
  char **campos;       /* Valores separados e preparados pelo leitor. */
  size_t quantidade;  /* Numero de campos efetivamente presentes nesta linha. */
  size_t posicao;     /* Ordem de leitura, contada a partir de zero. */
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

/* O leitor entrega as strings prontas; o item passa a cuidar de sua memoria. */
ITEM *item_criar(char *original, char **campos, size_t quantidade,
                  size_t posicao) {
  ITEM *item;
  size_t i;

  if (original == NULL || campos == NULL || quantidade == 0) {
    return NULL;
  }
  for (i = 0; i < quantidade; i++) {
    if (campos[i] == NULL) {
      return NULL;
    }
  }
  item = malloc(sizeof(ITEM));
  /* Apenas armazena os ponteiros: os dados ja foram alocados pelo leitor. */
  if (item != NULL) {
    item->original = original;
    item->campos = campos;
    item->quantidade = quantidade;
    item->posicao = posicao;
  }
  return item;
}

/* Destruir duas vezes pela mesma variavel e seguro porque ela fica nula. */
void item_apagar(ITEM **item) {
  if (item == NULL || *item == NULL) {
    return;
  }
  campos_apagar(&(*item)->campos, (*item)->quantidade);
  free((*item)->original);
  free(*item);
  *item = NULL;
}

/* Campos finais omitidos sao tratados como vazios pelas consultas. */
const char *item_campo(const ITEM *item, size_t indice) {
  if (item == NULL) {
    return NULL;
  }
  return indice < item->quantidade ? item->campos[indice] : "";
}

/* Empresta o texto preservado; quem consulta nao deve modifica-lo ou libera-lo. */
const char *item_original(const ITEM *item) {
  return item != NULL ? item->original : NULL;
}

/* A lista ordenada usa esta posicao quando a consulta nao define ORDER. */
size_t item_posicao(const ITEM *item) {
  return item != NULL ? item->posicao : 0;
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

/* Converte sob demanda, mantendo os valores dos campos armazenados como texto. */
bool item_numero(const ITEM *item, size_t indice, double *numero) {
  return texto_numero(item_campo(item, indice), numero);
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
