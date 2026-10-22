#include "csv.h"
#include "../dados/registro.h"

#include <string.h>

/* fgets le blocos; o buffer cresce somente quando o registro precisa de mais espaco. */
bool arquivo_ler_linha(FILE *arquivo, char **linha, bool csv) {
  char bloco[1024];
  char *novo;
  size_t tamanho = 0;
  size_t lidos;
  size_t i;
  bool aspas = false;

  if (arquivo == NULL || linha == NULL) {
    return false;
  }
  *linha = NULL;
  while (fgets(bloco, sizeof(bloco), arquivo) != NULL) {
    lidos = strlen(bloco);
    if (lidos > (size_t)-1 - tamanho - 1) {
      free(*linha);
      *linha = NULL;
      return false;
    }
    novo = realloc(*linha, tamanho + lidos + 1);
    if (novo == NULL) {
      free(*linha);
      *linha = NULL;
      return false;
    }
    *linha = novo;
    memcpy(*linha + tamanho, bloco, lidos + 1);
    tamanho += lidos;
    for (i = 0; csv && i < lidos; i++) {
      if (bloco[i] == '"') {
        aspas = !aspas;
      }
    }
    if (lidos > 0 && bloco[lidos - 1] == '\n' && !aspas) {
      break;
    }
  }
  if (ferror(arquivo) || aspas) {
    free(*linha);
    *linha = NULL;
    return false;
  }
  if (!csv && tamanho > 0) {
    (*linha)[strcspn(*linha, "\r\n")] = '\0';
  }
  return true;
}

/* Avanca posicao ate o separador ou fim e devolve uma nova string decodificada. */
static char *csv_ler_campo(const char *linha, size_t fim, size_t *posicao,
                            char delimitador) {
  size_t inicio = *posicao;
  size_t tamanho = 0;
  char *campo = malloc(fim - inicio + 1);

  if (campo == NULL) {
    return NULL;
  }
  if (inicio < fim && linha[inicio] == '"') {
    (*posicao)++;
    while (*posicao < fim) {
      char caractere = linha[(*posicao)++];

      if (caractere == '"') {
        if (*posicao < fim && linha[*posicao] == '"') {
          /* Duas aspas no arquivo representam uma unica aspa no valor consultavel. */
          campo[tamanho++] = '"';
          (*posicao)++;
        } else {
          if (*posicao < fim && linha[*posicao] != delimitador) {
            free(campo);
            return NULL;
          }
          campo[tamanho] = '\0';
          return campo;
        }
      } else {
        campo[tamanho++] = caractere;
      }
    }
    free(campo);
    return NULL;
  }
  while (*posicao < fim && linha[*posicao] != delimitador) {
    if (linha[*posicao] == '"' || linha[*posicao] == '\r' ||
        linha[*posicao] == '\n') {
      free(campo);
      return NULL;
    }
    campo[tamanho++] = linha[(*posicao)++];
  }
  campo[tamanho] = '\0';
  return campo;
}

bool csv_separar(const char *linha, char delimitador, char ***campos,
                 size_t *quantidade) {
  char **vetor = NULL;
  size_t tamanho = 0;
  size_t posicao = 0;
  size_t fim;

  if (linha == NULL || campos == NULL || quantidade == NULL ||
      delimitador == '\0' || delimitador == '"' || delimitador == '\r' ||
      delimitador == '\n') {
    return false;
  }
  *campos = NULL;
  *quantidade = 0;
  fim = strlen(linha);
  /* Desconsidera apenas o terminador externo; quebras dentro de campos permanecem. */
  if (fim > 0 && linha[fim - 1] == '\n') {
    fim--;
    if (fim > 0 && linha[fim - 1] == '\r') {
      fim--;
    }
  }
  for (;;) {
    /* Uma ultima iteracao apos o delimitador final preserva o ultimo campo vazio. */
    char *campo = csv_ler_campo(linha, fim, &posicao, delimitador);
    char **memoria;

    if (campo == NULL || tamanho >= (size_t)-1 / sizeof(char *)) {
      free(campo);
      campos_apagar(&vetor, tamanho);
      return false;
    }
    memoria = realloc(vetor, (tamanho + 1) * sizeof(char *));
    if (memoria == NULL) {
      free(campo);
      campos_apagar(&vetor, tamanho);
      return false;
    }
    vetor = memoria;
    vetor[tamanho++] = campo;
    if (posicao == fim) {
      break;
    }
    posicao++;
  }
  *campos = vetor;
  *quantidade = tamanho;
  return true;
}

bool csv_linha_vazia(const char *linha) {
  return linha != NULL && (linha[0] == '\0' || strcmp(linha, "\n") == 0 ||
                           strcmp(linha, "\r\n") == 0);
}

bool csv_ler_linha(FILE *arquivo, char **linha) {
  return arquivo_ler_linha(arquivo, linha, true);
}
