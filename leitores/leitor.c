#include "leitor.h"
#include "csv.h"
#include "datatran.h"

#include <string.h>

/* O leitor possui o arquivo e sua tabela, mas nao os registros ja entregues. */
struct leitor {
  FILE *arquivo;
  TABELA *tabela;
  size_t lidos;
  bool datatran;
};

static void leitor_erro(char *erro, size_t capacidade, const char *mensagem) {
  if (erro != NULL && capacidade > 0) {
    snprintf(erro, capacidade, "%s", mensagem);
  }
}

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
LEITOR *leitor_abrir(const char *nome, const char *caminho,
                     char *erro, size_t capacidade) {
  LEITOR *leitor;

  if (nome == NULL || caminho == NULL) {
    leitor_erro(erro, capacidade, "Leitor e caminho de entrada sao obrigatorios.");
    return NULL;
  }
  if (strcmp(nome, "nasa") != 0 && strcmp(nome, "datatran") != 0) {
    leitor_erro(erro, capacidade,
                "Leitor desconhecido. Use nasa ou datatran.");
    return NULL;
  }
  leitor = malloc(sizeof(LEITOR));
  if (leitor == NULL) {
    leitor_erro(erro, capacidade, "Sem memoria para criar o leitor.");
    return NULL;
  }
  leitor->arquivo = fopen(caminho, "rb");
  if (leitor->arquivo == NULL) {
    leitor_erro(erro, capacidade, "Nao foi possivel abrir o arquivo de entrada.");
    free(leitor);
    return NULL;
  }
  leitor->datatran = strcmp(nome, "datatran") == 0;
  leitor->tabela = ler_cabecalho(leitor->arquivo, leitor->datatran);
  if (leitor->tabela == NULL) {
    leitor_erro(erro, capacidade,
                "Cabecalho incompativel com o leitor, erro de leitura ou falta de memoria.");
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

bool leitor_proximo(LEITOR *leitor, REGISTRO **registro,
                     char *erro, size_t capacidade) {
  char *linha = NULL;
  char **campos = NULL;
  size_t quantidade = 0;
  bool valido;

  if (leitor == NULL || registro == NULL) {
    leitor_erro(erro, capacidade, "Leitor ou referencia de registro nula.");
    return false;
  }
  *registro = NULL;
  /* Linhas em branco nao contam como registros e sao descartadas antes do parse. */
  do {
    free(linha);
    linha = NULL;
    if (!csv_ler_linha(leitor->arquivo, &linha)) {
      leitor_erro(erro, capacidade, "Erro de leitura, CSV invalido ou falta de memoria.");
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
    /* registro_criar assume linha e campos apenas se conseguir criar o TAD. */
    *registro = registro_criar(linha, campos, quantidade, leitor->lidos);
  }
  if (*registro == NULL) {
    /* Se a criacao falhou, as alocacoes ainda pertencem ao leitor. */
    campos_apagar(&campos, quantidade);
    free(linha);
    leitor_erro(erro, capacidade,
                "Registro CSV invalido, colunas excedentes ou memoria insuficiente.");
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
