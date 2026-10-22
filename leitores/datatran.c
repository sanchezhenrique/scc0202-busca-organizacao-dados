#include "datatran.h"

#include <string.h>

/*
 * O arquivo fornecido usa Latin-1. Convertemos somente os campos consultaveis
 * para UTF-8, para que os acentos coincidam com os arquivos de consulta.
 */
static bool converter_texto(char **texto) {
  size_t tamanho = strlen(*texto);
  size_t i = 0;
  size_t destino = 0;
  unsigned char caractere;
  char *novo;

  /* Campos ASCII ja sao UTF-8: nao precisam de outra alocacao. */
  while (i < tamanho && (unsigned char)(*texto)[i] < 128) {
    i++;
  }
  if (i == tamanho) {
    return true;
  }
  if (tamanho > ((size_t)-1 - 1) / 2) {
    return false;
  }
  novo = malloc(tamanho * 2 + 1);
  if (novo == NULL) {
    return false;
  }
  for (i = 0; i < tamanho; i++) {
    caractere = (unsigned char)(*texto)[i];
    if (caractere < 128) {
      novo[destino++] = (char)caractere;
    } else {
      /* Um byte Latin-1 acima de 127 corresponde a dois bytes UTF-8. */
      novo[destino++] = (char)(192 + caractere / 64);
      novo[destino++] = (char)(128 + caractere % 64);
    }
  }
  novo[destino] = '\0';
  free(*texto);
  *texto = novo;
  return true;
}

bool datatran_validar(const TABELA *tabela) {
  const char *obrigatorios[] = {"id", "data_inversa", "uf", "km", "latitude", "longitude"};
  size_t indice;
  size_t i;

  for (i = 0; i < sizeof(obrigatorios) / sizeof(obrigatorios[0]); i++) {
    if (!tabela_indice(tabela, obrigatorios[i], &indice)) {
      return false;
    }
  }
  return true;
}

bool datatran_tratar(char **campos, size_t quantidade, const TABELA *tabela) {
  const char *nome;
  char *p;
  size_t i;

  if (campos == NULL || tabela == NULL || quantidade > tabela_colunas(tabela)) {
    return false;
  }
  for (i = 0; i < quantidade; i++) {
    if (campos[i] == NULL || !converter_texto(&campos[i])) {
      return false;
    }
    nome = tabela_campo(tabela, i);
    if (strcmp(nome, "km") == 0 || strcmp(nome, "latitude") == 0 ||
        strcmp(nome, "longitude") == 0) {
      /* A consulta usa ponto decimal; o CSV original conserva suas virgulas. */
      for (p = campos[i]; *p != '\0'; p++) {
        if (*p == ',') {
          *p = '.';
        }
      }
    }
  }
  return true;
}
