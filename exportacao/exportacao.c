#include "exportacao.h"

#include <stdio.h>
#include <string.h>

/* Acrescenta terminador se uma linha sem quebra final mudou de posicao na saida. */
static bool gravar_linha(FILE *arquivo, const char *linha, const char *quebra) {
  size_t tamanho = strlen(linha);

  return fputs(linha, arquivo) != EOF &&
         (tamanho == 0 || linha[tamanho - 1] == '\n' || fputs(quebra, arquivo) != EOF);
}

bool exportacao_gravar(const char *caminho, const TABELA *tabela,
                       COLECAO *resultado, size_t top_k,
                       METRICAS *metricas) {
  FILE *arquivo;
  ITEM *item;
  const char *cabecalho;
  const char *quebra;
  double inicio;
  double fim;
  bool sucesso;

  if (caminho == NULL || tabela == NULL || resultado == NULL || metricas == NULL ||
      !metricas_agora(&inicio)) {
    printf("ERRO=Parametros de exportacao invalidos ou relogio indisponivel.\n");
    return false;
  }
  cabecalho = tabela_cabecalho(tabela);
  quebra = strstr(cabecalho, "\r\n") != NULL ? "\r\n" : "\n";
  arquivo = fopen(caminho, "wb");
  sucesso = arquivo != NULL;
  if (sucesso) {
    sucesso = gravar_linha(arquivo, cabecalho, quebra) && colecao_reiniciar(resultado, 0);
    while (sucesso && (top_k == 0 || metricas->registros_saida < top_k) &&
           (item = colecao_proximo(resultado)) != NULL) {
      sucesso = gravar_linha(arquivo, item_original(item), quebra);
      if (sucesso) {
        metricas->registros_saida++;
      }
    }
    if (fclose(arquivo) != 0) {
      sucesso = false;
    }
  }
  if (metricas_agora(&fim)) {
    metricas->escrita_ms = fim - inicio;
  } else {
    sucesso = false;
  }
  if (!sucesso) {
    printf("ERRO=Falha ao abrir, gravar ou fechar a saida.\n");
  }
  return sucesso;
}
