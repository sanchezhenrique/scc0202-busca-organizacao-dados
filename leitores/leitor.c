#include "leitor.h"
#include "csv.h"
#include "datatran.h"

#include <stdio.h>
#include <string.h>

/* O leitor possui o arquivo e sua tabela, mas nao os itens ja entregues. */
struct leitor {
  FILE *arquivo;
  TABELA *tabela;
  size_t lidos;
  bool datatran;
};

static TABELA *ler_cabecalho(FILE *arquivo, bool datatran) {
  char *linha = NULL;
  char **campos = NULL;
  size_t quantidade = 0;
  size_t indice;
  TABELA *tabela;
  char delimitador = datatran ? ';' : ',';

  if (!csv_ler_linha(arquivo, &linha) || linha == NULL) {
    return NULL;
  }
  /* Cada leitor usa o delimitador observado no respectivo arquivo fornecido. */
  if (!csv_separar(linha, delimitador, &campos, &quantidade)) {
    free(linha);
    return NULL;
  }
  tabela = tabela_criar(linha, campos, quantidade, delimitador);
  if (tabela == NULL) {
    campos_apagar(&campos, quantidade);
    free(linha);
    return NULL;
  }
  if (datatran) {
    if (!datatran_validar(tabela)) {
      tabela_apagar(&tabela);
    }
  } else {
    if (!tabela_indice(tabela, "asteroid_designation", &indice) ||
        !tabela_indice(tabela, "close_approach_date", &indice) ||
        !tabela_indice(tabela, "distance_au", &indice)) {
      tabela_apagar(&tabela);
    }
  }
  return tabela;
}

/* A selecao do dominio fica aqui para que os TADs nao dependam de asteroides. */
LEITOR *leitor_abrir(const char *nome, const char *caminho) {
  LEITOR *leitor;

  if (nome == NULL || caminho == NULL) {
    printf("ERRO=Leitor e caminho de entrada sao obrigatorios.\n");
    return NULL;
  }
  if (strcmp(nome, "nasa") != 0 && strcmp(nome, "datatran") != 0) {
    printf("ERRO=Leitor desconhecido. Use nasa ou datatran.\n");
    return NULL;
  }
  leitor = malloc(sizeof(LEITOR));
  if (leitor == NULL) {
    printf("ERRO=Sem memoria para criar o leitor.\n");
    return NULL;
  }
  leitor->arquivo = fopen(caminho, "rb");
  if (leitor->arquivo == NULL) {
    printf("ERRO=Nao foi possivel abrir o arquivo de entrada.\n");
    free(leitor);
    return NULL;
  }
  leitor->datatran = strcmp(nome, "datatran") == 0;
  leitor->tabela = ler_cabecalho(leitor->arquivo, leitor->datatran);
  if (leitor->tabela == NULL) {
    printf("ERRO=Cabecalho incompativel com o leitor, erro de leitura ou falta de memoria.\n");
    fclose(leitor->arquivo);
    free(leitor);
    return NULL;
  }
  leitor->lidos = 0;
  return leitor;
}

const TABELA *leitor_tabela(const LEITOR *leitor) {
  return leitor != NULL ? leitor->tabela : NULL;
}

bool leitor_proximo(LEITOR *leitor, ITEM **item) {
  char *linha = NULL;
  char **campos = NULL;
  size_t quantidade = 0;
  bool valido;

  if (leitor == NULL || item == NULL) {
    printf("ERRO=Leitor ou referencia de item nula.\n");
    return false;
  }
  *item = NULL;
  /* Linhas em branco nao contam como itens e sao descartadas antes do parse. */
  do {
    free(linha);
    linha = NULL;
    if (!csv_ler_linha(leitor->arquivo, &linha)) {
      printf("ERRO=Erro de leitura, CSV invalido ou falta de memoria.\n");
      return false;
    }
  } while (csv_linha_vazia(linha));
  if (linha == NULL) {
    return true;
  }
  valido = csv_separar(linha, tabela_delimitador(leitor->tabela),
                        &campos, &quantidade);
  if (valido) {
    valido = quantidade <= tabela_colunas(leitor->tabela);
  }
  if (valido && leitor->datatran) {
    /* A adaptacao fica no leitor: busca, consultas e TADs continuam os mesmos. */
    valido = datatran_tratar(campos, quantidade, leitor->tabela);
  }
  if (valido && leitor->lidos != (size_t)-1) {
    /* item_criar assume linha e campos apenas se conseguir criar o TAD. */
    *item = item_criar(linha, campos, quantidade, leitor->lidos);
  }
  if (*item == NULL) {
    /* Se a criacao falhou, as alocacoes ainda pertencem ao leitor. */
    campos_apagar(&campos, quantidade);
    free(linha);
    printf("ERRO=Registro CSV invalido, colunas excedentes ou memoria insuficiente.\n");
    return false;
  }
  leitor->lidos++;
  return true;
}

void leitor_apagar(LEITOR **leitor) {
  if (leitor == NULL || *leitor == NULL) {
    return;
  }
  fclose((*leitor)->arquivo);
  tabela_apagar(&(*leitor)->tabela);
  free(*leitor);
  *leitor = NULL;
}
