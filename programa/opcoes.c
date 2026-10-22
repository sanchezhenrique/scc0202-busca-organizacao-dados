#include "opcoes.h"

#include <stdio.h>
#include <string.h>

/* Conversao digito a digito permite recusar negativos e detectar estouro de size_t. */
static bool ler_natural(const char *texto, size_t *valor) {
  size_t numero = 0;

  if (texto[0] == '\0') {
    return false;
  }
  while (*texto != '\0') {
    unsigned int algarismo;

    if (*texto < '0' || *texto > '9') {
      return false;
    }
    algarismo = (unsigned int)(*texto - '0');
    /* Testa o limite antes da multiplicacao, que ja seria incorreta se transbordasse. */
    if (numero > ((size_t)-1 - algarismo) / 10) {
      return false;
    }
    numero = numero * 10 + algarismo;
    texto++;
  }
  *valor = numero;
  return true;
}

static int identificar_opcao(const char *nome) {
  static const char *const nomes[] = {
    "--reader", "--input", "--base", "--result", "--query", "--output",
    "--top-k", "--limit"
  };
  size_t i;

  for (i = 0; i < sizeof(nomes) / sizeof(nomes[0]); i++) {
    if (strcmp(nome, nomes[i]) == 0) {
      return (int)i;
    }
  }
  return -1;
}

static bool atribuir_opcao(OPCOES *opcoes, int indice, const char *valor) {
  switch (indice) {
    case 0:
      opcoes->leitor = valor;
      break;
    case 1:
      opcoes->entrada = valor;
      break;
    case 2:
      return colecao_tipo(valor, &opcoes->base);
    case 3:
      return colecao_tipo(valor, &opcoes->resultado);
    case 4:
      opcoes->consulta = valor;
      break;
    case 5:
      opcoes->saida = valor;
      break;
    case 6:
      return ler_natural(valor, &opcoes->top_k);
    case 7:
      return ler_natural(valor, &opcoes->limite);
    default:
      return false;
  }
  return true;
}

bool opcoes_ler(int argc, char **argv, OPCOES *opcoes,
                char *erro, size_t capacidade) {
  OPCOES vazias = {NULL, NULL, NULL, NULL, COLECAO_PILHA, COLECAO_PILHA, 0, 0};
  bool presentes[8] = {false};
  int i;

  if (opcoes == NULL || argv == NULL || argc < 1) {
    return false;
  }
  *opcoes = vazias;
  /* Cada opcao consome um par: nome e valor. presentes impede definicoes repetidas. */
  for (i = 1; i < argc; i += 2) {
    int indice = identificar_opcao(argv[i]);

    if (indice < 0 || i + 1 >= argc || presentes[indice] ||
        argv[i + 1][0] == '\0' || !atribuir_opcao(opcoes, indice, argv[i + 1])) {
      if (erro != NULL && capacidade > 0) {
        snprintf(erro, capacidade, "Argumento invalido, repetido ou sem valor: %s", argv[i]);
      }
      return false;
    }
    presentes[indice] = true;
  }
  for (i = 0; i < 6; i++) {
    if (!presentes[i]) {
      if (erro != NULL && capacidade > 0) {
        snprintf(erro, capacidade,
                  "Informe --reader, --input, --base, --result, --query e --output.");
      }
      return false;
    }
  }
  /* Impede sobrescrever diretamente a entrada ou a consulta por engano. */
  if (strcmp(opcoes->entrada, opcoes->saida) == 0 ||
      strcmp(opcoes->consulta, opcoes->saida) == 0) {
    if (erro != NULL && capacidade > 0) {
      snprintf(erro, capacidade, "A saida deve ter caminho diferente da entrada e da consulta.");
    }
    return false;
  }
  return true;
}

void opcoes_imprimir_uso(void) {
  fputs("Uso: ./trabalho --reader nasa|datatran --input arquivo.csv --base estrutura "
        "--result estrutura --query consulta.txt --output resultado.csv "
        "[--top-k N] [--limit N]\n", stderr);
}
